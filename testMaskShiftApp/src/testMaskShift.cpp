/*
 * testMaskShift.cpp
 *
 *
 * Author: Tynan Ford
 *
 * Created Jun. 4, 2024
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include "testMaskShift.h"
#include <epicsExport.h>

static const char *driverName="testMaskShift";
void simTask(void *drvPvt);


/** Constructor for the testMaskShift class.
  * Calls constructor for the asynPortDriver base class.
  * \param[in] portName The name of the asyn port driver to be created. */
testMaskShift::testMaskShift(const char *portName)
   : asynPortDriver(portName,
                    1, /* maxAddr */
                    asynInt32Mask | asynUInt32DigitalMask | asynEnumMask | asynDrvUserMask, /* Interface mask */
                    asynInt32Mask | asynUInt32DigitalMask | asynEnumMask,  /* Interrupt mask */
                    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
                    1, /* Autoconnect */
                    0, /* Default priority */
                    0) /* Default stack size*/
{
    asynStatus status;
    int i;
    const char *functionName = "testMaskShift";

    createParam(P_LongString,          asynParamUInt32Digital,       &P_Long);
    createParam(P_LongRBVString,       asynParamUInt32Digital,       &P_LongRBV);
    createParam(P_ShiftLongString,     asynParamUInt32Digital,       &P_ShiftLong);
    createParam(P_ShiftLongRBVString,  asynParamUInt32Digital,       &P_ShiftLongRBV);

    createParam(P_LongCallbackString,          asynParamUInt32Digital,       &P_LongCallback);
    createParam(P_LongCallbackRBVString,       asynParamUInt32Digital,       &P_LongCallbackRBV);
    createParam(P_ShiftLongCallbackString,     asynParamUInt32Digital,       &P_ShiftLongCallback);
    createParam(P_ShiftLongCallbackRBVString,  asynParamUInt32Digital,       &P_ShiftLongCallbackRBV);

    createParam(P_BoolString,          asynParamUInt32Digital,       &P_Bool);
    createParam(P_BoolRBVString,       asynParamUInt32Digital,       &P_BoolRBV);
    createParam(P_ShiftBoolString,     asynParamUInt32Digital,       &P_ShiftBool);
    createParam(P_ShiftBoolRBVString,  asynParamUInt32Digital,       &P_ShiftBoolRBV);

    createParam(P_BoolCallbackString,          asynParamUInt32Digital,       &P_BoolCallback);
    createParam(P_BoolCallbackRBVString,       asynParamUInt32Digital,       &P_BoolCallbackRBV);
    createParam(P_ShiftBoolCallbackString,     asynParamUInt32Digital,       &P_ShiftBoolCallback);
    createParam(P_ShiftBoolCallbackRBVString,  asynParamUInt32Digital,       &P_ShiftBoolCallbackRBV);

    createParam(P_MbbString,          asynParamUInt32Digital,       &P_Mbb);
    createParam(P_MbbRBVString,       asynParamUInt32Digital,       &P_MbbRBV);

    createParam(P_MbbCallbackString,          asynParamUInt32Digital,       &P_MbbCallback);
    createParam(P_MbbCallbackRBVString,       asynParamUInt32Digital,       &P_MbbCallbackRBV);

    createParam(P_MbbdirectString,          asynParamUInt32Digital,       &P_Mbbdirect);
    createParam(P_MbbdirectRBVString,       asynParamUInt32Digital,       &P_MbbdirectRBV);

    createParam(P_MbbdirectCallbackString,          asynParamUInt32Digital,       &P_MbbdirectCallback);
    createParam(P_MbbdirectCallbackRBVString,       asynParamUInt32Digital,       &P_MbbdirectCallbackRBV);


    /* Set the initial values of some parameters */
    setUIntDigitalParam(P_Long, 0, 0xFFFFFFFF);
    setUIntDigitalParam(P_ShiftLong, 0, 0xFFFFFFFF);

    /* Create the thread that computes the waveforms in the background */
    status = (asynStatus)(epicsThreadCreate("testMaskShiftTask",
                          epicsThreadPriorityMedium,
                          epicsThreadGetStackSize(epicsThreadStackMedium),
                          (EPICSTHREADFUNC)::simTask,
                          this) == NULL);
    if (status) {
        printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
        return;
    }
}

