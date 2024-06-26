/*! \file bobcat.h
    \brief Bobcat camera controls.*/

#ifndef _BOBCAT
#define _BOBCAT

#include <phx_api.h> /* Main Phoenix library */

#ifdef CHEETAH_PARAM
#define BKUP_CHEETAH_PARAM CHEETAH_PARAM
#endif

#define CHEETAH_PARAM(name, addr) \
    CHEETAH_##name = (ui16)0x##addr,

typedef enum
{
    CHEETAH_INVALID_PARAM = (ui16)0,

    CHEETAH_INFO_TEST_REGISTER = (ui16)0x600C,
    CHEETAH_SOFT_RESET         = (ui16)0x601C,

    /* Saving and restoring registers */
    CHEETAH_BOOT_FROM        = (ui16)0x6060,
    CHEETAH_CFG_LOAD    = (ui16)0x6064,
    CHEETAH_CFG_SAVE    = (ui16)0x6068,
    CHEETAH_CFG_DEFAULT     = (ui16)0x5000,
    CHEETAH_BAUD_RATE_SELECT = (ui16)0x0604,

    /* Camera Manufacturere data */
    CHEETAH_MFG_FW_REV   = (ui16)0x6004,
    CHEETAH_MFG_FPGA_ID  = (ui16)0x6008,
    CHEETAH_MFG_FW_BUILD = (ui16)0x6038,
    CHEETAH_FAMILY_ID    = (ui16)0x603c,

    /* Camera information registers */
    CHEETAH_INFO_CCD_TEMP         = (ui16)0x6010,
    CHEETAH_INFO_MIN_MAX_XLENGTHS = (ui16)0x6070,
    CHEETAH_INFO_MIN_MAX_YLENGTHS = (ui16)0x6074,
    CHEETAH_INFO_XYLENGTHS        = (ui16)0x6078,
    CHEETAH_INFO_FRM_TIME         = (ui16)0x6080,
    CHEETAH_INFO_MIN_FRM_TIME     = (ui16)0x6084,
    CHEETAH_INFO_EXP_TIME         = (ui16)0x6088,
    CHEETAH_INFO_MAX_EXP_TIME     = (ui16)0x6090,

    /* Camera A2D Control */
    CHEETAH_PARAM(A2D_BITS, 8)
    CHEETAH_PARAM(LINK_BITS, 100)
    CHEETAH_PARAM(TAPS, 104)
    CHEETAH_PARAM(BITSHIFT_SEL, 158)

    /* Camera MAOI Control */
    CHEETAH_PARAM(TEST_PATTERN, 108)
    CHEETAH_PARAM(MAOI_STATE, 10)
    CHEETAH_PARAM(MAOI_XOFST, 14)
    CHEETAH_PARAM(MAOI_XWIDTH, 18)
    CHEETAH_PARAM(MAOI_YOFST, 1c)
    CHEETAH_PARAM(MAOI_YWIDTH, 20)
    CHEETAH_PARAM(MAOI_XFLIP, 30)
    CHEETAH_PARAM(MAOI_YFLIP, 34)

    /* Camera Exposure and FrameTime control */
    CHEETAH_PARAM(EXP_CTL_MOD, 40)
    CHEETAH_PARAM(EXP_TIME_ABS, 44)
    CHEETAH_PARAM(PRG_FRMTIME_EN, 48)
    CHEETAH_PARAM(PRG_FRMTIME, 4c)
    CHEETAH_PARAM(AEC_CTL_EN, 140)

    /* Camera Trigger Control */
    CHEETAH_PARAM(TRGMODE_EN, 500)
    CHEETAH_PARAM(TRGIN_SEL, 504)
    CHEETAH_PARAM(TRGEDGE_SEL, 508)
    CHEETAH_PARAM(SOFT_TRIGGER, 6030)

    /* Camera GPIO */
    CHEETAH_PARAM(OUT1_POL, 558)
    CHEETAH_PARAM(OUT1_SRC, 55c)
    CHEETAH_PARAM(OUT2_POL, 560)
    CHEETAH_PARAM(OUT2_SRC, 564)

} CheetahParam;

