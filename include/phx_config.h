#ifndef _CONFIG
#define _CONFIG

#include "phx_cheetah.h"
#include <phx_api.h>

#define PHX_MAX_FILE_LENGTH 128
#define PHX_CONFIG_MAX_LINE 255
typedef struct
{
    ui32 dwBoardNumber;
    etParamValue eBoardNumber;
    etCamConfigLoad eCamConfigLoad;
    char bConfigFileName[PHX_MAX_FILE_LENGTH];
    char bOutputFileName[PHX_MAX_FILE_LENGTH];
    char *pszConfigFileName;
    char *pszOutputFileName;
    ui32 dwBayerOption;
    ui32 dwGridSize;
    ui32 dwGammaOption;
    ui32 dwThresholdOption;
    ui32 dwTrackOption;
    ui32 dwFrameOption;
    ui32 dwTimeOption;
    ui32 dwSlowOption;
    ui32 dwServerPort;
} PhxSettings;

#define DEFAULT_CFG_FILENAME                                                   \
    "Imperx_Cheetah_0620CL-8bit-1tap.pcf" /* Application constants */

etStat PhxConfig_ParseCmdLine(int, char *[], PhxSettings *);

int PhxConfig_str_to_region(char *, CheetahRoi *);
etStat PhxConfig_RunFile(tHandle, char *);

#endif /* _CONFIG */
