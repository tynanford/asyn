/* devAsynAnalog.c */
/***********************************************************************
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory, and the Regents of the University of
* California, as Operator of Los Alamos National Laboratory, and
* Berliner Elektronenspeicherring-Gesellschaft m.b.H. (BESSY).
* asynDriver is distributed subject to a Software License Agreement
* found in file LICENSE that is included with this distribution.
***********************************************************************/
/*
    Author:  Mark Rivers and Marty Kraimer
    14-OCT-2004

*/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <alarm.h>
#include <callback.h>
#include <recGbl.h>
#include <dbAccess.h>
#include <dbDefs.h>
#include <link.h>
#include <epicsPrint.h>
#include <epicsExport.h>
#include <epicsMutex.h>
#include <cantProceed.h>
#include <dbCommon.h>
#include <dbScan.h>
#include <aoRecord.h>
#include <aiRecord.h>
#include <recSup.h>
#include <devSup.h>

#include "asynDriver.h"
#include "asynDrvUser.h"
#include "asynFloat64.h"
#include "asynFloat64SyncIO.h"
#include "asynEpicsUtils.h"

typedef enum {
    typeAiFloat64,
    typeAiFloat64Average,
    typeAiFloat64Interrupt,
    typeAoFloat64
}asynAnalogDevType;

typedef struct devPvt{
    dbCommon          *pr;
    asynUser          *pasynUser;
    asynFloat64       *pfloat64;
    void              *float64Pvt;
    void              *registrarPvt;
    int               canBlock;
    asynAnalogDevType devType;
    epicsMutexId      mutexId;
    int               gotValue;
    epicsFloat64      value;
    epicsFloat64      sum;
    int               numAverage;
    CALLBACK          callback;
    IOSCANPVT         ioScanPvt;
    char              *portName;
    char              *userParam;
    int               addr;
}devPvt;

static long initCommon(dbCommon *pr, DBLINK *plink,
    userCallback processCallback,interruptCallbackFloat64 interruptCallback);
static long getIoIntInfo(int cmd, dbCommon *pr, IOSCANPVT *iopvt);
/* asyn callbacks */
static void interruptCallbackInput(void *drvPvt, epicsFloat64 value);
static void interruptCallbackOutput(void *drvPvt, epicsFloat64 value);
static void interruptCallbackAverage(void *drvPvt, epicsFloat64 value);

static long initAi(aiRecord *pai);
static long initAo(aoRecord *pai);
static long initAiAverage(aiRecord *pai);
/* process callbacks */
static long processAi(aiRecord *pai);
static long processAo(aoRecord *pai);
static long processAiAverage(aiRecord *pai);
/*queue process Callbacks*/
static void processCallbackAi(asynUser *pasynUser);
static void processCallbackAo(asynUser *pasynUser);

typedef struct analogDset { /* analog  dset */
    long          number;
    DEVSUPFUN     dev_report;
    DEVSUPFUN     init;
    DEVSUPFUN     init_record; 
    DEVSUPFUN     get_ioint_info;
    DEVSUPFUN     processCommon;/*(0)=>(success ) */
    DEVSUPFUN     special_linconv;
} analogDset;

analogDset asynAiFloat64 = {
    6, 0, 0, initAi, getIoIntInfo, processAi, 0};
analogDset asynAoFloat64 = {
    6, 0, 0, initAo, getIoIntInfo, processAo, 0};
analogDset asynAiFloat64Average = {
    6, 0, 0, initAiAverage, getIoIntInfo, processAiAverage, 0};

epicsExportAddress(dset, asynAiFloat64);
epicsExportAddress(dset, asynAoFloat64);
epicsExportAddress(dset, asynAiFloat64Average);

