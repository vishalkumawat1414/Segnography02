#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    rewind(fptr_image);
    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *eInfo)
{
    // Src Image file
    eInfo->fptr_src_image = fopen(eInfo->src_image_fname, "r");
    // Do Error handling
    if (eInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", eInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    eInfo->fptr_secret = fopen(eInfo->secret_fname, "r");
    // Do Error handling
    if (eInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", eInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    eInfo->fptr_stego_image = fopen(eInfo->stego_image_fname, "w");
    // Do Error handling
    if (eInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", eInfo->stego_image_fname);

        return e_failure;
    }
    // No failure return e_success
    return e_success;
}

//Checking the CLA (user provide arg) are valid or not
Status read_valid_arg( char *argv[], EncodeInfo *eInfo)
{
    if (argv[2] == NULL)
        return e_failure;
    if (strstr(argv[2], ".bmp") == NULL)
        return e_failure;
    eInfo->src_image_fname = argv[2];
    if (strstr(argv[3], ".") == NULL)
        return e_failure;
    eInfo->secret_fname = argv[3];
    if (argv[4] != NULL){
        if (strstr(argv[4], ".bmp"))
            eInfo->stego_image_fname = argv[4];
        else
            eInfo->stego_image_fname = "stego.bmp";
    }
    else
        eInfo->stego_image_fname = "stego.bmp";
    
    return e_success;
}

Status do_encoding(EncodeInfo *eInfo){
    Status res = check_capacity(eInfo);
    open_files(eInfo);
    if(res==e_success) {
        //Header copying
        Status hres = copy_bmp_header(eInfo->fptr_src_image,eInfo->fptr_stego_image);
        if(hres==e_failure) return hres;

        // Magic String copy
        Status msres = encode_magic_string("#*",eInfo);
        if(msres==e_failure) return msres;
        
        // Getting extension and len for secret file
        char *secExt = strstr(eInfo->secret_fname, ".");
        int secExtLen = strlen(secExt);

        // Secrect Extension size copied
        Status ext_size = encode_secret_file_extn_size(secExtLen, eInfo);
        if (ext_size == e_failure) return ext_size;

        //Secret file extension copied
        Status secRes = encode_secret_file_extn(secExt,eInfo);
        if(secRes==e_failure) return secRes;
        
        //Secret file size copied
        Status sec_size = encode_secret_file_size(eInfo->size_secret_file,eInfo);
        if(sec_size==e_failure) return sec_size;
        
        //Secret file data encoded
        Status sec_data = encode_secret_file_data(eInfo);
        if(sec_data==e_failure) return sec_data;
 
        //coping remaing data
        Status remainData = copy_remaining_img_data(eInfo->fptr_src_image, eInfo->fptr_stego_image);
        if(remainData==e_failure) return remainData;

        fclose(eInfo->fptr_stego_image);
        fclose(eInfo->fptr_src_image);
    }
    return res;
}

Status check_capacity(EncodeInfo *eInfo){
    //Src size
    FILE*src = fopen(eInfo->src_image_fname,"r");
    int Src_fsize = get_image_size_for_bmp(src);
    fclose(src);
    
    // Size of secrete file
    FILE*sec = fopen(eInfo->secret_fname,"r");
    long Sec_fsize = get_file_size(sec);
    rewind(sec);
    eInfo->size_secret_file = Sec_fsize;  //size to structure

    eInfo->secret_data = (char*)calloc(1,eInfo->size_secret_file+1);

    fread(eInfo->secret_data,1,eInfo->size_secret_file,sec);  //secret data to structure
    
    fclose(sec);

    int src_extns = strlen(strstr(eInfo->secret_fname,".")); //size of src extension
    int sec_extns = strlen(strstr(eInfo->secret_fname,"."));

    if(Src_fsize<54+16+src_extns*8+sec_extns*8+32+Sec_fsize*8){   
        printf("ERROR: Souce file size is not enough!\n");
        return e_failure;
    }
    return e_success;
}

long get_file_size(FILE *fptr){
    fseek(fptr, 0, SEEK_END);
    int Sec_fsize = ftell(fptr);
    rewind(fptr);
    // fclose(fptr);
    return Sec_fsize;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_fptr_dest_image){
      rewind(fptr_src_image);
      char buffer[54];
      //read data from src and push to dest
      fread(buffer,1,sizeof buffer,fptr_src_image);
      fwrite(buffer,1,sizeof buffer,fptr_fptr_dest_image);

      if (ftell(fptr_src_image) == 54 && ftell(fptr_fptr_dest_image)==54){
          printf("SUCCESS: Header transfered successfully!\n");
        return e_success;
      }
      else return e_failure;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *eInfo){
    char buffer[8];
    int j = 0;
 
    //encoding the magic string
    Status res = e_success;
    while (magic_string[j]){
        // getting data in buffer
        fread(buffer,1,sizeof buffer,eInfo->fptr_src_image);

        //passed for ecoding
        res = encode_byte_to_lsb(magic_string[j], buffer, eInfo->fptr_stego_image);
        if(res == e_failure) break;
        j++;
    }
    if(res==e_success) printf("SUCCESS: Magic String Encoded!\n");
    else printf("ERROR: Magic String Encoding Failed!\n");
    return res;
}

Status encode_byte_to_lsb(char data, char image_buffer[], FILE *fptr_dest){
    int pos = 7;
    for (int i = 0; i < 8; i++)
    {
        uint mask = 1 << pos;
        uint res = data & mask;
        res = res >> pos;
        // clear lsb of buffer
        image_buffer[i] = image_buffer[i] & (-1 << 1);
        res = res | image_buffer[i];
        image_buffer[i] = res;
        pos--;
    }
    // pushing arr data to stego.bmp file
    fwrite(image_buffer, 1, 8, fptr_dest);

    if (pos == -1)
        return e_success;
    else
        e_failure;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *eInfo){
    char buffer[8];
    int j = 0;

    // encoding the secret_file_extn
    Status res = e_success;
    while (file_extn[j])
    {
        // getting data in buffer
        fread(buffer, 1, sizeof buffer, eInfo->fptr_src_image);
        // passed for ecoding
        res = encode_byte_to_lsb(file_extn[j], buffer, eInfo->fptr_stego_image);
        if (res == e_failure)
            break;
        j++;
    }
    if (res == e_success)
        printf("SUCCESS: File Extension Encoded!\n");
    else
        printf("ERROR: File Extension Encoding Failed!\n");
    return res;
}

Status encode_secret_file_extn_size(int ext_size, EncodeInfo *eInfo){
        char buffer[32];

        Status res = e_success;
        // getting data in buffer
        fread(buffer, 1, sizeof buffer, eInfo->fptr_src_image);
        // passed for ecoding
        res = encode_4byte_to_lsb(ext_size, buffer, eInfo->fptr_stego_image);
    
        printf("SUCCESS: File Extension_Size Encoded!\n");
        return res;
}

/* Encode 4 byte into LSB of image data array */
Status encode_4byte_to_lsb(int size, char *image_buffer, FILE *fptr_dest){

    int pos = 31;
    for (int i = 0; i < 32; i++)
    {
        uint mask = 1 << pos;
        uint res = size & mask;
        res = res >> pos;
        // clear lsb of buffer
        image_buffer[i] = image_buffer[i] & (-1 << 1);
        res = res | image_buffer[i];
        image_buffer[i] = res;
        pos--;
    }
    // pushing arr data to stego.bmp file
    fwrite(image_buffer, 1, 32, fptr_dest);

    if (pos == -1)
        return e_success;
    else
        e_failure;
}

Status encode_secret_file_size(long file_size, EncodeInfo *eInfo){
    char buffer[32];

    Status res = e_success;
    // getting data in buffer
    fread(buffer, 1, sizeof buffer, eInfo->fptr_src_image);
    // passed for ecoding
    res = encode_4byte_to_lsb(file_size, buffer, eInfo->fptr_stego_image);

    printf("SUCCESS: Secret File Size Encoded!\n");
    return res;
}

Status encode_secret_file_data(EncodeInfo *eInfo){
    char buffer[8];
    int j = 0;

    // encoding the secret_file_extn
    Status res = e_success;
    while (j<eInfo->size_secret_file){
        // getting data in buffer
        fread(buffer, 1, sizeof buffer, eInfo->fptr_src_image);
        // passed for ecoding
        res = encode_byte_to_lsb(eInfo->secret_data[j], buffer, eInfo->fptr_stego_image);
        if (res == e_failure)
            break;
        j++;
    }
    if (res == e_success)
        printf("SUCCESS: Secret File Data Encoded!\n");
    else
        printf("ERROR: Secret File Data Encoding Failed!\n");
    return res;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
  
    char buffer[1024];
    while (fread(buffer, 1, 1024,fptr_src )>0){
           fwrite(buffer,1,1024,fptr_dest);
    }
    printf("SUCCESS: Remaining Data Encoded!\n");
}