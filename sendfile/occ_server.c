#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define FILE_LEN 32
#define BUF_SIZE 1024
void error_handling(char *message);
void *handle_client(void *arg);
char filename[100];
char ipadd[100];

int main(int argc, char *argv[])
{
	pthread_t thread; 
	int serv_sd, clnt_sd;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	
	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sd = socket(PF_INET, SOCK_STREAM, 0);   
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if (bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sd, 5) == -1)
		error_handling("listen() error");

	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);    
		clnt_sd = accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);	

		// TODO: pthread_create & detach
		pthread_create(&thread, NULL, handle_client, (void *)&clnt_sd);
		pthread_detach(thread);
        strcpy(ipadd,inet_ntoa(clnt_adr.sin_addr));
	}

	close(serv_sd);
	return 0;
}

void *handle_client(void *arg) {
    int clnt_sd = *((int *)arg);

    char file_name[FILE_LEN];
    char buf[BUF_SIZE];
    int file_size = 0;

    read(clnt_sd, file_name, FILE_LEN);
    file_name[strcspn(file_name, "\n")] = 0; 
    strcpy(filename,file_name);
    printf("Received %s from %s\n", file_name,ipadd);
    //printf("Received %s from %s\n",file_name,inet_ntoa(clnt_adr.sin_addr));
    FILE *fp = fopen(file_name, "wb");

    if (fp == NULL) {
        error_handling("File open error");
    }

    while (1) {
        int read_cnt = read(clnt_sd, buf, BUF_SIZE);
        if (read_cnt <= 0) {
            break;
        }
        fwrite(buf, 1, read_cnt, fp);
        file_size += read_cnt;
    }

    const char *ack = "File received successfully!";
    write(clnt_sd, ack, strlen(ack) + 1);

    printf("Compile %s and return results\n", file_name);
    fclose(fp);
    char a[100];
    strcpy(a,"gcc ");
    strcat(a,file_name);
    // printf("%s",a);
    // char b[100];
    // strcpy(b,"./a.out");

    FILE *fp1 = popen(a,"r");
    char c[BUF_SIZE];

    while(fgets(c,BUF_SIZE,fp1)!=NULL){
        printf("%s",c);
    }
    pclose(fp1);

    char b[100];
    strcpy(b,"./a.out");
    FILE *fp2 =popen(b,"r");

    char d[100];
    while(fgets(d,BUF_SIZE,fp2)!=NULL){
        printf("%s",d);
    }
    pclose(fp2);
    
    // fclose(fp);
    close(clnt_sd);
    printf("-----------------------------------------------------\n\n");

    return NULL;
}


void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
