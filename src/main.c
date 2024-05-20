#define _XOPEN_SOURCE 500 // for sigset
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <phx_api.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <termios.h>
#include <unistd.h>

#include <math.h>

#include <libbmp/libbmp.h>

/* piccflight headers */
#include "phx_config.h"
#include "picc_dio.h"

/* SHK board number */
#define SHK_BOARD_NUMBER PHX_BOARD_NUMBER_1

#define ONE_MILLION      1000000ull

/* Global Variables */
tHandle cheetah_camera = 0; /* Camera Handle   */
typedef struct _CamreaContext
{
    uint16_t wid;
    uint16_t hei;
    uint16_t bitshift;
    char name[64];
} CameraContext;

/**************************************************************/
/* SHKCTRLC                                                   */
/*  - SIGINT function                                         */
/**************************************************************/
void shkctrlC(int sig)
{
    fflush(stdout);
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

// Unset DIO bit C1
#if PICC_DIO_ENABLE
    outb(0x00, PICC_DIO_BASE + PICC_DIO_PORTC);
#endif

#if MSG_CTRLC
    printf("SHK: exiting\n");
#endif

    exit(sig);
}

/**************************************************************/
/* SHK_CALLBACK                                               */
/*  - Exposure ISR callback function                          */
/**************************************************************/
static void image_cb(tHandle cam, ui32 dwInterruptMask, void *pvParams)
{
    static uint64_t frame_count = 0;
    static char first_frame     = 1;
    static char first_irq       = 1;

    if (first_irq)
    {
        first_irq = 0;
        printf("SHK: First IRQ\n");
    }

    if (dwInterruptMask & PHX_INTRPT_BUFFER_READY)
    {
        stImageBuff stBuffer;
        CameraContext *evtCtx = (CameraContext *)pvParams;
        frame_count++;
// Set DIO bit C1
#if PICC_DIO_ENABLE
        // outb(0x02, PICC_DIO_BASE + PICC_DIO_PORTC);
        if (frame_count % 2 == 0)
        {
            outb(0x02, PICC_DIO_BASE + PICC_DIO_PORTC);
        }
        else
        {
            outb(0x00, PICC_DIO_BASE + PICC_DIO_PORTC);
        }
#endif

        etStat eStat = PHX_StreamRead(cam, PHX_BUFFER_GET, &stBuffer);
        if (PHX_OK == eStat && first_frame)
        {
            // do stuff with image
            uint16_t wid      = evtCtx->wid;
            uint16_t hei      = evtCtx->hei;
            uint16_t bitshift = evtCtx->bitshift;
            printf("SHK: First frame: [%u x %u][%u]\n", wid, hei, bitshift);
            Bitmap *img = bm_create(wid, hei);
            if (img == NULL)
            {
                printf("SHK: Failed to create image\n");
            }
            else
            {
                printf("SHK: Created image\n");
                for (int i = 0; i < wid; i++)
                {
                    for (int j = 0; j < hei; j++)
                    {
                        uint16_t val;
                        if (bitshift >= 8)
                        {
                            val = ((uint8_t *)stBuffer.pvAddress)[i + j * wid];
                        }
                        else
                        {
                            val =
                                ((uint16_t *)stBuffer.pvAddress)[i + j * wid] >>
                                bitshift;
                        }
                        uint32_t argb = 0xff000000;
                        argb |= (0x000000ff & val);
                        argb |= (0x000000ff & val) << 8;
                        argb |= (0x000000ff & val) << 16;
                        bm_set(img, i, j, argb);
                    }
                }
                printf("SHK: Loaded image\n");
                char filename[1024];
                snprintf(filename, sizeof(filename),
                         "data/image_%s_%" PRIu64 ".bmp", evtCtx->name,
                         frame_count);
                int ret = bm_save(img, filename);
                printf("SHK: Saved image to %s [%d]\n", filename, ret);
            }
            first_frame = 0;
        }
        PHX_StreamRead(cam, PHX_BUFFER_RELEASE, NULL);
    }
}

