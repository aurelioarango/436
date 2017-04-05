//
// StringServer.h: server to provide simple processing of std::string objects
//

#ifndef STRINGSERVER_H
#define STRINGSERVER_H

#include <TcpSocket.h>
#include <string>
class ISocketHandler;

class StringServer
{
public:
   static const int DEFAULT_PORT_ID = 9001;
   
   // initialize port
   StringServer(int port_id = DEFAULT_PORT_ID) : port(port_id)
   {
      cur_server = this;
   }

   // run the server - this function never returns
   void run();

   // set "echo" mode in which all received and sent strings are echoed
   //  to cerr (for debugging)
   void setEcho(bool value) { echo_strings = value; }
   // returns whether to echo strings
   bool echoStrings() const { return echo_strings; }

   // method to process a received string; client programs need to define this
   virtual std::string process(std::string input) = 0;

protected:
   bool echo_strings;           // echo all strings as processed?
   int  port;                   // port to use for server

private:
   static StringServer *cur_server; // most recently created object

   class StringSocket : public TcpSocket
   {
   public:
      StringSocket(ISocketHandler& h)
         : TcpSocket(h), my_server(StringServer::cur_server)
      { }
      void OnRead();
   protected:
      StringServer *my_server;  // server containing this socket
   };
};

#endif
