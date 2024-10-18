TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

DBD += testMaskShift.dbd

LIBRARY_IOC += testMaskShiftSupport
testMaskShiftSupport_SRCS += testMaskShift.cpp
testMaskShiftSupport_LIBS += asyn
testMaskShiftSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

PROD_IOC += testMaskShift

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
testMaskShift_SRCS_DEFAULT += testMaskShift_registerRecordDeviceDriver.cpp testMaskShiftMain.cpp
testMaskShiftVx_SRCS_vxWorks  += testMaskShift_registerRecordDeviceDriver.cpp
testMaskShift_LIBS += testMaskShiftSupport asyn
testMaskShift_LIBS += $(EPICS_BASE_IOC_LIBS)

testMaskShift_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
