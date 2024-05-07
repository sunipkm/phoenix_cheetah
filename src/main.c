#define _XOPEN_SOURCE 500
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pbl_api.h>
#include <phx_api.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <termios.h>
#include <unistd.h>

/* piccflight headers */
#include "phx_config.h"
#include "picc_dio.h"

/* SHK board number */
#define SHK_BOARD_NUMBER PHX_BOARD_NUMBER_1

#define ONE_MILLION      1000000ull

/* Global Variables */
tHandle cheetah_camera = 0; /* Camera Handle   */
float CHEETAH_GetTemp(tHandle hCamera);

/**************************************************************/
/* SHKCTRLC                                                   */
/*  - SIGINT function                                         */
/**************************************************************/
void shkctrlC(int sig)
{
    if (cheetah_camera)
    {
        PHX_StreamRead(cheetah_camera, PHX_ABORT,
                       NULL); /* Now cease all captures */
    }

    if (cheetah_camera)
    {                                 /* Release the Phoenix board */
        PHX_Close(&cheetah_camera);   /* Close the Phoenix board */
        PHX_Destroy(&cheetah_camera); /* Destroy the Phoenix handle */
    }

#if MSG_CTRLC
    printf("SHK: exiting\n");
#endif

    exit(sig);
}

/**************************************************************/
/* SHK_CALLBACK                                               */
/*  - Exposure ISR callback function                          */
/**************************************************************/
static void image_cb(tHandle cheetah_camera, ui32 dwInterruptMask,
                     void *pvParams)
{
    if (dwInterruptMask & PHX_INTRPT_BUFFER_READY)
    {
        stImageBuff stBuffer;
// Set DIO bit C1
#if PICC_DIO_ENABLE
        outb(0x02, PICC_DIO_BASE + PICC_DIO_PORTC);
#endif

        etStat eStat =
            PHX_StreamRead(cheetah_camera, PHX_BUFFER_GET, &stBuffer);
        if (PHX_OK == eStat)
        {
            // do stuff with image
// Unset DIO bit C1
#if PICC_DIO_ENABLE
            outb(0x00, PICC_DIO_BASE + PICC_DIO_PORTC);
#endif
        }
        PHX_StreamRead(cheetah_camera, PHX_BUFFER_RELEASE, NULL);
    }
}

