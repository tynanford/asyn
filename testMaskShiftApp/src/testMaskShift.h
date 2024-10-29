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
#define P_LongRBVString                 "TEST_LONG_RBV"                 /* asynUInt32Digital,    r/o */
#define P_LongString                    "TEST_LONG"                     /* asynUInt32Digital,    r/w */
#define P_ShiftLongRBVString            "TEST_LONG_SHIFT_RBV"           /* asynUInt32Digital,    r/o */
#define P_ShiftLongString               "TEST_LONG_SHIFT"               /* asynUInt32Digital,    r/w */

#define P_LongCallbackRBVString         "TEST_CALLBACK_LONG_RBV"        /* asynUInt32Digital,    r/o */
#define P_LongCallbackString            "TEST_CALLBACK_LONG"            /* asynUInt32Digital,    r/w */
#define P_ShiftLongCallbackRBVString    "TEST_CALLBACK_LONG_SHIFT_RBV"  /* asynUInt32Digital,    r/o */
#define P_ShiftLongCallbackString       "TEST_CALLBACK_LONG_SHIFT"      /* asynUInt32Digital,    r/w */

#define P_BoolRBVString                 "TEST_BOOL_RBV"                 /* asynUInt32Digital,    r/o */
#define P_BoolString                    "TEST_BOOL"                     /* asynUInt32Digital,    r/w */
#define P_ShiftBoolRBVString            "TEST_BOOL_SHIFT_RBV"           /* asynUInt32Digital,    r/o */
#define P_ShiftBoolString               "TEST_BOOL_SHIFT"               /* asynUInt32Digital,    r/w */

#define P_BoolCallbackRBVString         "TEST_CALLBACK_BOOL_RBV"        /* asynUInt32Digital,    r/o */
#define P_BoolCallbackString            "TEST_CALLBACK_BOOL"            /* asynUInt32Digital,    r/w */
#define P_ShiftBoolCallbackRBVString    "TEST_CALLBACK_BOOL_SHIFT_RBV"  /* asynUInt32Digital,    r/o */
#define P_ShiftBoolCallbackString       "TEST_CALLBACK_BOOL_SHIFT"      /* asynUInt32Digital,    r/w */

#define P_MbbRBVString                  "TEST_MBB_RBV"                  /* asynUInt32Digital,    r/o */
#define P_MbbString                     "TEST_MBB"                      /* asynUInt32Digital,    r/w */

#define P_MbbCallbackRBVString          "TEST_CALLBACK_MBB_RBV"         /* asynUInt32Digital,    r/o */
#define P_MbbCallbackString             "TEST_CALLBACK_MBB"             /* asynUInt32Digital,    r/w */

#define P_MbbdirectRBVString            "TEST_MBBDIRECT_RBV"            /* asynUInt32Digital,    r/o */
#define P_MbbdirectString               "TEST_MBBDIRECT"                /* asynUInt32Digital,    r/w */

#define P_MbbdirectCallbackRBVString    "TEST_CALLBACK_MBBDIRECT_RBV"   /* asynUInt32Digital,    r/o */
#define P_MbbdirectCallbackString       "TEST_CALLBACK_MBBDIRECT"       /* asynUInt32Digital,    r/w */

/** Class that demonstrates the use of the asynMaskShift INP/OUT link for the asynUInt32Digital DTYP.
  * This only applies to the longin/longout records. For the binary and multi-bit records, the existing
  * asynMask syntax works well. But for longin/longout records, it could be nice to shift the bits in 
  * cases where a 32 bit register contains multiple numeric PVs that happen to be bitpacked in one register.
  * The binary and multi-bit PVs were included in this example to show the existing functionality and for 
  * backwards compatiblity testing */
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

    int P_Mbb;
    int P_MbbRBV;

    int P_MbbCallback;
    int P_MbbCallbackRBV;

    int P_Mbbdirect;
    int P_MbbdirectRBV;

    int P_MbbdirectCallback;
    int P_MbbdirectCallbackRBV;

private:
};
