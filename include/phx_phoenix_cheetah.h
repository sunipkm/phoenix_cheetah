#ifndef _ADDONS
#define _ADDONS

#include <phx_api.h> /* Main Phoenix library */

typedef enum
{
    PHX_CHEETAH_BIT_DEPTH,
    PHX_CHEETAH_TAPS,
    PHX_CHEETAH_ROI
} PhxCheetahParam;

typedef enum
{
    /*PHX_CHEETAH_BIT_DEPTH_SELECT*/
    PHX_CHEETAH_8BIT,
    PHX_CHEETAH_10BIT,
    PHX_CHEETAH_12BIT,

    /* PHX_CHEETAH_TAPS */
    PHX_CHEETAH_DOUBLE_TAP,

} PhxCheetahParamValue;

etStat Phx_Cheetah_Configure(tHandle, PhxCheetahParam, void *);

int Phx_Cheetah_str_to_PhxCheetahParam(char *, PhxCheetahParam *);
int Phx_Cheetah_str_to_PhxCheetahParamValue(char *, PhxCheetahParamValue *);

#endif /* _ADDONS */
