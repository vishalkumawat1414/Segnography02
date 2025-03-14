#include "types.h"


typedef struct _DecodeInfo
{
    /* Source Image info */
    int extSize;
    char *extn;
    int secSize;
    char *SecData;
    char *user_magic_s; //magic string given by user
    char *user_secF_name;  //secret file name given by  user
    char secFileName[50];

} DecodeInfo;

Status read_valid_arg_decode(char *argv[], DecodeInfo *dInfo);
char Decoder(char buffer[],int size);
Status do_decoding(char*stego,DecodeInfo *dInfo);