void simTask(void *drvPvt)
{
    testMaskShift *pPvt = (testMaskShift *)drvPvt;

    pPvt->simTask();
}

void testMaskShift::simTask(void)
{
    /* This thread computes the waveform and does callbacks with it */
    const char* functionName = "simTask";

    epicsUInt32 longValue = 1;
    epicsUInt32 shiftLongValue = 1;
    epicsUInt32 longCallbackValue = 1;
    epicsUInt32 shiftLongCallbackValue = 1;

    epicsUInt32 boolValue = 1;
    epicsUInt32 shiftBoolValue = 1;
    epicsUInt32 boolCallbackValue = 1;
    epicsUInt32 shiftBoolCallbackValue = 1;

    epicsUInt32 mbbValue = 1;
    epicsUInt32 mbbCallbackValue = 1;

    epicsUInt32 mbbdirectValue = 1;
    epicsUInt32 mbbdirectCallbackValue = 1;

    /*
     * Loop forever, sleeping for 3 seconds after each iteration.
     * In each iteration, fetch the "setpoint" PV values from the asyn parameters and write them to the "readback" PVs.
     * This simulates a hardware driver that has a "setpoint" register that contains multiple setpoint PVs
     * Each time the user sets a new value in the setpoint PV, the driver writes the value to the hardware and expects
     * the hardware to respond with the updated value for the readback PV.
     */
    while (1) {
        lock();
        getUIntDigitalParam(P_Long, &longValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_LongRBV, longValue, 0xFFFFFFFF);
        getUIntDigitalParam(P_ShiftLong, &shiftLongValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_ShiftLongRBV, shiftLongValue, 0xFFFFFFFF);

        getUIntDigitalParam(P_LongCallback, &longCallbackValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_LongCallbackRBV, longCallbackValue, 0xFFFFFFFF);
        getUIntDigitalParam(P_ShiftLongCallback, &shiftLongCallbackValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_ShiftLongCallbackRBV, shiftLongCallbackValue, 0xFFFFFFFF);

        getUIntDigitalParam(P_Bool, &boolValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_BoolRBV, boolValue, 0xFFFFFFFF);
        getUIntDigitalParam(P_ShiftBool, &shiftBoolValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_ShiftBoolRBV, shiftBoolValue, 0xFFFFFFFF);

        getUIntDigitalParam(P_BoolCallback, &boolCallbackValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_BoolCallbackRBV, boolCallbackValue, 0xFFFFFFFF);
        getUIntDigitalParam(P_ShiftBoolCallback, &shiftBoolCallbackValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_ShiftBoolCallbackRBV, shiftBoolCallbackValue, 0xFFFFFFFF);

        getUIntDigitalParam(P_Mbb, &mbbValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_MbbRBV, mbbValue, 0xFFFFFFFF);

        getUIntDigitalParam(P_MbbCallback, &mbbCallbackValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_MbbCallbackRBV, mbbCallbackValue, 0xFFFFFFFF);

        getUIntDigitalParam(P_Mbbdirect, &mbbdirectValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_MbbdirectRBV, mbbdirectValue, 0xFFFFFFFF);

        getUIntDigitalParam(P_MbbdirectCallback, &mbbdirectCallbackValue, 0xFFFFFFFF);
        setUIntDigitalParam(P_MbbdirectCallbackRBV, mbbdirectCallbackValue, 0xFFFFFFFF);

        callParamCallbacks();
        unlock();
        epicsThreadSleep(3.0);
    }
}

/* Configuration routine.  Called directly, or from the iocsh function below */
extern "C" {

/** EPICS iocsh callable function to call constructor for the testMaskShift class.
  * \param[in] portName The name of the asyn port driver to be created. */
int testMaskShiftConfigure(const char *portName)
{
    new testMaskShift(portName);
    return(asynSuccess);
}


/* EPICS iocsh shell commands */

static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"testMaskShiftConfigure",1,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    testMaskShiftConfigure(args[0].sval);
}

void testMaskShiftRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

epicsExportRegistrar(testMaskShiftRegister);

}

