/*
 * testAsynMaskShift.h
 *
 * Asyn driver that inherits from the asynPortDriver class to demonstrate asynMaskShift functionality.
 * asynMaskShift allows asyn DTYP PVs to include a shift parameter
 *
 * Author: Tynan Ford
 *
 * Created Jun. 4, 2024
 */

#include "asynPortDriver.h"

/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
#define P_LongRBVString            "TEST_LONG_RBV"             /* asynUInt32Digital,    r/o */
#define P_LongString               "TEST_LONG"                 /* asynUInt32Digital,    r/w */
#define P_ShiftLongRBVString       "TEST_LONG_SHIFT_RBV"       /* asynUInt32Digital,    r/o */
#define P_ShiftLongString          "TEST_LONG_SHIFT"           /* asynUInt32Digital,    r/w */

#define P_LongCallbackRBVString            "TEST_CALLBACK_LONG_RBV"             /* asynUInt32Digital,    r/o */
#define P_LongCallbackString               "TEST_CALLBACK_LONG"                 /* asynUInt32Digital,    r/w */
#define P_ShiftLongCallbackRBVString       "TEST_CALLBACK_LONG_SHIFT_RBV"       /* asynUInt32Digital,    r/o */
#define P_ShiftLongCallbackString          "TEST_CALLBACK_LONG_SHIFT"           /* asynUInt32Digital,    r/w */

#define P_BoolRBVString            "TEST_BOOL_RBV"             /* asynUInt32Digital,    r/o */
#define P_BoolString               "TEST_BOOL"                 /* asynUInt32Digital,    r/w */
#define P_ShiftBoolRBVString       "TEST_BOOL_SHIFT_RBV"       /* asynUInt32Digital,    r/o */
#define P_ShiftBoolString          "TEST_BOOL_SHIFT"           /* asynUInt32Digital,    r/w */

#define P_BoolCallbackRBVString            "TEST_CALLBACK_BOOL_RBV"             /* asynUInt32Digital,    r/o */
#define P_BoolCallbackString               "TEST_CALLBACK_BOOL"                 /* asynUInt32Digital,    r/w */
#define P_ShiftBoolCallbackRBVString       "TEST_CALLBACK_BOOL_SHIFT_RBV"       /* asynUInt32Digital,    r/o */
#define P_ShiftBoolCallbackString          "TEST_CALLBACK_BOOL_SHIFT"           /* asynUInt32Digital,    r/w */

/** Class that demonstrates the use of the asynPortDriver base class to greatly simplify the task
  * of writing an asyn port driver.
  * This class does a simple simulation of a digital oscilloscope.  It computes a waveform, computes
  * statistics on the waveform, and does callbacks with the statistics and the waveform data itself.
  * I have made the methods of this class public in order to generate doxygen documentation for them,
  * but they should really all be private. */
class testMaskShift : public asynPortDriver {
public:
    testMaskShift(const char *portName);

    /* These are the methods that are new to this class */
    void simTask(void);

protected:
    /** Values used for pasynUser->reason, and indexes into the parameter library. */
    int P_Long;
    int P_LongRBV;
    int P_ShiftLong;
    int P_ShiftLongRBV;

    int P_LongCallback;
    int P_LongCallbackRBV;
    int P_ShiftLongCallback;
    int P_ShiftLongCallbackRBV;

    int P_Bool;
    int P_BoolRBV;
    int P_ShiftBool;
    int P_ShiftBoolRBV;

    int P_BoolCallback;
    int P_BoolCallbackRBV;
    int P_ShiftBoolCallback;
    int P_ShiftBoolCallbackRBV;

private:
};
