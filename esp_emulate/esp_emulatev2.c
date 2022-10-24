#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netdb.h>
#include<stdlib.h>
#include<unistd.h>

void main(){
	char buff[44100];
	FILE *fptr;
	fptr=fopen("Dwn_Audio.raw","rb");
	fread(buff,1,44100,fptr);
	struct addrinfo hints,*res;
	memset(&hints,0,sizeof(hints));
	getaddrinfo("192.168.68.200","5000",&hints,&res);
	int s;
	s=socket(AF_INET,SOCK_STREAM,0);
	if(s==-1){
		printf("Error unable to create socket");
	}
	connect(s,res->ai_addr,res->ai_addrlen);
	int i=50000;
	send(s,buff,44100,0);
	
}
