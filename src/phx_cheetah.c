#include "phx_cheetah.h"
#include <phx_api.h> /* Main Phoenix library */

// #define _VERBOSE
#define MAX_BUFFER_LENGTH 256

etStat Cheetah_ControlRead(tHandle hCamera, ui8 *msgBuffer, ui8 *msgLength)
{
    etStat eStat = PHX_OK;
    etParamValue eParamValue;

    eStat = PHX_ParameterGet(
        hCamera, PHX_COMMS_INCOMING,
        (void *)
            msgLength); /* Check how many characters are waiting to be read */
    if (PHX_OK != eStat)
        return eStat;

    if (msgLength != 0)
    {
        eParamValue = *msgLength;
        eStat       = PHX_ControlRead(hCamera, PHX_COMMS_PORT, NULL, msgBuffer,
                                      (ui32 *)&eParamValue, 500);
        if (eStat != PHX_OK)
            return eStat;
        if (eParamValue == *msgLength)
        {
#if defined _VERBOSE
            int x;
            printf("rx : ");
            for (x = 0; x < *msgLength; x++)
                printf("[%02X]", msgBuffer[x]);
            printf("\n");
#endif
            return PHX_OK;
        }
        else
        {
            return PHX_ERROR_NOT_IMPLEMENTED; /*Failed to read all characters on
                                                 comm port read buffer*/
        }
    }
    else
    {
        return PHX_ERROR_NOT_IMPLEMENTED; /*camera is not returning any
                                             characters*/
    }
}

etStat Cheetah_ControlWrite(tHandle hCamera, ui8 *msgBuffer, ui8 msgLength)
{
    etStat eStat             = PHX_OK;
    etParamValue eParamValue = msgLength;

    eStat = PHX_ControlWrite(hCamera, PHX_COMMS_PORT, NULL, msgBuffer,
                             (ui32 *)&eParamValue, 500);
    if (eStat != PHX_OK)
        return eStat;

    if (eParamValue == msgLength)
    {
#if defined _VERBOSE
        int x;
        printf("tx : ");
        for (x = 0; x < msgLength; x++)
            printf("[%02X]", msgBuffer[x]);
        printf("\n");
#endif
        return PHX_OK;
    }
    else
    {
        return PHX_ERROR_NOT_IMPLEMENTED; /*Failed to write all characters to
                                             comm port*/
    }
}

#define READ_CMD  0x52
#define WRITE_CMD 0x57

etStat Cheetah_ParameterGet(tHandle hCamera, CheetahParam parameter,
                            ui32 *value)
{
    etStat eStat = PHX_OK;
    ui8 rxMsgBuffer[MAX_BUFFER_LENGTH];
    ui8 rxMsgLength;
    ui8 txMsgBuffer[3];
    ui8 txMsgLength = 3;

    // Setup command
    txMsgBuffer[0] = READ_CMD;
    txMsgBuffer[1] = ((ui8 *)&parameter)[1];
    txMsgBuffer[2] = ((ui8 *)&parameter)[0];

    eStat = Cheetah_ControlWrite(hCamera, txMsgBuffer, txMsgLength);
    if (PHX_OK != eStat)
        goto Return;

    _PHX_SleepMs(100);

    eStat = Cheetah_ControlRead(hCamera, rxMsgBuffer, &rxMsgLength);
    if (PHX_OK != eStat)
        goto Return;

    if (rxMsgBuffer[0] == 0x06)
    {
        *(ui32 *)value = (rxMsgBuffer[1] << 24) + (rxMsgBuffer[2] << 16) +
                         (rxMsgBuffer[3] << 8) +
                         (rxMsgBuffer[4]); /* all good */
    }
    else if (rxMsgBuffer[0] == 0x15)
    { /* camera returns an error code */
        int x;
        printf(
            "Camera error code returned for command : Cheetah_ParameterGet\n");
        for (x = 0; x < txMsgLength; x++)
            printf("[%02X]", txMsgBuffer[x]);
        printf("\n");
        printf("address  0x%02X%02X\n", txMsgBuffer[1], txMsgBuffer[2]);
        printf("data     0x%02X%02X%02X%02X (%d)\n", rxMsgBuffer[3],
               rxMsgBuffer[4], rxMsgBuffer[5], rxMsgBuffer[6], *(ui32 *)value);
        switch (rxMsgBuffer[1])
        {
        case 0:
            printf("No error\n");
            break;
        case 1:
            printf("Invalid command\n");
            break;
        case 2:
            printf("Time-out\n");
            break;
        case 3:
            printf("Checksum error\n");
            break;
        case 4:
            printf("Value less then minimum\n");
            break;
        case 5:
            printf("Value higher than maximum\n");
            break;
        case 6:
            printf("AGC error\n");
            break;
        case 7:
            printf("Supervisor mode error\n");
            break;
        case 8:
            printf("Mode not supported error\n");
            break;
        }
        eStat = PHX_ERROR_NOT_IMPLEMENTED;
        goto Return;
    }
    else
    {
        eStat = PHX_ERROR_NOT_IMPLEMENTED;
        goto Return;
    }

Return:
    return eStat;
}

