#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS += configure
DIRS += makeSupport
DIRS += opi

DIRS += asyn
asyn_DEPEND_DIRS = configure
DIRS += asyn/asynPortDriver/unittest
asyn/asynPortDriver/unittest_DEPEND_DIRS = asyn

ifneq ($(EPICS_LIBCOM_ONLY),YES)
  DIRS += testApp
  testApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testApp
  DIRS += testArrayRingBufferApp
  testArrayRingBufferApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testArrayRingBufferApp
  DIRS += testAsynPortDriverApp
  testAsynPortDriverApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testAsynPortDriverApp
  DIRS += testBroadcastApp
  testBroadcastApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testBroadcastApp
  DIRS += testConnectApp
  testConnectApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testConnectApp
  DIRS += testEpicsApp
  testEpicsApp_DEPEND_DIRS = testApp asyn
  iocBoot_DEPEND_DIRS += testEpicsApp
  DIRS += testErrorsApp
  testErrorsApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testErrorsApp
  DIRS += testGpibApp
  testGpibApp_DEPEND_DIRS = testApp asyn
  iocBoot_DEPEND_DIRS += testGpibApp
  DIRS += testGpibSerialApp
  testGpibSerialApp_DEPEND_DIRS = testApp testGpibApp
  iocBoot_DEPEND_DIRS += testGpibSerialApp
  DIRS += testMaskShiftApp
  testMaskShiftApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testMaskShiftApp
  ifdef SNCSEQ
    DIRS += testIPServerApp
    testIPServerApp_DEPEND_DIRS = asyn
    iocBoot_DEPEND_DIRS += testIPServerApp
  endif
  DIRS += testManagerApp
  testManagerApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testManagerApp
  DIRS += testOutputReadbackApp
  testOutputReadbackApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testOutputReadbackApp
  DIRS += testOutputCallbackApp
  testOutputCallbackApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testOutputCallbackApp
  DIRS += testUsbtmcApp
  testUsbtmcApp_DEPEND_DIRS = asyn
  iocBoot_DEPEND_DIRS += testUsbtmcApp
  DIRS += iocBoot
  ifeq ($(DRV_FTDI),YES)
    DIRS += testFtdiApp
    testFtdiApp_DEPEND_DIRS = asyn
  endif
endif

DIRS += testAsynPortClientApp
testAsynPortClientApp_DEPEND_DIRS = asyn

include $(TOP)/configure/RULES_TOP
