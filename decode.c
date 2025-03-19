#include <stdio.h>
#include <string.h>
#include "types.h"
#include "decode.h"

Status do_decoding(char*stego, DecodeInfo*dInfo){
        // step1 open the stego file reach at 54
        FILE *stego_ptr = fopen(stego,"r");
        if (stego_ptr == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file\n");
            return e_failure;
        }
        fseek(stego_ptr, 54, SEEK_SET);

        if(stego_ptr == NULL){
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file\n" );
            return e_failure;
        }

        //get & check the magic string
        int ms_size = strlen(dInfo->user_magic_s);
        char d_magic_str[ms_size];

        char buffer[8];
        int i;
        for(i=0;i<ms_size;i++){
        fread(buffer,1,8,stego_ptr);
        d_magic_str[i] = Decoder(buffer,8);
        }
        d_magic_str[i] =0;
        
        //Authentication
        if(strcmp(dInfo->user_magic_s,d_magic_str)!=0){
            printf("ERROR: Authentication Failed!\n");
            return e_failure;
        }else {
            printf("SUCCESS: Authentication Successful!\n");
        }

        /*get extension, make file (sec.txt)*/
        //1st getting the size of secret file extension
        char Sbuffer[32];
        fread(Sbuffer, 1, 32, stego_ptr);
        dInfo->extSize = Decoder(Sbuffer, 32);

        //2nd getting extension
        char SecExt[dInfo->extSize];
        char Extbuffer[8];
        for (i = 0; i < dInfo->extSize; i++){
            fread(Extbuffer, 1, 8, stego_ptr);
            SecExt[i] = Decoder(Extbuffer, 8);
        }
        SecExt[i] = 0;
        dInfo->extn = SecExt;

        //concate ext to provided name of secret file
        strcpy(dInfo->secFileName, dInfo->user_secF_name);
        strcat(dInfo->secFileName, dInfo->extn);
        
        printf("SUCCESS: Secret file created as %s !\n",dInfo->secFileName);
        
        //step5 decode secrect data put in  (sec.txt)
        //1st get the size for file
        char Secbuffer[32];
        fread(Secbuffer, 1, 32, stego_ptr);
        dInfo->secSize = Decoder(Secbuffer, 32);

        //getting data
        dInfo->SecData = (char*)malloc(dInfo->secSize);

        char Databuffer[8];
        for (i = 0; i < dInfo->secSize; i++)
        {
            fread(Databuffer, 1, 8, stego_ptr);
            dInfo->SecData[i] = Decoder(Databuffer, 8);
        }
        //  dInfo->SecData[dInfo->secSize] = '\0';

         // 3rd opening secrect file and put data
         FILE*secPtr = fopen(dInfo->secFileName,"w");
         fwrite(dInfo->SecData, 1, dInfo->secSize,secPtr);
         printf("SUCCESS: Data transfered to %s!\n",dInfo->secFileName);

         free(dInfo->SecData);
         return e_success;
}

Status read_valid_arg_decode(char *argv[], DecodeInfo*dInfo){
    //flow should be (stego.bmp, "#*" , secret(optional))
    if (strstr(argv[2], ".bmp") == NULL){
        printf("ERROR: Encoded File not provided!\n"); 
        return e_failure;
    }else{
        if (argv[3] == NULL) {
            printf("ERROR: Please provide Magic String!\n");
            return e_failure;
        }
        else dInfo->user_magic_s = argv[3];
    }
    if(argv[4]!=NULL)
        dInfo->user_secF_name = argv[4];
    else
        dInfo->user_secF_name = "Secret";
    return e_success;
}

uint32_t Decoder(char buffer[], int size)
{
    int i=0;
    uint32_t ans = 0, mask = 0;
    while(i<size){
        ans = ans << 1;
        mask = (buffer[i]) & 1;
        ans = ans | mask;
        i++;
    }
    return ans;
}