etStat Cheetah_ParameterSet(tHandle hCamera, CheetahParam parameter,
                            ui32 *value)
{
    ui8 rxMsgBuffer[MAX_BUFFER_LENGTH];
    ui8 rxMsgLength;
    ui8 txMsgBuffer[7];
    ui8 txMsgLength = 7;
    etStat eStat    = PHX_OK;

    // Setup command
    txMsgBuffer[0] = WRITE_CMD;
    txMsgBuffer[1] = ((ui8 *)&parameter)[1];
    txMsgBuffer[2] = ((ui8 *)&parameter)[0];
    txMsgBuffer[3] = ((ui8 *)value)[3];
    txMsgBuffer[4] = ((ui8 *)value)[2];
    txMsgBuffer[5] = ((ui8 *)value)[1];
    txMsgBuffer[6] = ((ui8 *)value)[0];

    eStat = Cheetah_ControlWrite(hCamera, txMsgBuffer, txMsgLength);
    if (PHX_OK != eStat)
        goto Return;

    _PHX_SleepMs(100);

    eStat = Cheetah_ControlRead(hCamera, rxMsgBuffer, &rxMsgLength);
    if (PHX_OK != eStat)
        goto Return;

    if (rxMsgBuffer[0] == 0x6)
    { /* all good */
    }
    else if (rxMsgBuffer[0] == 0x15)
    { /* camera returns an error code */
        int x;
        printf(
            "Camera error code returned for command : Cheetah_ParameterSet \n");
        for (x = 0; x < txMsgLength; x++)
            printf("[%02X]", txMsgBuffer[x]);
        printf("\n");
        printf("address  0x%02X%02X\n", txMsgBuffer[1], txMsgBuffer[2]);
        printf("data     0x%02X%02X%02X%02X (%d)\n", txMsgBuffer[3],
               txMsgBuffer[4], txMsgBuffer[5], txMsgBuffer[6], *(ui32 *)value);
        switch (rxMsgBuffer[1])
        {
        case 0:
            printf("No error\n");
            break;
        case 1:
            printf("Invalid command\n");
            break;
        case 2:
            printf("Time-out\n");
            break;
        case 3:
            printf("Checksum error\n");
            break;
        case 4:
            printf("Value less then minimum\n");
            break;
        case 5:
            printf("Value higher than maximum\n");
            break;
        case 6:
            printf("AGC error\n");
            break;
        case 7:
            printf("Supervisor mode error\n");
            break;
        case 8:
            printf("Mode not supported error\n");
            break;
        }
        eStat = PHX_ERROR_NOT_IMPLEMENTED;
        goto Return;
    }
    else
    { /* camera returns an error coder */
        eStat = PHX_ERROR_NOT_IMPLEMENTED;
        goto Return;
    }

Return:
    return eStat;
}

etStat Cheetah_SoftReset(tHandle hCamera)
{
    CheetahParamValue value = CHEETAHPARAM_SOFT_RESET_CODE;
    CheetahParam parameter  = CHEETAH_SOFT_RESET;
    return Cheetah_ParameterSet(hCamera, parameter, &value);
}

etStat Cheetah_LoadFromFactory(tHandle hCamera)
{
    CheetahParamValue value = CHEETAHPARAM_CFG_FACTORY;
    CheetahParam parameter  = CHEETAH_CFG_LOAD;
    return Cheetah_ParameterSet(hCamera, parameter, &value);
}

