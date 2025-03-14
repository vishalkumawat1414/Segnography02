#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argb,char*argv[])
{
    EncodeInfo eInfo;
    DecodeInfo dInfo;

    OperationType ret = check_operation_type(argv);
    // printf("er %d",ret);
    Status arg_ret = read_valid_arg(argv, &eInfo);
    Status decode_ret = read_valid_arg_decode(argv,&dInfo);
    if(ret == e_encode){
        //encoding
        if(arg_ret==e_success){
            printf("SUCCESS: Encoding started...\n");
            Status res =  do_encoding(&eInfo);
            if(res==e_success) printf("SUCCESS: Encoding Done!\n");
            else printf("ERROR: Encoding Unsuccessfull!\n");
        }else {
            printf("ERROR: Encode validation failed!\n");
        }
        
    }else if(ret==e_decode){
        if(decode_ret==e_success){
            printf("Decoding started...\n");
            Status res= do_decoding(argv[2],&dInfo);
            if(res == e_success) printf("Decoding Done!!\n");
            else printf("ERROR: Decoding failed!\n");
        }else {
            printf("ERROR: Decode validation failed!\n");
        }
        
    }else {
        printf("ERROR: No valid cmd passed!\n");
    }

        return 0;
}

OperationType check_operation_type(char *argv[]){
    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }else if(strcmp(argv[1],"-d")==0) return e_decode;
    else return e_unsupported;
}
