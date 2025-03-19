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
    if(ret == e_encode){
        Status arg_ret = read_valid_arg(argv, &eInfo);

        if(arg_ret==e_success){
            printf("\n\n<--------------Encoding started--------------->\n\n");
            Status res =  do_encoding(&eInfo);
            if(res==e_success)
                printf("\n<---------------Encoding Done----------------->\n\n");
            else {printf("ERROR: Encoding Unsuccessfull!\n");
                return 0;
            }
        }else {
            printf("ERROR: Encode validation failed!\n");
            return 0;
        }
        
    }else if(ret==e_decode){
        Status decode_ret = read_valid_arg_decode(argv, &dInfo);

        if(decode_ret==e_success){
            printf("\n\n<--------------Decoding Started--------------->\n\n");
            Status res= do_decoding(argv[2],&dInfo);
            if(res == e_success)
            printf("\n<----------------Decoding Done---------------->\n\n");
            else {
                printf("ERROR: Decoding failed!\n");
                return 0;
            }
        }else {
            printf("ERROR: Decode validation failed!\n");
            return 0;
        }
        
    }else {
        printf("ERROR: No valid cmd passed!\nCommand should be like:\nFor encoding: ./a.out -e beautiful.bmp sample.txt encoded_file_name(optional)\n"
                 "For decoding: ./a.out -d stego.bmp ""#*"" decoded_file_name(optional)\n");
        return 0;
    }

        return 0;
}

OperationType check_operation_type(char *argv[]){
    if(argv[1]==NULL) return e_unsupported;
    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }else if(strcmp(argv[1],"-d")==0) return e_decode;
    else return e_unsupported;
}
