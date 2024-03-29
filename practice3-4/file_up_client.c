#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 32
void error_handling(char *message);

int main(int argc, char *argv[])
{
   int sd;
   FILE *fp;
   
   char file_name[BUF_SIZE];
   char buf[BUF_SIZE];
   int read_cnt;
   int read_size;
   struct sockaddr_in serv_adr;
   if (argc != 4) {
      printf("Usage: %s <IP> <port> <file name> \n", argv[0]);
      exit(1);
   }

   strcpy(file_name, argv[3]);
   fp = fopen(argv[3], "rb");
   sd = socket(PF_INET, SOCK_STREAM, 0);   

   memset(&serv_adr, 0, sizeof(serv_adr));
   serv_adr.sin_family = AF_INET;
   serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
   serv_adr.sin_port = htons(atoi(argv[2]));

   connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    // TODO: Send file name to server 
   read_cnt = strlen(file_name);
   send(sd, file_name, read_cnt, 0);
    // send(sd, file_name, strlen(file_name), 0);
    
    // TODO: Send file data (Hint: file_server.c)
   read_size=0;
   while ((fread(buf, 1, BUF_SIZE, fp)) > 0) {
      read_size += fread(buf, 1, BUF_SIZE, fp);
        send(sd, buf, read_size, 0);
    }

    printf("Send total %d bytes \n", read_size);

    // TODO: shutdown 
    shutdown(sd, SHUT_WR);

    // TODO: read complete message from server 
    read(sd, buf, BUF_SIZE);
   printf("Message from server: %s \n", buf);

   fclose(fp);
   close(sd);
   return 0;
}

void error_handling(char *message)
{
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}
