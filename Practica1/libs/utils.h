/*Angel Farre - angel.farre & Enric Gutierrez - enric.gutierrez*/

#ifndef _UTILS_H_
#define _UTILS_H_

#include "types.h"

#define ERR_PARAM "Error in the number of parameters introduced!\n"
#define ERR_CREATE_THREAD "Error creating thread!\n"
#define ERR_CONN_LIONEL "Error when connecting to Lionel!\n"
#define ERR_CONN_MC "Error when connecting to McGruder!\n"
#define ERR_SENT "Error sending the files.\n"
#define ERR_FOLDER "Could not open \"/files\" directory.\n"
#define ERR_FILE "Error opening the file!\n"
#define ERR_OPEN_IMAGE "Could not open image!\n"
#define ERR_CREATE_IMAGE "Could not create an image!\n"
#define ERR_KALKUN "Error updating Kalkun files!\n"
#define ERR_PIPE "Error generating a pipe!\n"
#define ERR_FORK "Error generating a fork!\n"
#define ERR_EXECL "Error executing execl!\n"
#define ERR_MAIL "Error creating the mailbox!\n"
#define ERR_MAIL_SEND "Error sending a message through the mailbox!\n"
#define ERR_MAIL_READ "Error reading content of the mailbox!\n"

#define WAR_PAQUITA "Warning: Paquita data not updated\n"
#define PQT_DATA "Showing data from Paquita: \n\n"

#define CONN_RDY "Connection ready.\n"
#define DISCON "\nDisconnecting Telescope1.\n"
#define NO_FILES "No files found.\n"
#define SENT "Files sent.\n"
#define CONN_MSG "Connecting to Lionel...\n"

#define TRANSMISSION_OK "0x03[FILEOK]0[]"
#define TRANSMISSION_KO "0x03[FILEKO]0[]"

#define IMAGE_MD5SUM_OK "Image recieved succesfully!\n"
#define IMAGE_MD5SUM_KO "Error recieving the image. MD5SUM does not match!\n"



char* get_string();

char* read_string(int fd, char end);

char* get_filename_ext(char *filename);

int fsize(const char *filename);

Data get_metadata_fields (char* string);

File_Info get_metadata_data(char* string, int size);

char* retrieve_actual_data(char* data);

Last_Astronomical_Data treat_astronomical_data(char* data, int size);

int fileSize(char* filePath);

char* get_md5sum(char* path, char * filename);

#endif
