#ifndef ENCODE_H
#define ENCODE_H
#include <stdlib.h>
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char *secret_data;
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_valid_arg(char *argv[], EncodeInfo *eInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *eInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *eInfo);

/* check capacity */
Status check_capacity(EncodeInfo *eInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
long get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *eInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *eInfo);

/* Encode secret file extenstion size*/
Status encode_secret_file_extn_size(int ext_size, EncodeInfo *eInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *eInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *eInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char image_buffer[], FILE *fptr_dest);

/* Encode 4-byte into LSB of image data array */
Status encode_4byte_to_lsb(int size, char *image_buffer, FILE *fptr_dest);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);


#endif
// header, magic_string, sec_file_ext_size, secret_file_ext, sec_file_size, sec_file_data