/**************************************************************/
/* SHK_PROC                                                   */
/*  - Main SHK camera process                                 */
/**************************************************************/
int main(int argc, const char *argv[])
{
    char *configFileName = "config/shk_1bin_2tap_12bit.cfg";
    etStat eStat         = PHX_OK;
    etParamValue eParamValue;
    CheetahParamValue bParamValue, expmin, expmax, expcmd, frmmin, frmcmd,
        lnmin;
    int nLastEventCount = 0;
    ui64 dwParamValue;
    etParamValue roiWidth, roiHeight, bufferWidth, bufferHeight;
    int camera_running = 0;

    /* Set soft interrupt handler */
    sigset(SIGINT, shkctrlC); /* usually ^C */

    /* Create a Phoenix handle */
    eStat = PHX_Create(&cheetah_camera, PHX_ErrHandlerDefault);
    if (PHX_OK != eStat)
    {
        printf("SHK: Error PHX_Create\n");
        shkctrlC(0);
    }

    /* Set the board number */
    eParamValue = SHK_BOARD_NUMBER;
    eStat = PHX_ParameterSet(cheetah_camera, PHX_BOARD_NUMBER, &eParamValue);
    if (PHX_OK != eStat)
    {
        printf("SHK: Error PHX_ParameterSet --> Board Number\n");
        shkctrlC(0);
    }

    /* Open the Phoenix board */
    eStat = PHX_Open(cheetah_camera);
    if (PHX_OK != eStat)
    {
        printf("SHK: Error PHX_Open\n");
        shkctrlC(0);
    }

    /* Run the config file */
    eStat = PhxConfig_RunFile(cheetah_camera, configFileName);
    if (PHX_OK != eStat)
    {
        printf("SHK: Error PhxConfig_RunFile\n");
        shkctrlC(0);
    }

    /* Setup our own event context */
    eStat = PHX_ParameterSet(cheetah_camera, PHX_EVENT_CONTEXT, NULL);
    if (PHX_OK != eStat)
    {
        printf("SHK: Error PHX_ParameterSet --> PHX_EVENT_CONTEXT\n");
        shkctrlC(0);
    }

    /* Get debugging info */
    eStat = PHX_ParameterGet(cheetah_camera, PHX_ROI_XLENGTH, &roiWidth);
    eStat = PHX_ParameterGet(cheetah_camera, PHX_ROI_YLENGTH, &roiHeight);
    printf("SHK: roi                     : [%d x %d]\n", roiWidth, roiHeight);
    eStat = PHX_ParameterGet(cheetah_camera, PHX_BUF_DST_XLENGTH, &bufferWidth);
    eStat =
        PHX_ParameterGet(cheetah_camera, PHX_BUF_DST_YLENGTH, &bufferHeight);
    printf("SHK: destination buffer size : [%d x %d]\n", bufferWidth,
           bufferHeight);
    eStat = Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_MIN_MAX_XLENGTHS,
                                 &bParamValue);
    printf("SHK: Camera x size (width)      : [%d to %d]\n",
           (bParamValue & 0x0000FFFF), (bParamValue & 0xFFFF0000) >> 16);
    eStat = Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_MIN_MAX_YLENGTHS,
                                 &bParamValue);
    printf("SHK: Camera y size (height)     : [%d to %d]\n",
           (bParamValue & 0x0000FFFF), (bParamValue & 0xFFFF0000) >> 16);
    eStat = Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_XYLENGTHS,
                                 &bParamValue);
    printf("SHK: Camera current size        : [%d x %d]\n",
           (bParamValue & 0x0000FFFF), (bParamValue & 0xFFFF0000) >> 16);

    /* STOP Capture to put camera in known state */
    eStat = PHX_StreamRead(cheetah_camera, PHX_STOP, (void *)image_cb);
    if (PHX_OK != eStat)
    {
        printf("SHK: PHX_StreamRead --> PHX_STOP\n");
        shkctrlC(0);
    }
    camera_running = 0;
    printf("SHK: Camera stopped\n");

    /* Setup exposure */
    usleep(500000);
    // Get minimum frame time and check against command
    eStat = Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_MIN_FRM_TIME,
                                 &frmmin);
    frmmin &= 0x00FFFFFF; // 16 us
    printf("SHK: Min ln = %d | frm = %d\n", lnmin, frmmin);
    frmcmd = lround(0.2e-3 * ONE_MILLION); // 200 us
    frmcmd = frmcmd < frmmin ? frmmin : frmcmd;
    // Set the frame time
    eStat = Cheetah_ParameterSet(cheetah_camera, CHEETAH_PRG_FRMTIME, &frmcmd);
    if (PHX_OK != eStat)
    {
        printf("SHK: Cheetah_ParameterSet --> CHEETAH_FRM_TIME %d\n", frmcmd);
        shkctrlC(0);
    }
    // Get minimum and maximum exposure time and check against command
    usleep(500000);
    eStat =
        Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_EXP_TIME, &expmin);
    eStat = Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_MAX_EXP_TIME,
                                 &expmax);
    expmin &= 0xFF00000;
    expmin = ((ui32)expmin) >> 24;
    expmax &= 0x00FFFFFF;
    expcmd = lround(ONE_MILLION);
    expcmd = expcmd > expmax ? expmax : expcmd;
    expcmd = expcmd < expmin ? expmin : expcmd;
    eStat = Cheetah_ParameterSet(cheetah_camera, CHEETAH_EXP_TIME_ABS, &expcmd);
    if (PHX_OK != eStat)
    {
        printf("SHK: Cheetah_ParameterSet --> CHEETAH_EXP_TIME %d\n", expcmd);
        shkctrlC(0);
    }
    // Get set exposure and frame times
    eStat =
        Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_EXP_TIME, &expcmd);
    eStat =
        Cheetah_ParameterGet(cheetah_camera, CHEETAH_INFO_FRM_TIME, &frmcmd);
    expcmd &= 0x00FFFFFF;
    frmcmd &= 0x00FFFFFF;
    printf("SHK: Set frm = %d | exp = %d\n", frmcmd, expcmd);

    // Get CCD temperature
    // We only do this once now because it fails often
    printf("SHK: Get temp = %.2f\n", Cheetah_GetTemp(cheetah_camera));

    /* ----------------------- Enter Exposure Loop ----------------------- */

    /* Check if camera should start */
    if (!camera_running)
    {
        eStat = PHX_StreamRead(cheetah_camera, PHX_START, (void *)image_cb);
        if (PHX_OK != eStat)
        {
            printf("SHK: PHX_StreamRead --> PHX_START\n");
            shkctrlC(0);
        }
        camera_running = 1;
        printf("SHK: Camera started\n");
    }

    sleep(10); // run for 10 seconds

    /* Exit */
    shkctrlC(0);
    return 0;
}