static long initCommon(dbCommon *pr, DBLINK *plink,
    userCallback processCallback,interruptCallbackFloat64 interruptCallback)
{
    devPvt *pPvt;
    asynStatus status;
    asynUser *pasynUser;
    asynInterface *pasynInterface;

    pPvt = callocMustSucceed(1, sizeof(*pPvt), "devAsynFloat64::initCommon");
    pr->dpvt = pPvt;
    pPvt->pr = pr;
    /* Create asynUser */
    pasynUser = pasynManager->createAsynUser(processCallback, 0);
    pasynUser->userPvt = pPvt;
    pPvt->pasynUser = pasynUser;
    pPvt->mutexId = epicsMutexCreate();
    /* Parse the link to get addr and port */
    status = pasynEpicsUtils->parseLink(pasynUser, plink,
                &pPvt->portName, &pPvt->addr,&pPvt->userParam);
    if (status != asynSuccess) {
        printf("devAsynFloat64::initCommon, %s error in link %s\n",
                     pr->name, pasynUser->errorMessage);
        goto bad;
    }
    /* Connect to device */
    status = pasynManager->connectDevice(pasynUser, pPvt->portName, pPvt->addr);
    if (status != asynSuccess) {
        printf("devAsynFloat64::initCommon, %s connectDevice failed %s\n",
                     pr->name, pasynUser->errorMessage);
        goto bad;
    }
    /*call drvUserCreate*/
    pasynInterface = pasynManager->findInterface(pasynUser,asynDrvUserType,1);
    if(pasynInterface && pPvt->userParam) {
        asynDrvUser *pasynDrvUser;
        void       *drvPvt;

        pasynDrvUser = (asynDrvUser *)pasynInterface->pinterface;
        drvPvt = pasynInterface->drvPvt;
        status = pasynDrvUser->create(drvPvt,pasynUser,pPvt->userParam,0,0);
        if(status!=asynSuccess) {
            printf("devAsynFloat64::initCommon, %s drvUserCreate failed %s\n",
                     pr->name, pasynUser->errorMessage);
            goto bad;
        }
    }
    /* Get interface asynFloat64 */
    pasynInterface = pasynManager->findInterface(pasynUser, asynFloat64Type, 1);
    if (!pasynInterface) {
        printf("devAsynFloat64::initCommon, %s findInterface asynFloat64Type failed\n",
                     pr->name);
        goto bad;
    }
    pPvt->pfloat64 = pasynInterface->pinterface;
    pPvt->float64Pvt = pasynInterface->drvPvt;
    scanIoInit(&pPvt->ioScanPvt);
    status = pPvt->pfloat64->registerInterruptUser(
        pPvt->float64Pvt,pPvt->pasynUser,
        interruptCallback,pPvt,&pPvt->registrarPvt);
    if(status!=asynSuccess) {
        printf("%s devAsynFloat64 registerInterruptUser failed %s\n",
            pr->name,pPvt->pasynUser->errorMessage);
    }
    return 0;

bad:
   pr->pact=1;
   return(-1);
}

static long getIoIntInfo(int cmd, dbCommon *pr, IOSCANPVT *iopvt)
{
    devPvt *pPvt = (devPvt *)pr->dpvt;

    *iopvt = pPvt->ioScanPvt;
    return 0;
}

static void interruptCallbackInput(void *drvPvt, epicsFloat64 value)
{
    devPvt *pPvt = (devPvt *)drvPvt;
    dbCommon *pr = pPvt->pr;

    asynPrint(pPvt->pasynUser, ASYN_TRACEIO_DEVICE,
        "%s devAsynFloat64::interruptCallbackInput new value=%f\n",
        pr->name, value);
    epicsMutexLock(pPvt->mutexId);
    pPvt->gotValue = 1; pPvt->value = value;
    epicsMutexUnlock(pPvt->mutexId);
    scanIoRequest(pPvt->ioScanPvt);
}

static void interruptCallbackOutput(void *drvPvt, epicsFloat64 value)
{
    devPvt *pPvt = (devPvt *)drvPvt;
    dbCommon *pr = pPvt->pr;

    if(pPvt->gotValue) return;
    asynPrint(pPvt->pasynUser, ASYN_TRACEIO_DEVICE,
        "%s devAsynFloat64::interruptCallbackInput new value=%f\n",
        pr->name, value);
    epicsMutexLock(pPvt->mutexId);
    pPvt->gotValue = 1; pPvt->value = value;
    epicsMutexUnlock(pPvt->mutexId);
    scanOnce(pr);
}

static void interruptCallbackAverage(void *drvPvt, epicsFloat64 value)
{
    devPvt *pPvt = (devPvt *)drvPvt;
    dbCommon *pr = pPvt->pr;

    asynPrint(pPvt->pasynUser, ASYN_TRACEIO_DEVICE,
        "%s devAsynFloat64::interruptCallbackInput new value=%lu\n",
        pr->name, value);
    epicsMutexLock(pPvt->mutexId);
    pPvt->numAverage++;
    pPvt->sum += value;
    epicsMutexUnlock(pPvt->mutexId);
}

static long initAi(aiRecord *pai)
{
    devPvt *pPvt = (devPvt *)pai->dpvt;
    asynStatus status;

    status = initCommon((dbCommon *)pai,&pai->inp,
        processCallbackAi,interruptCallbackInput);
    if (status != asynSuccess) return 0;
    pPvt = pai->dpvt;
    pasynManager->canBlock(pPvt->pasynUser, &pPvt->canBlock);
    return(0);
}