etStat Cheetah_LoadFromUser1(tHandle hCamera)
{
    CheetahParamValue value = CHEETAHPARAM_CFG_USER1;
    CheetahParam parameter  = CHEETAH_CFG_LOAD;
    return Cheetah_ParameterSet(hCamera, parameter, &value);
}

etStat Cheetah_LoadFromUser2(tHandle hCamera)
{
    CheetahParamValue value = CHEETAHPARAM_CFG_USER1;
    CheetahParam parameter  = CHEETAH_CFG_LOAD;
    return Cheetah_ParameterSet(hCamera, parameter, &value);
}

etStat Cheetah_SaveToUser1(tHandle hCamera)
{
    CheetahParamValue value = CHEETAHPARAM_CFG_USER1;
    CheetahParam parameter  = CHEETAH_CFG_SAVE;
    return Cheetah_ParameterSet(hCamera, parameter, &value);
}

etStat Cheetah_SaveToUser2(tHandle hCamera)
{
    CheetahParamValue value = CHEETAHPARAM_CFG_USER2;
    CheetahParam parameter  = CHEETAH_CFG_SAVE;
    return Cheetah_ParameterSet(hCamera, parameter, &value);
}

etStat CHEETAH_SoftwareTriggerStart(tHandle hCamera)
{
    CheetahParamValue value = 0x1;
    CheetahParam parameter  = CHEETAH_SOFT_TRIGGER;
    return Cheetah_ParameterSet(hCamera, parameter, &value);
}