/**************************************************************/
/* SHK_PROC                                                   */
/*  - Main SHK camera process                                 */
/**************************************************************/
int main(int argc, const char *argv[])
{
// Unset DIO bit C1
#if PICC_DIO_ENABLE
    printf("SHK: Setting up IO permissions...\t");
    fflush(stdout);
    if (ioperm(PICC_DIO_BASE, PICC_DIO_LENGTH, 1))
    {
        printf("\n\nFailed to set ioperm: %s\n", strerror(errno));
        exit(1);
    }
    printf("Done.\n");
    printf("SHK: Unsetting DIO...\t");
    fflush(stdout);
    outb(0x00, PICC_DIO_BASE + PICC_DIO_PORTC);
    printf("Done.\n");
#endif
    char *configFileName = "config/shk_1bin_2tap_8bit.cfg";
    if (argc != 2)
    {
        printf("SHK: Using default config file: %s\n", configFileName);
    }
    else
    {
        configFileName = (char *)argv[1];
        printf("SHK: Using config file: %s\n", configFileName);
    }
    etStat eStat = PHX_OK;
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
    eParamValue = PHX_BOARD_NUMBER_AUTO;
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
    CameraContext eventContext;
    time_t timer;
    char buffer[26];
    struct tm *tm_info;

    timer   = time(NULL);
    tm_info = localtime(&timer);
    strftime(eventContext.name, sizeof(eventContext.name), "%Y%m%d_%H%M%S",
             tm_info);

    eStat = PHX_ParameterSet(cheetah_camera, PHX_EVENT_CONTEXT,
                             (void *)&eventContext);
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
    eventContext.hei = (bParamValue & 0xFFFF0000) >> 16;
    eventContext.wid = (bParamValue & 0x0000FFFF);

    eStat =
        Cheetah_ParameterGet(cheetah_camera, CHEETAH_A2D_BITS, &bParamValue);
    switch (bParamValue)
    {
    case CHEETAHPARAM_A2D_8B:
        printf("SHK: Camera A2D bits            : 8\n");
        eventContext.bitshift = 8;
        break;
    case CHEETAHPARAM_A2D_10B:
        printf("SHK: Camera A2D bits            : 10\n");
        eventContext.bitshift = 6;
        break;
    case CHEETAHPARAM_A2D_12B:
        printf("SHK: Camera A2D bits            : 12\n");
        eventContext.bitshift = 4;
        break;
    default:
        printf("SHK: Camera A2D bits            : Unknown [%d]\n", bParamValue);
        shkctrlC(0);
        break;
    }

    eStat =
        Cheetah_ParameterGet(cheetah_camera, CHEETAH_MAOI_STATE, &bParamValue);
    printf("SHK: Camera MAOI state          : %d [%d]\n", bParamValue, eStat);
    if (bParamValue == 0)
    {
        printf("SHK: Setting MAOI state to 1\n");
        eStat = Cheetah_ParameterSet(cheetah_camera, CHEETAH_MAOI_STATE, 1);

        if (PHX_OK != eStat)
        {
            printf("SHK: Cheetah_ParameterSet --> CHEETAH_MAOI_STATE 1\n");
            fflush(stdout);
            // shkctrlC(0);
        }
        sleep(1);
        printf("SHK: Checking camera MAOI state...\n");
        eStat = Cheetah_ParameterGet(cheetah_camera, CHEETAH_MAOI_STATE,
                                     &bParamValue);
        printf("SHK: Camera MAOI state          : %d [%d]\n", bParamValue,
               eStat);
    }

    eStat =
        Cheetah_ParameterGet(cheetah_camera, CHEETAH_TRGMODE_EN, &bParamValue);
    printf("SHK: Camera trigger mode        : %d\n", bParamValue);

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
    printf("SHK: Min exp = %d | Max exp = %d\n", expmin, expmax);
    expcmd = lround(ONE_MILLION);
    expcmd = expcmd > expmax ? expmax - 10 * expmin : expcmd;
    expcmd = expcmd < expmin ? expmin : expcmd;
    // eStat = Cheetah_ParameterSet(cheetah_camera, CHEETAH_EXP_TIME_ABS,
    // &expcmd); if (PHX_OK != eStat)
    // {
    //     printf("SHK: Cheetah_ParameterSet --> CHEETAH_EXP_TIME %d\n",
    //     expcmd); shkctrlC(0);
    // }
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

    sleep(4); // run for 10 seconds

    printf("SHK: Exiting\n");
    /* Exit */
    shkctrlC(0);
    return 0;
}
