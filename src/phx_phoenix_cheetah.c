#include <phx_api.h>

#include "phx_cheetah.h"
#include "phx_phoenix.h"
#include "phx_phoenix_cheetah.h"

/* function to handle parameters that require simultaneous configuration of both
  the bobcat and the phoenix PHX_CHEETAH_BIT_DEPTH = PHX_CHEETAH_8BIT |
  PHX_CHEETAH_10BIT | PHX_CHEETAH_12BIT PHX_CHEETAH_TAPS =
  PHX_CHEETAH_DOUBLE_TAP | PHX_CHEETAH_SINGLE_TAP PHX_CHEETAH_ROI ~
  0,0,400,400,CHEETAHPARAM_BINNING_1X,CHEETAHPARAM_BINNING_1X
*/
etStat Phx_Cheetah_Configure(tHandle hpb, PhxCheetahParam parameter,
                             void *value)
{
    etStat eStat = PHX_OK;
    etParamValue
        eParamValue; /* Parameter for use with PHX_ParameterSet/Get calls */
    CheetahParamValue
        bParamValue; /* Parameter for use with Cheetah_ParameterSet/Get calls */
    PhxCheetahParamValue *ppbParamValue;
    CheetahRoi *proi;

    switch (parameter)
    {
    case PHX_CHEETAH_BIT_DEPTH:
        eParamValue = PHX_CAM_SRC_MONO;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_SRC_COL,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;

        ppbParamValue = value;
        switch (*ppbParamValue)
        {
        case PHX_CHEETAH_DOUBLE_TAP:
            eStat = PHX_ERROR_BAD_PARAM_VALUE;
            goto Error;
            break;

        case PHX_CHEETAH_8BIT: /* 8bit format */
            bParamValue = CHEETAHPARAM_A2D_8B;
            eStat       = Cheetah_ParameterSet(hpb, CHEETAH_A2D_BITS,
                                               (CheetahParamValue *)&bParamValue);
            if (eStat != PHX_OK)
                goto Error;

            bParamValue = CHEETAHPARAM_LINK_8B;
            eStat       = Cheetah_ParameterSet(hpb, CHEETAH_LINK_BITS,
                                               (CheetahParamValue *)&bParamValue);
            eParamValue = 8;
            eStat       = PHX_ParameterSet(hpb, PHX_CAM_SRC_DEPTH,
                                           (etParamValue *)&(eParamValue));
            if (eStat != PHX_OK)
                goto Error;
            eParamValue = PHX_DST_FORMAT_Y8;
            eStat       = PHX_ParameterSet(hpb, PHX_CAPTURE_FORMAT,
                                           (etParamValue *)&(eParamValue));
            if (eStat != PHX_OK)
                goto Error;
            break;

        case PHX_CHEETAH_10BIT: /* 10bit format */
            bParamValue = CHEETAHPARAM_A2D_10B;
            eStat       = Cheetah_ParameterSet(hpb, CHEETAH_A2D_BITS,
                                               (CheetahParamValue *)&bParamValue);
            if (eStat != PHX_OK)
                goto Error;

            bParamValue = CHEETAHPARAM_LINK_10B;
            eStat       = Cheetah_ParameterSet(hpb, CHEETAH_LINK_BITS,
                                               (CheetahParamValue *)&bParamValue);
            if (eStat != PHX_OK)
                goto Error;
            eParamValue = 10;
            eStat       = PHX_ParameterSet(hpb, PHX_CAM_SRC_DEPTH,
                                           (etParamValue *)&(eParamValue));
            if (eStat != PHX_OK)
                goto Error;
            eParamValue = PHX_DST_FORMAT_Y10;
            eStat       = PHX_ParameterSet(hpb, PHX_CAPTURE_FORMAT,
                                           (etParamValue *)&(eParamValue));
            if (eStat != PHX_OK)
                goto Error;
            break;

        case PHX_CHEETAH_12BIT: /* 12bit format */
            bParamValue = CHEETAHPARAM_A2D_12B;
            eStat       = Cheetah_ParameterSet(hpb, CHEETAH_A2D_BITS,
                                               (CheetahParamValue *)&bParamValue);
            if (eStat != PHX_OK)
                goto Error;

            bParamValue = CHEETAHPARAM_LINK_12B;
            eStat       = Cheetah_ParameterSet(hpb, CHEETAH_LINK_BITS,
                                               (CheetahParamValue *)&bParamValue);

            eParamValue = 12;
            eStat       = PHX_ParameterSet(hpb, PHX_CAM_SRC_DEPTH,
                                           (etParamValue *)&(eParamValue));
            if (eStat != PHX_OK)
                goto Error;
            eParamValue = PHX_DST_FORMAT_Y12;
            eStat       = PHX_ParameterSet(hpb, PHX_CAPTURE_FORMAT,
                                           (etParamValue *)&(eParamValue));
            if (eStat != PHX_OK)
                goto Error;
            break;
        }
        break;

    case PHX_CHEETAH_TAPS:
        eParamValue = PHX_CAM_HTAP_LEFT;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_HTAP_DIR,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eParamValue = PHX_CAM_HTAP_LINEAR;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_HTAP_TYPE,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eParamValue = PHX_CAM_HTAP_ASCENDING;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_HTAP_ORDER,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eParamValue = 1;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_VTAP_NUM,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eParamValue = PHX_CAM_VTAP_TOP;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_VTAP_DIR,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eParamValue = PHX_CAM_VTAP_LINEAR;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_VTAP_TYPE,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eParamValue = PHX_CAM_VTAP_ASCENDING;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_VTAP_ORDER,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;

        ppbParamValue = value;
        switch (*ppbParamValue)
        {

        case PHX_CHEETAH_8BIT:
            eStat = PHX_ERROR_BAD_PARAM_VALUE;
            goto Error;
            break;

        case PHX_CHEETAH_10BIT:
            eStat = PHX_ERROR_BAD_PARAM_VALUE;
            goto Error;
            break;

        case PHX_CHEETAH_12BIT:
            eStat = PHX_ERROR_BAD_PARAM_VALUE;
            goto Error;
            break;

        case PHX_CHEETAH_DOUBLE_TAP:
            eParamValue = 2;
            eStat       = PHX_ParameterSet(hpb, PHX_CAM_HTAP_NUM,
                                           (etParamValue *)&(eParamValue));
            if (eStat != PHX_OK)
                goto Error;
            bParamValue = CHEETAHPARAM_TAPS_BASE2;
            eStat       = Cheetah_ParameterSet(hpb, CHEETAH_TAPS,
                                               (CheetahParamValue *)&bParamValue);
            if (eStat != PHX_OK)
                goto Error;
            break;
        }
        break;

    case PHX_CHEETAH_ROI:
        proi = value;

        eParamValue = 1;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_XBINNING,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterSet(hpb, PHX_CAM_YBINNING,
                                 (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterSet(hpb, PHX_CAM_ACTIVE_XLENGTH,
                                 (etParamValue *)&(proi->x_length));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterSet(hpb, PHX_CAM_ACTIVE_YLENGTH,
                                 (etParamValue *)&(proi->y_length));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterSet(hpb, PHX_ROI_XLENGTH,
                                 (etParamValue *)&(proi->x_length));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterSet(hpb, PHX_ROI_YLENGTH,
                                 (etParamValue *)&(proi->y_length));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterGet(hpb, PHX_CAM_HTAP_NUM,
                                 (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        // NOTE: Set only the x-offset on the frame grabber. must divide by the
        // number of taps.
        eParamValue = proi->x_offset / eParamValue;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_ACTIVE_XOFFSET,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eParamValue = 0;
        eStat       = PHX_ParameterSet(hpb, PHX_CAM_ACTIVE_YOFFSET,
                                       (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterSet(hpb, PHX_ROI_SRC_XOFFSET,
                                 (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        eStat = PHX_ParameterSet(hpb, PHX_ROI_SRC_YOFFSET,
                                 (etParamValue *)&(eParamValue));
        if (eStat != PHX_OK)
            goto Error;
        // eStat = Cheetah_ParameterSet(hpb, CHEETAH_XBINNING,
        //                              (CheetahParamValue
        //                              *)&(proi->x_binning));
        // if (eStat != PHX_OK)
        //     goto Error;
        // eStat = Cheetah_ParameterSet(hpb, CHEETAH_YBINNING,
        //                              (CheetahParamValue
        //                              *)&(proi->y_binning));
        // if (eStat != PHX_OK)
        //     goto Error;

        bParamValue = 0x1;
        eStat       = Cheetah_ParameterSet(hpb, CHEETAH_MAOI_STATE,
                                           (CheetahParamValue *)&bParamValue);
        if (eStat != PHX_OK)
            goto Error;
        eStat = Cheetah_ParameterSet(hpb, CHEETAH_MAOI_XWIDTH,
                                     (CheetahParamValue *)&(proi->x_length));
        if (eStat != PHX_OK)
            goto Error;
        eStat = Cheetah_ParameterSet(hpb, CHEETAH_MAOI_YWIDTH,
                                     (CheetahParamValue *)&(proi->y_length));
        if (eStat != PHX_OK)
            goto Error;
        eStat = Cheetah_ParameterSet(hpb, CHEETAH_MAOI_XOFST,
                                     (CheetahParamValue *)&(proi->x_offset));
        if (eStat != PHX_OK)
            goto Error;
        eStat = Cheetah_ParameterSet(hpb, CHEETAH_MAOI_YOFST,
                                     (CheetahParamValue *)&(proi->y_offset));
        if (eStat != PHX_OK)
            goto Error;
        break;
    }

Error:
    return eStat;
}

int Phx_Cheetah_str_to_PhxCheetahParam(char *str, PhxCheetahParam *ppbParam)
{
    if (strcmp(str, "PHX_CHEETAH_BIT_DEPTH") == 0)
    {
        *ppbParam = PHX_CHEETAH_BIT_DEPTH;
    }
    else if (strcmp(str, "PHX_CHEETAH_TAPS") == 0)
    {
        *ppbParam = PHX_CHEETAH_TAPS;
    }
    else if (strcmp(str, "PHX_CHEETAH_ROI") == 0)
    {
        *ppbParam = PHX_CHEETAH_ROI;
    }
    else
    {
        return 0;
    }
    return 1;
}

int Phx_Cheetah_str_to_PhxCheetahParamValue(char *str,
                                            PhxCheetahParamValue *ppbParamValue)
{
    if (strcmp(str, "PHX_CHEETAH_8BIT") == 0)
    {
        *ppbParamValue = PHX_CHEETAH_8BIT;
    }
    else if (strcmp(str, "PHX_CHEETAH_10BIT") == 0)
    {
        *ppbParamValue = PHX_CHEETAH_10BIT;
    }
    else if (strcmp(str, "PHX_CHEETAH_12BIT") == 0)
    {
        *ppbParamValue = PHX_CHEETAH_12BIT;
    }
    else if (strcmp(str, "PHX_CHEETAH_DOUBLE_TAP") == 0)
    {
        *ppbParamValue = PHX_CHEETAH_DOUBLE_TAP;
    }
    else
    {
        return 0;
    }
    return 1;
}