#undef CHEETAH_PARAM

#define CHEETAH_PARAM(name, value) \
    CHEETAHPARAM_##name = (ui32)0x##value,

#define CHEETAH_PARAM_MULTI(class, variant, value) \
    CHEETAHPARAM_##class##_##variant = (ui32)0x##value,

typedef enum {
    CHEETAH_PARAM(INVALID, 0)

    CHEETAH_PARAM(ENABLE, 1)
    CHEETAH_PARAM(DISABLE, 0)

    CHEETAH_PARAM(SOFT_RESET_CODE, deadbeef)

    CHEETAH_PARAM(BINNING_1X, 1)

    /* Boot Loader */
    CHEETAH_PARAM(CFG_FACTORY, 0)
    CHEETAH_PARAM(CFG_USER1, 1)
    CHEETAH_PARAM(CFG_USER2, 2)
    CHEETAH_PARAM(CFG_USER3, 3)
    CHEETAH_PARAM(CFG_USER4, 4)

    /* Baud Rate */
    CHEETAH_PARAM(B9600, 0)
    CHEETAH_PARAM(B19200, 1)
    CHEETAH_PARAM(B38400, 2)
    CHEETAH_PARAM(B57600, 3)
    CHEETAH_PARAM(B115200, 4)

    /* Camera A2D Control */
    CHEETAH_PARAM(A2D_8B, 0)
    CHEETAH_PARAM(A2D_10B, 1)
    CHEETAH_PARAM(A2D_12B, 2)

    CHEETAH_PARAM(LINK_8B, 0)
    CHEETAH_PARAM(LINK_10B, 1)
    CHEETAH_PARAM(LINK_12B, 2)

    CHEETAH_PARAM(TAPS_BASE2, 0)
    CHEETAH_PARAM(TAPS_BASE3, 1)
    CHEETAH_PARAM(TAPS_MEDIUM, 2)
    CHEETAH_PARAM(TAPS_FULL, 3)
    CHEETAH_PARAM(TAPS_DECA, 4)

    /* Camera Test Pattern */
    CHEETAH_PARAM(TEST_PATTERN_NONE, 0)
    CHEETAH_PARAM(TEST_PATTERN_BW_CHECKER, 1)
    CHEETAH_PARAM(TEST_PATTERN_GRAY, 2)
    CHEETAH_PARAM(TEST_PATTERN_TAP_SEG, 3)
    CHEETAH_PARAM(TEST_PATTERN_HOR_RAMP, 4)
    CHEETAH_PARAM(TEST_PATTERN_VER_RAMP, 5)
    CHEETAH_PARAM(TEST_PATTERN_BOTH_RAMP_STATIC, 6)
    CHEETAH_PARAM(TEST_PATTERN_BOTH_RAMP_DYN, 7)
    CHEETAH_PARAM(TEST_PATTERN_VERT_BAR, 8)
    CHEETAH_PARAM(TEST_PATTERN_CENTER_CROSS, 9)

    /* Bit Shift */
    CHEETAH_PARAM(BIT_SHIFT_NONE, 0)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 1L, 1)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 2L, 2)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 3L, 3)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 4L, 4)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 5L, 5)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 6L, 6)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 7L, 7)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 1R, 9)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 2R, a)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 3R, b)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 4R, c)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 5R, d)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 6R, e)
    CHEETAH_PARAM_MULTI(BIT_SHIFT, 7R, f)


    /* FrameTime */
    CHEETAH_PARAM(EXPCTL_OFF, 0)
    CHEETAH_PARAM(EXPCTL_PWM, 1)
    CHEETAH_PARAM(EXPCTL_AUTO, 3)

    /* Trigger and I/O */
    CHEETAH_PARAM(TRIG_NONE, 0)
    CHEETAH_PARAM(TRIG_EXT1, 1)
    CHEETAH_PARAM(TRIG_INT, 2)
    CHEETAH_PARAM(TRIG_COMPUTER, 3) // Computer; Camera expects trigger from CC1 via CL cable
    CHEETAH_PARAM(TRIG_SOFT, 4) // Software trigger, expects a one clock cycle pulse generated by software. Exposure is internal timer controlled. Pulse duration exposure not allowed.
    CHEETAH_PARAM(TRIG_EXT2, 5)

    CHEETAH_PARAM(EDGE_RISE, 0)
    CHEETAH_PARAM(EDGE_FALL, 1)

    CHEETAH_PARAM(OUT_POL_HIGH, 1)
    CHEETAH_PARAM(OUT_POL_LOW, 0)

    CHEETAH_PARAM(OUT_SRC_NONE, 0)
    CHEETAH_PARAM(OUT_SRC_EXP_START, 1)
    CHEETAH_PARAM(OUT_SRC_EXP_END, 2)
    CHEETAH_PARAM(OUT_SRC_EXP_MID, 3)
    CHEETAH_PARAM(OUT_SRC_EXP_WIN, 4)
    CHEETAH_PARAM(OUT_SRC_HSYNC, 5)
    CHEETAH_PARAM(OUT_SRC_VSYNC, 6)
    CHEETAH_PARAM(OUT_SRC_ODD_EVEN, 7)
    CHEETAH_PARAM(OUT_SRC_TRIG_PULSE_ACTUAL, 8)
    CHEETAH_PARAM(OUT_SRC_TRIG_PULSE_DELAYED, 9)
    CHEETAH_PARAM(OUT_SRC_CAMERA_READY, A)
    CHEETAH_PARAM(OUT_SRC_PULSE_GEN, B)
    CHEETAH_PARAM(OUT_SRC_STROBE1, C)
    CHEETAH_PARAM(OUT_SRC_STROBE2, D)
    CHEETAH_PARAM(OUT_SRC_TOGGLE, E)
    CHEETAH_PARAM(OUT_SRC_FRAME_PULSE, F)
} CheetahParamValue;

