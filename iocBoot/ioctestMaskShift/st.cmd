#!../../bin/linux-x86_64/testMaskShift
##!../../bin/linux-x86_64-debug/testMaskShift

< envPaths

dbLoadDatabase("../../dbd/testMaskShift.dbd")
testMaskShift_registerRecordDeviceDriver(pdbbase)

# Turn on asynTraceFlow and asynTraceError for global trace, i.e. no connected asynUser.
#asynSetTraceMask("", 0, 17)

testMaskShiftConfigure("testSM")

dbLoadRecords("../../db/testMaskShift.db","P=testSM:,R=1:,PORT=testSM,ADDR=0,TIMEOUT=1")
dbLoadRecords("../../db/asynRecord.db","P=testSM:,R=asyn1,PORT=testSM,ADDR=0,OMAX=80,IMAX=80")
#asynSetTraceMask("testSM",0,0xff)
asynSetTraceIOMask("testSM",0,0x2)
iocInit()