#define CHEETAH_TEST(name)                                                     \
    else if (strcmp(str, "CHEETAH_" #name) == 0)                               \
    {                                                                          \
        *pbParam = CHEETAH_##name;                                             \
    }

#define CHEETAH_TEST_INFO(name)                                                \
    else if (strcmp(str, "CHEETAH_INFO_" #name) == 0)                          \
    {                                                                          \
        *pbParam = CHEETAH_INFO_##name;                                        \
    }

#define CHEETAH_TEST_MAOI(name)                                                \
    else if (strcmp(str, "CHEETAH_MAOI_" #name) == 0)                          \
    {                                                                          \
        *pbParam = CHEETAH_MAOI_##name;                                        \
    }

int Cheetah_str_to_CheetahParam(char *str, CheetahParam *pbParam)
{
    if (strcmp(str, "CHEETAH_INVALID_PARAM") == 0)
    {
        *pbParam = CHEETAH_INVALID_PARAM;
    }
    /* General things */
    CHEETAH_TEST(INFO_TEST_REGISTER)
    CHEETAH_TEST(SOFT_RESET)
    CHEETAH_TEST(BOOT_FROM)
    CHEETAH_TEST(CFG_LOAD)
    CHEETAH_TEST(CFG_SAVE)
    CHEETAH_TEST(BAUD_RATE_SELECT)
    CHEETAH_TEST(MFG_FW_REV)
    CHEETAH_TEST(MFG_FPGA_ID)
    CHEETAH_TEST(MFG_FW_BUILD)
    CHEETAH_TEST(FAMILY_ID)
    /* Info */
    CHEETAH_TEST_INFO(CCD_TEMP)
    CHEETAH_TEST_INFO(MIN_MAX_XLENGTHS)
    CHEETAH_TEST_INFO(MIN_MAX_YLENGTHS)
    CHEETAH_TEST_INFO(XYLENGTHS)
    CHEETAH_TEST_INFO(FRM_TIME)
    CHEETAH_TEST_INFO(MIN_FRM_TIME)
    CHEETAH_TEST_INFO(EXP_TIME)
    CHEETAH_TEST_INFO(MAX_EXP_TIME)
    /* A2D and Taps */
    CHEETAH_TEST(A2D_BITS)
    CHEETAH_TEST(LINK_BITS)
    CHEETAH_TEST(TAPS)
    CHEETAH_TEST(BITSHIFT_SEL)
    /* MAOI */
    CHEETAH_TEST(TEST_PATTERN)
    CHEETAH_TEST_MAOI(STATE)
    CHEETAH_TEST_MAOI(XOFST)
    CHEETAH_TEST_MAOI(XWIDTH)
    CHEETAH_TEST_MAOI(YOFST)
    CHEETAH_TEST_MAOI(YWIDTH)
    CHEETAH_TEST_MAOI(XFLIP)
    CHEETAH_TEST_MAOI(YFLIP)
    /* Exposure and Frame Time */
    CHEETAH_TEST(EXP_CTL_MOD)
    CHEETAH_TEST(EXP_TIME_ABS)
    CHEETAH_TEST(PRG_FRMTIME_EN)
    CHEETAH_TEST(PRG_FRMTIME)
    CHEETAH_TEST(AEC_CTL_EN)
    /* Camera Trigger Control */
    CHEETAH_TEST(TRGMODE_EN)
    CHEETAH_TEST(TRGIN_SEL)
    CHEETAH_TEST(TRGEDGE_SEL)
    CHEETAH_TEST(SOFT_TRIGGER)
    /* Camera GPIO */
    CHEETAH_TEST(OUT1_POL)
    CHEETAH_TEST(OUT1_SRC)
    CHEETAH_TEST(OUT2_POL)
    CHEETAH_TEST(OUT2_SRC)
    else
    {
        return 0;
    }
    return 1;
}

#undef CHEETAH_TEST

#define CHEETAH_TEST(name)                                                     \
    else if (strcmp(str, "CHEETAHPARAM_" #name) == 0)                          \
    {                                                                          \
        *pbParamValue = CHEETAHPARAM_##name;                                   \
    }

#define CHEETAH_TEST_MULTI(class, variant) \
    else if (strcmp(str, "CHEETAHPARAM_" #class "_" #variant) == 0) \
    {\
        *pbParamValue = CHEETAHPARAM_##class##_##variant; \
    }

int Cheetah_str_to_CheetahParamValue(char *str, CheetahParamValue *pbParamValue)
{
    if (0) // never true
    {
    }
    /* Generics */
    CHEETAH_TEST(INVALID)
    CHEETAH_TEST(ENABLE)
    CHEETAH_TEST(DISABLE)
    CHEETAH_TEST(SOFT_RESET_CODE)
    /* Config */
    CHEETAH_TEST(CFG_FACTORY)
    CHEETAH_TEST(CFG_USER1)
    CHEETAH_TEST(CFG_USER2)
    CHEETAH_TEST(CFG_USER3)
    CHEETAH_TEST(CFG_USER4)
    /* Baud Rates */
    CHEETAH_TEST(B9600)
    CHEETAH_TEST(B19200)
    CHEETAH_TEST(B38400)
    CHEETAH_TEST(B57600)
    CHEETAH_TEST(B115200)
    /* A2D Control */
    CHEETAH_TEST_MULTI(A2D, 8B)
    CHEETAH_TEST_MULTI(A2D, 10B)
    CHEETAH_TEST_MULTI(A2D, 12B)

    CHEETAH_TEST_MULTI(LINK, 12B)
    CHEETAH_TEST_MULTI(LINK, 12B)
    CHEETAH_TEST_MULTI(LINK, 12B)

    CHEETAH_TEST_MULTI(TAPS, BASE2)
    CHEETAH_TEST_MULTI(TAPS, BASE3)
    CHEETAH_TEST_MULTI(TAPS, MEDIUM)
    CHEETAH_TEST_MULTI(TAPS, FULL)
    CHEETAH_TEST_MULTI(TAPS, DECA)
    /* Test Pattern */
    CHEETAH_TEST_MULTI(TEST_PATTERN, NONE)
    CHEETAH_TEST_MULTI(TEST_PATTERN, BW_CHECKER)
    CHEETAH_TEST_MULTI(TEST_PATTERN, GRAY)
    CHEETAH_TEST_MULTI(TEST_PATTERN, TAP_SEG)
    CHEETAH_TEST_MULTI(TEST_PATTERN, HOR_RAMP)
    CHEETAH_TEST_MULTI(TEST_PATTERN, VER_RAMP)
    CHEETAH_TEST_MULTI(TEST_PATTERN, BOTH_RAMP_STATIC)
    CHEETAH_TEST_MULTI(TEST_PATTERN, BOTH_RAMP_DYN)
    CHEETAH_TEST_MULTI(TEST_PATTERN, VERT_BAR)
    CHEETAH_TEST_MULTI(TEST_PATTERN, CENTER_CROSS)
    /* Bit Shift */
    CHEETAH_TEST_MULTI(BIT_SHIFT, NONE)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 1L)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 2L)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 3L)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 4L)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 5L)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 6L)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 7L)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 1R)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 2R)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 3R)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 4R)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 5R)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 6R)
    CHEETAH_TEST_MULTI(BIT_SHIFT, 7R)
    /* Frame Time */
    CHEETAH_TEST_MULTI(EXPCTL, OFF)
    CHEETAH_TEST_MULTI(EXPCTL, PWM)
    CHEETAH_TEST_MULTI(EXPCTL, AUTO)
    /* Trigger and I/O */
    CHEETAH_TEST_MULTI(TRIG, NONE)
    CHEETAH_TEST_MULTI(TRIG, EXT1)
    CHEETAH_TEST_MULTI(TRIG, INT)
    CHEETAH_TEST_MULTI(TRIG, COMPUTER)
    CHEETAH_TEST_MULTI(TRIG, SOFT)
    CHEETAH_TEST_MULTI(TRIG, EXT2)

    CHEETAH_TEST_MULTI(EDGE, RISE)
    CHEETAH_TEST_MULTI(EDGE, FALL)

    CHEETAH_TEST_MULTI(OUT_POL, HIGH)
    CHEETAH_TEST_MULTI(OUT_POL, LOW)

    CHEETAH_TEST_MULTI(OUT_SRC, NONE)
    CHEETAH_TEST_MULTI(OUT_SRC_EXP, START)
    CHEETAH_TEST_MULTI(OUT_SRC_EXP, END)
    CHEETAH_TEST_MULTI(OUT_SRC_EXP, MID)
    CHEETAH_TEST_MULTI(OUT_SRC_EXP, WIN)
    CHEETAH_TEST_MULTI(OUT_SRC, HSYNC)
    CHEETAH_TEST_MULTI(OUT_SRC, VSYNC)
    CHEETAH_TEST_MULTI(OUT_SRC, ODD_EVEN)
    CHEETAH_TEST_MULTI(OUT_SRC_TRIG_PULSE, ACTUAL)
    CHEETAH_TEST_MULTI(OUT_SRC_TRIG_PULSE, DELAYED)
    CHEETAH_TEST_MULTI(OUT_SRC, CAMERA_READY)
    CHEETAH_TEST_MULTI(OUT_SRC, PULSE_GEN)
    CHEETAH_TEST_MULTI(OUT_SRC, STROBE1)
    CHEETAH_TEST_MULTI(OUT_SRC, STROBE2)
    CHEETAH_TEST_MULTI(OUT_SRC, TOGGLE)
    CHEETAH_TEST_MULTI(OUT_SRC, FRAME_PULSE)
    else
    {
        *pbParamValue = atol(str);
        return 0;
    }
    return 1;
}

