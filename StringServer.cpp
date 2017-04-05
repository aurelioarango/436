//
// StringServer.cpp: implementation of StringServer
//

#include "StringServer.h"
#include <TcpSocket.h>
#include <ISocketHandler.h>
#include <ListenSocket.h>
#include <SocketHandler.h>
#include <iostream>
#include <sstream>
#include <string.h> // for strncpy

using namespace std;

StringServer* StringServer::cur_server = NULL;

void StringServer::StringSocket::OnRead()
{
   // OnRead of TcpSocket actually reads the data from the socket
   // and moves it to the input buffer (ibuf)
   TcpSocket::OnRead();
   // get number of bytes in input buffer
   size_t n = ibuf.GetLength();
   if (n > 0)
   {
      // TCP_BUFSIZE_READ is the maximum size of the standard input buffer
      //   of TcpSocket
      char buf[TCP_BUFSIZE_READ + 1];
      ibuf.Read(buf, n);
      if ( n > TCP_BUFSIZE_READ )
         n = TCP_BUFSIZE_READ;
      buf[n] = '\0';            // ensure null-terminated
      string input(buf);
      bool has_length;         // if input has length, output will too
      {
         // see if there's a length in the input
         string::size_type bar_pos = input.find('|');
         // cerr << "Bar_pos: " << bar_pos << endl;
         has_length = bar_pos != string::npos && bar_pos > 0 && bar_pos < 20;
         if ( has_length )
            input = input.substr(bar_pos + 1);
      }
      if ( my_server->echoStrings() )
         cerr << "Input: " << input << endl;

      string result = my_server->process(input);

      if ( my_server->echoStrings() )
      {
         cerr << " - result: " << result;
         if ( result.empty() || result[result.size() - 1] != '\n' )
            cerr << endl;
      }

      if ( has_length )
      {
         ostringstream numstr;
         numstr << result.size();
         result = numstr.str() + "|" + result;
      }
      else if ( result.empty() || result[result.size() - 1] != '\n' )
         result += "\n";
      
      strncpy(buf, result.c_str(), TCP_BUFSIZE_READ);
      buf[TCP_BUFSIZE_READ] = '\0'; // ensure null-terminated

      Send(buf);
   }
}

//----------------------------------------------------------------------

void StringServer::run()
{
   SocketHandler socket_handler;
   ListenSocket<StringServer::StringSocket> listener(socket_handler);
   
   if ( listener.Bind(port) )
   {
      cerr << "Could not bind to socket " << port << endl;
      exit(-1);
   }
   socket_handler.Add(&listener);
   socket_handler.Select(1,0);
   
   while (true)
   {
      socket_handler.Select(1,0);
   }
}
