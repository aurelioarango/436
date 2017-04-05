#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>
using namespace std;

#define MAXLINE 1024

void str_client(FILE *fp, int socket_fd);

int main(int argc, char *argv[])
{
	int	socket_fd;
	struct  sockaddr_in servaddr;

	if (argc != 3) {
		printf("Usage: caseClient <address> <port> \n");
        return -1;
	}

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
        fprintf(stderr, "Error creating socket, errno = %d (%s) \n", 
                errno, strerror(errno));
        return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	if (connect(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        fprintf(stderr, "Error unable to connect to socket, errno = %d (%s) \n", errno,
                strerror(errno));
        return -1;
	}

	str_client(stdin, socket_fd);

	return 0;

}

void str_client(FILE *pFile, int socket_fd)
{
     
     char userInput[MAXLINE];
     
    if(fgets(userInput, MAXLINE, pFile) != NULL)
    {
		//information that will be sent to the server
		send(socket_fd, (void *) userInput, strlen(userInput), 0);
		
		string userinput;
		
		userinput = userInput;
			
		if(userinput.find("PUT")!=std::string::npos)
		{
		
			
			//open file an sentd request
			fstream infile;
			string out;
			
			if(userinput.find("putting.html") != std::string::npos)
			{  
				try
				{		
				infile.open("putting.html", fstream::in);
			
					while(!infile.eof())//reading file 
					{
						getline(infile,out);//read line and then send out
						out.append("\n");
						send(socket_fd,out.c_str(), out.length(),0);
					}
					
					infile.close();
					close(socket_fd);//closing connection
				}
				catch(exception e)
				{
					
				}
			} 
			else
			{
				string out = "FILE Not Found";
				//send(fd, out.c_str(), out.length(),0);
				cout << out << endl;
				close(socket_fd);
			}
		}
		else
		{
			if(userinput.find("DELETE")!=std::string::npos||userinput.find("GET")!=std::string::npos||userinput.find("HEAD")!=std::string::npos)
			{
			   char	sndLine[MAXLINE];
			   char	rcvLine[MAXLINE];
			   string buffer;
			   memset((void *)sndLine, 0, MAXLINE);
			   memset((void *)rcvLine, 0, MAXLINE);
		   
			   if (fgets(sndLine, MAXLINE, pFile) != NULL)
			   {
				   send(socket_fd, (void *)sndLine, strlen(sndLine),0);
				   string temp;
				   getline(cin,temp);
				   
				   while ( true)
				   {
					   recv(socket_fd, rcvLine, MAXLINE,0);
					   //printf("ERROR: server terminated \n");
					   fputs(rcvLine, stdout);
					   rcvLine[0] ='\0';
					   return;
					}
				   
				   fputs(rcvLine, stdout);
			   }
			}
		}
    }     
	exit(1);	
		
     
}

	
