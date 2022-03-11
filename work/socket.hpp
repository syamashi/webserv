#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "util.hpp"
#include "defines.hpp"

class Socket {
 public:
  Socket();
  ~Socket();
  Socket(Socket const &src);
  Socket& operator=(const Socket &src);

  int Create();
  int Passive (short int sPort) { return (Passive(sPort, 0)); }
  int Passive(short int sPort, int iReuse);
  Socket * Accept();               // Listen for connections
  int Connect(char *szBuf, short sPort);  // Connect the socket to the remote host
  int Recv() { return (Recv(MAX_SOCK_BUFFER)); } // Receive bytes on this socket
  int Recv(int iBytes);            // Receive up to iBytes on this socket
  int RecvTeol(){ return (RecvTeol(0)); } // Receive up to the telnet eol
  int RecvTeol(int iToast);        // Receive up to the telnet eol
                                   // and possibly remove the telnet eol
  int Send(char *szBuf, int iLen);  // Send the buffer on this socket
  int Send(char *szBuf);            // Send the text buffer on this socket
  int Send(const char *szBuf);      // Send the text buffer on this socket
  int SendText(char *szFileName);  // Send this text file across the socket
  int SendBinary(char *szFileName);// Send this binary file across the socket
  int ResolveName();               // Look up the ip address and name of the peer
  int Close();                      // Close this socket
  void debug();

  int iSock;                // The socket number allocated
  int iErr;                 // The last error code on a socket call
  char *szOutBuf;           // Used to return data in
  char *szPeerName;         // The ip name of the peer connected.
  char *szPeerIp;           // The ip address of the peer connected.
  unsigned long ulTimeout;  // The timeout for receives in seconds.

 protected:
  struct sockaddr_in siUs;    // Our address
  struct sockaddr_in siThem;  // Their address
  short int sPortUs;          // Our port
  short int sPortThem;        // Their port
  socklen_t iLen;                   // The size of siUs and siThem
  int iBuf;                   // Active buffer flag.
  char *szBuf1, *szBuf2;      // Internal buffers.
  int iBeg1, iEnd1,           // Buffer markers.
      iBeg2, iEnd2;
};

#endif