int Cheetah_str_to_CheetahParamValues(char *str,
                                      CheetahParamValue *pbParamValue)
{
    char *token;
    CheetahParamValue temp_bParamValue;
    char char_pipe[2] = "|";
    *pbParamValue     = 0;
#if defined _VERBOSE
    printf("[");
#endif
    token = strtok(str, char_pipe);
    while (token != NULL)
    { /* cycle through other tokens */
#if defined _VERBOSE
        printf("%s:", token);
#endif
        if (Cheetah_str_to_CheetahParamValue(token, &temp_bParamValue) < 0)
        {
            return 0;
        }
        else
        {
            *pbParamValue |= temp_bParamValue;
#if defined _VERBOSE
            printf("%d:%d ,", temp_bParamValue, *pbParamValue);
#endif
        }
        token = strtok(NULL, char_pipe);
    }
#if defined _VERBOSE
    printf("\b]\n");
#endif
    return 1;
}

float Cheetah_GetTemp(tHandle hCamera)
{
    CheetahParamValue bParamValue;
    int temp_value, temp_sign;
    float temp;
    etStat eStat = PHX_OK;
    eStat = Cheetah_ParameterGet(hCamera, CHEETAH_INFO_CCD_TEMP, &bParamValue);
    if (PHX_OK != eStat)
    {
        printf("PHX: Error Cheetah_GetTemp\n");
    }
    else
    {
        return 246.312 - 0.304*(bParamValue & 0x1ff);
    }
    return 0;
}