static long processAi(aiRecord *pr)
{
    devPvt *pPvt = (devPvt *)pr->dpvt;
    int status;

    if(pPvt->gotValue) {
        pr->val = pPvt->value;
        pPvt->gotValue = 0;
    } else {
        if(pr->pact == 0) {
            status = pasynManager->queueRequest(pPvt->pasynUser, 0, 0);
            if((status==asynSuccess) && pPvt->canBlock) {
                 pr->pact = 1;
                 return 0;
            }
            if(status != asynSuccess) {
                asynPrint(pPvt->pasynUser, ASYN_TRACE_ERROR,
                    "%s devAsynFloat64::process error queuing request %s\n",
                    pr->name,pPvt->pasynUser->errorMessage);
            }
        }
    }
    return 0;
}

static void processCallbackAi(asynUser *pasynUser)
{
    devPvt *pPvt = (devPvt *)pasynUser->userPvt;
    dbCommon *pr = (dbCommon *)pPvt->pr;
    int status=asynSuccess;

    status = pPvt->pfloat64->read(pPvt->float64Pvt, pPvt->pasynUser,
        &pPvt->value);
    if (status == asynSuccess) {
        asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
            "%s devAsynFloat64::process val=%f\n",
            pr->name,pPvt->value);
        pr->udf=0;
    } else {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
            "%s devAsynFloat64::process read error %s\n",
            pr->name, pasynUser->errorMessage);
        recGblSetSevr(pr, READ_ALARM, INVALID_ALARM);
    }
    if(pr->pact) callbackRequestProcessCallback(&pPvt->callback,pr->prio,pr);
}

static long initAo(aoRecord *pao)
{
    devPvt *pPvt;
    asynStatus status;
    epicsFloat64 value;

    status = initCommon((dbCommon *)pao,&pao->out,
        processCallbackAo,interruptCallbackOutput);
    if (status != asynSuccess) return 0;
    pPvt = pao->dpvt;
    /* Read the current value from the device */
    status = pasynFloat64SyncIO->readOnce(pPvt->portName,pPvt->addr,
                      &value,pPvt->pasynUser->timeout);
    if (status == asynSuccess) {
        pao->val = value;
        pao->udf = 0;
        return 0;
    }
    return 2; /* Do not convert */
}

static long processAo(aoRecord *pr)
{
    devPvt *pPvt = (devPvt *)pr->dpvt;
    int status;

    if(pPvt->gotValue) {
        pr->val = pPvt->value;
        pr->udf = 0;
    } else if(pr->pact == 0) {
        pPvt->gotValue = 1; pPvt->value = pr->val;
        status = pasynManager->queueRequest(pPvt->pasynUser, 0, 0);
        if((status==asynSuccess) && pPvt->canBlock) {
             pr->pact = 1;
             return 0;
        }
        if(status != asynSuccess) {
            asynPrint(pPvt->pasynUser, ASYN_TRACE_ERROR,
                "%s devAsynFloat64:process error queuing request %s\n",
                pr->name,pPvt->pasynUser->errorMessage);
        }
    }
    pPvt->gotValue = 0;
    return 0;
}
static void processCallbackAo(asynUser *pasynUser)
{
    devPvt *pPvt = (devPvt *)pasynUser->userPvt;
    dbCommon *pr = pPvt->pr;
    aoRecord *pao = (aoRecord *)pPvt->pr;
    int status=asynSuccess;

    status = pPvt->pfloat64->write(pPvt->float64Pvt, pPvt->pasynUser,
        pao->val);
    if(status == asynSuccess) {
        pr->udf=0;
        asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
            "%s devAsynFloat64 process val %f\n",pr->name,pao->val);
    } else {
       asynPrint(pasynUser, ASYN_TRACE_ERROR,
           "%s devAsynFloat64 process error %s\n",
           pr->name, pasynUser->errorMessage);
       recGblSetSevr(pr, WRITE_ALARM, INVALID_ALARM);
    }
    if(pr->pact) callbackRequestProcessCallback(&pPvt->callback,pr->prio,pr);
}

static long initAiAverage(aiRecord *pai)
{
    devPvt *pPvt = (devPvt *)pai->dpvt;
    asynStatus status;

    status = initCommon((dbCommon *)pai,&pai->inp,
        0,interruptCallbackInput);
    pasynManager->canBlock(pPvt->pasynUser, &pPvt->canBlock);
    return(0);
}

static long processAiAverage(aiRecord *pai)
{
    devPvt *pPvt = (devPvt *)pai->dpvt;

    epicsMutexLock(pPvt->mutexId);
    if (pPvt->numAverage == 0) pPvt->numAverage = 1;
    pai->val = pPvt->sum/pPvt->numAverage;
    pPvt->numAverage = 0;
    pPvt->sum = 0.;
    epicsMutexUnlock(pPvt->mutexId);
    asynPrint(pPvt->pasynUser, ASYN_TRACEIO_DEVICE,
              "%s devAsynAnalog::callbackAiAverage val=%f\n",
              pai->name, pai->val);
    return 0;
}
