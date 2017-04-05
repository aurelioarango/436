//Aurelio Arango

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

using namespace std;

#define MAXLINE	256

const int backlog = 4;
int queue=0;

void *process_client(void *arg)
{
	char str[MAXLINE];
    int  new_port, n;
	
	
    int  fd = *(int*)(arg);
	//trying to read file
	if ((n = recv(fd, str, MAXLINE,0)) == 0)
	{
           close(fd);
    }
	else
	{
		string input(str);//creating a string object
		
		queue++;//increasing number of ports
		new_port+queue;
		char * new_port_S;//port to be used to communicate 
		//itoa (new_port,new_port_S,10);
		//open a new connection to send data over the new connection and send info
		
		
		//getting command, and checking if the appropriate request was sent
		if(input.find("HEAD")!=std::string::npos || input.find("PUT")!=std::string::npos
		   ||input.find("DELETE")!=std::string::npos || input.find("GET")!=std::string::npos)
		{
			if(input.find("HEAD")!=std::string::npos)
			{
				//open headerfile
				fstream infile1;
				try
				{
					infile1.open ("head.txt", fstream::in );	
					string out;
					while(!infile1.eof())//reading file 
					{
						getline(infile1,out);//read line and then send out
						out.append("\n");
						send(fd,out.c_str(), out.length(),0);
					}
					close(fd);//closing connection
					infile1.close();
				}
				catch(exception e)
				{
					
				}
			}
			if(input.find("PUT")!=std::string::npos)
			{
				string out="Updating...\n";
				unsigned pos = input.find(" ");
				unsigned posend = input.find("\n");
				string file_name ="copy";
				file_name += input.substr(pos+1,posend-3);//file to be save as
				
				ofstream file;
				string file_contents="start";
				file.open(file_name.c_str());//opening file
				//read and write
				
				
				while (file_contents.length() > 1)
				{
					
					file_contents="";//clear entry
					recv(fd, str, MAXLINE,0);//read line
					
					file_contents.append(str);//store in file contents
					str[0] ='\0';//setting to 0
					file<<file_contents;//saving to file
				}
				cout <<"DONE"<<endl;
				file.close();//closing file
				send(fd,out.c_str(), out.length(),0);
				close(fd);
			}
			if(input.find("DELETE")!=std::string::npos)
			{
				string out="DELETING...\n";
				unsigned pos = input.find(" ");//find first space
				unsigned posend = input.find("\n");
				string file = input.substr(pos+1,posend-3);//get from white space to the end of the file
				cout <<file<<endl;
				if((remove(file.c_str())) !=0 )//trying to remove file
				{
					out.append("Error Deleting File.\n");
				}
				else
				{
					out.append("File Successfully Deleted.\n");
				}
				send(fd,out.c_str(), out.length(),0);
				close(fd);
			}
			if (input.find("GET")!=std::string::npos &&input.find("index.html")!=std::string::npos )
			{
				//open file an sent request
				fstream infile;
				try
				{
					infile.open ("head.txt", fstream::in );
					
					string out;
					while(!infile.eof())//reading file 
					{
						getline(infile,out);//read line and then send out
						out.append("\n");
						send(fd,out.c_str(), out.length(),0);
					}
					
					infile.close();
					out="";
					//cout <<"From client(try block): " <<input << endl;
					if(input.find("index.html") )
					{
						infile.open ("index.html", fstream::in );
					}
					else
					{
						unsigned first = input.find("/");
						//unsigned second = input.find("/",first+1);
						string filename =input.substr(first+1,first+1);//trying to open a file
						
						infile.open(filename.c_str(),fstream::in);
					}
					

					while(!infile.eof())//reading file 
					{
						getline(infile,out);//read line and then send out
						out.append("\n");
						send(fd,out.c_str(), out.length(),0);
					}
					infile.close();
					close(fd);//closing connection
				}
				catch(exception e)
				{
					
				}
			}
			else
			{
				string out = "404 Not Found";
				send(fd, out.c_str(), out.length(),0);
				close(fd);
			}
		}
		else
		{
			string out = "400 Bad Request";
			send(fd, out.c_str(), out.length(),0);
			close(fd);
		}
		
	}
}

int main(int argc, char *argv[])
{
    //fd, socket descriptor
    int	socket_fd=0, conn_fd=0;
	socklen_t client_len;
    struct 	sockaddr_in client_addr, server_addr;
    pthread_t thread_id;
    
    if (argc != 3)
    {
		printf("Usage: caseServer <address> <port> \n");
        return -1;
	}
    //descriptor = socket(protofamily,type, protocol)
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd == -1)
	{
		fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
                errno, strerror(errno));
		return -1;
	}
    
    //setting structs to zero
    bzero(&server_addr, sizeof(server_addr));
    
    server_addr.sin_family 	   = AF_INET;
	server_addr.sin_addr.s_addr   = inet_addr(argv[1]);//ip address
	server_addr.sin_port          = htons(atoi(argv[2]));//port number
    
    //bind, to supply a protocol port number at which the server will wait for contact
    //bind (socket, local_address, address_len)
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
        fprintf(stderr, "Error binding to socket, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;

	}
    // listen makes the socket ready to wait for contact from other clients
    //listen(socket,queue_size)
    if (listen(socket_fd, backlog) == -1)
	{
        fprintf(stderr, "Error listening for connection request, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;
	}
    
    while (true)
	{
		client_len = sizeof(client_addr);
        //Accept() causes the system to block until a connection is made
        //returns a new file descriptor
        //
		if ((conn_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len)) < 0 )
		{
			if (errno == EINTR)
				continue;
			else
            {
                fprintf(stderr, "Error connection request refused, errno = %d (%s) \n",
                        errno, strerror(errno));
            }
		}
		
        if (pthread_create(&thread_id, NULL, process_client, (void *)&conn_fd) != 0)
        {
            fprintf(stderr, "Error unable to create thread, errno = %d (%s) \n",
                       errno, strerror(errno));
        }
    }
    
	return 0;
}