#undef CHEETAH_PARAM

#ifdef BKUP_CHEETAH_PARAM
#define CHEETAH_PARAM BKUP_CHEETAH_PARAM
#undef BKUP_CHEETAH_PARAM
#endif

typedef ui32 CheetahParamNumber;

enum bStat
{
    CHEETAH_OK = 0,
    CHEETAH_ERROR_BAD_HANDLE
};
typedef enum bStat CheetahStat;

/*!	\typedef
  \struct CheetahRoi
  \brief A region structure.
  \details Used to define the region on the CCD in the detector coordinate
  space.*/
typedef struct
{
    ui32 x_offset;               /**< x offset */
    ui32 y_offset;               /**< y offset */
    ui32 x_length;               /**< x length or width */
    ui32 y_length;               /**< y length or height */
    CheetahParamValue x_binning; /**< x binning, CHEETAH_BINNING_*X */
    CheetahParamValue y_binning; /**< y binning, CHEETAH_BINNING_*X */
} CheetahRoi;

/* Function prototypes */
etStat Cheetah_ControlRead(tHandle, ui8 *, ui8 *);
etStat Cheetah_ControlWrite(tHandle, ui8 *, ui8);
etStat Cheetah_ParameterGet(tHandle, CheetahParam, ui32 *);
etStat Cheetah_ParameterSet(tHandle, CheetahParam, ui32 *);
etStat Cheetah_SoftReset(tHandle);
etStat Cheetah_LoadFromFactory(tHandle);
etStat Cheetah_LoadFromUser1(tHandle);
etStat Cheetah_SaveToUser1(tHandle);
etStat Cheetah_LoadFromUser2(tHandle);
etStat Cheetah_SaveToUser2(tHandle);
etStat Cheetah_SoftwareTriggerStart(tHandle);
float Cheetah_GetTemp(tHandle hCamera);

int Cheetah_str_to_CheetahParam(char *, CheetahParam *);
int Cheetah_str_to_CheetahParamValue(char *, CheetahParamValue *);
int Cheetah_str_to_CheetahParamValues(char *, CheetahParamValue *);

#endif /* _BOBCAT */
