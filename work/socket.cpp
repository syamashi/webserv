#include "socket.hpp"
#include "ftutil.hpp"

template<typename T>
void puterr(T t){
  cerr << "[" << t << "]" << endl;
}

template<typename T, typename S>
void puterr(T t, S s){
  cerr << "[" << t << "]" << " " << s << endl;
}

template<typename T, typename S, typename U>
void puterr(T t, S s, U u){
  cerr << "[" << t << "]" << " " << s << " " << u << endl;
}

Socket::Socket(){
  iLen = sizeof(siUs);
  iSock = -1;
  iErr = 0;
  szOutBuf = new char[MAX_SOCK_BUFFER];
  szBuf1 = new char[(MAX_SOCK_BUFFER) / 2];
  szBuf2 = new char[(MAX_SOCK_BUFFER) / 2];
  iBeg1 = iEnd1 = iBeg2 = iEnd2 = 0;
  iBuf = 1;
  szPeerIp = NULL;
  szPeerName = NULL;
  ulTimeout = 5 * 60;  // 5 minutes default.
}

Socket::Socket(Socket const &src){
    *this = src;
}

Socket::~Socket() {
  if (iSock > -1) close(iSock);
  delete[] szOutBuf;
  delete[] szBuf1;
  delete[] szBuf2;
  if (szPeerIp) delete[] szPeerIp;
  if (szPeerName) delete[] szPeerName;
}

Socket& Socket::operator=(const Socket &src) {
  if (this == &src)
    return (*this);

  iLen = src.iLen;
  iSock = src.iSock;
  iErr = src.iErr;
  strcpy(szOutBuf, src.szOutBuf);
  strcpy(szBuf1, src.szBuf1);
  strcpy(szBuf2, src.szBuf2);
  iBeg1 = src.iBeg1;
  iEnd1 = src.iEnd1;
  iBeg2 = src.iBeg2;
  iEnd2 = src.iEnd2;
  iBuf = src.iBuf;
  szPeerIp = src.szPeerIp;
  szPeerName = src.szPeerName;
  ulTimeout = src.ulTimeout;
  return (*this);
}

int Socket::Create(){
  // set TCPsocket
  iSock = socket(AF_INET, SOCK_STREAM, 0);
  return iSock;
}

int Socket::Passive(short int sPort, int iReuse) {
  int optval = 1;

  if (iReuse > 0) {  // Force reuse of the address.
    setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
  }

  sPortUs = sPort;

  bzero((void *)&siUs, iLen);  // make sure everything zero
  siUs.sin_family = AF_INET;
  siUs.sin_port = htons(sPortUs);
  siUs.sin_addr.s_addr = INADDR_ANY;

  // Bind to the given port
  iErr = bind(iSock, (struct sockaddr *)&siUs, iLen);
  if (iErr < 0) {
    return iErr;
  }

  // change to passive socket
  iErr = listen(iSock, SOMAXCONN);  // SOMAXCONN:1024 通常5-10
  if (iErr < 0) {
    return iErr;
  }

  return 0;
}

Socket *Socket::Accept() {  // Listen for connections
  Socket *sSock;
  sSock = new Socket();

  bzero(&siThem, iLen);
  // client socket
  sSock->iSock = accept(iSock, (struct sockaddr *)&(sSock->siThem), &iLen);
  if (sSock->iSock < 0) {
    iErr = sSock->iSock;
    delete sSock;
    return NULL;
  }

  sSock->szPeerIp = new char[128];
  strncpy(sSock->szPeerIp, inet_ntoa(sSock->siThem.sin_addr), 128);

  return sSock;
}

/*
in <netdb.h>
struct	hostent {
        char	*h_name;	// official name of host
        char	**h_aliases;	// alias list
        int	h_addrtype;	// host address type
        int	h_length;	// length of address
        char	**h_addr_list;	// list of addresses from name server
  #define	h_addr	h_addr_list[0]	// address, for backward compatibility
};
*/

int Socket::Connect(char *szBuf,
                    short sPort) {  // Connect the socket to the remote host
  struct hostent *heHost;

  heHost = gethostbyname(szBuf);
  if (heHost == NULL) {
    return (iErr = 1);
  }

  bzero((void *)&siUs, iLen);
  siUs.sin_family = AF_INET;
  siUs.sin_port = htons(sPort);
  siUs.sin_addr.s_addr = *((u_long *)heHost->h_addr);

  iErr = connect(iSock, (sockaddr *)&siUs, iLen);

  return (iErr);
}

///////////////////
int Socket::Recv(int iBytes) {  // Receive up to iBytes on this socket
  return 0;
}

/*
struct timeval
{
        long tv_sec;
        long tv_usec;
};
*/

/*
recv_input for szOutBuf
*/
int Socket::RecvTeol(int iToast) {  // Receive up to the telnet eol and
                                    // possibly remove the telnet eol
  int i;
  int iState = 1, idx = 0;
  int nfds = 0;
  fd_set fdsSocks;
  struct timeval stTimeout;

  bzero(szOutBuf, MAX_SOCK_BUFFER);
  while (iState != 0) {
    switch (iState) {
      case 1:  // Figure out where to start.
      {
        if ((iEnd1 == 0) && (iEnd2 == 0)) {  // Both buffers empty.
          iState = 2;
        } else {
          iState = 3;
        }
        break;
      }
      case 2:  // Fill the buffers with data.
      {
        FD_ZERO(&fdsSocks);        // 集合の消去
        nfds = 0;
        FD_SET(iSock, &fdsSocks);  // fdsSocksにiSockを追加する
        nfds = max(nfds, iSock);
        stTimeout.tv_sec = ulTimeout;
        iErr = select(nfds + 1, &fdsSocks, NULL, NULL,
                      &stTimeout);  // max{r,w,e}+1, read, write, ex, time
        if (iErr < 1) {             // Error occured.
          return -1;
        }
        // message length = recv()
        iErr = recv(iSock, szBuf1, (MAX_SOCK_BUFFER) / 2, 0);  // sockfd, buf, len, flags
        if (iErr < 1) {
          iState = 0;
          break;
        }

        iBeg1 = 0;
        iEnd1 = iErr;
        if (iErr == (MAX_SOCK_BUFFER) / 2)  // Filled up Buffer 1.
        {
          FD_ZERO(&fdsSocks);
          nfds = 0;
          FD_SET(iSock, &fdsSocks);
          nfds = max(nfds, iSock);
          stTimeout.tv_sec = ulTimeout;
          iErr = select(nfds + 1, &fdsSocks, 0, 0, &stTimeout);
          if (iErr < 1) {  // Error occured.
            return -1;
          }
          iErr = recv(iSock, szBuf2, (MAX_SOCK_BUFFER) / 2, 0);
          if (iErr < 1) {
            iState = 0;
            break;
          }
          iBeg2 = 0;
          iEnd2 = iErr;
        }
        iBuf = 1;
        iState = 3;  // Advance to the next state.
        break;
      }
      case 3:  // Look for the EOL sequence.
      {
        if ((iBuf == 1) && (iEnd1 != 0)) {  // Use Buffer 1 first. iEnd1 = szBuf1.size()
          for (; iBeg1 < iEnd1; ++iBeg1) { // iBeg1[0, iEnd1)
            szOutBuf[idx] = szBuf1[iBeg1];  // Copy.   idx[0, N)
            if ((szOutBuf[idx] == '\n') || (szOutBuf[idx] == '\r')) {
              ++iBeg1;  // Count the char just read.
              if ((szOutBuf[idx] == '\r') && (szBuf1[iBeg1] == '\n')) {
                // Using CRLF as end-of-line.
                ++idx;
                szOutBuf[idx] = szBuf1[iBeg1]; // '\n'
                ++iBeg1;  // Consume LF.
              }
              szOutBuf[idx + 1] = '\0';  // True. Null line.
              iState = 4;                // Goto cleanup & exit.
              break;                     // Break from for loop.
            }
            ++idx;                             // Advance to next spot.
            if ((idx + 1) == MAX_SOCK_BUFFER)  // Out of room.
            {
              szOutBuf[MAX_SOCK_BUFFER] = '\0';
              iState = 4;
              break;
            }
          }
          if (iBeg1 == iEnd1) iBeg1 = iEnd1 = 0;   // Reset.
          if (iState == 3) iBuf = 2;               // EOL not found yet.
        } else if ((iBuf == 2) && (iEnd2 != 0)) {  // Use Buffer 2.
          for (; iBeg2 < iEnd2; ++iBeg2) {
            szOutBuf[idx] = szBuf2[iBeg2];  // Copy.
            if ((szOutBuf[idx] == '\n') || (szOutBuf[idx] == '\r')) {
              ++iBeg2;  // Count the char just read
              if ((szOutBuf[idx] == '\r') && (szBuf2[iBeg2] == '\n')) {
                // Using CRLF as end-of-line.
                ++idx;
                szOutBuf[idx] = szBuf2[iBeg2]; // \n
                ++iBeg2;
              }
              szOutBuf[idx + 1] = '\0';  // True. Null line.
              iState = 4;                // Goto cleanup & exit.
              break;                     // Break from for loop.
            }
            ++idx;                             // Advance to next spot.
            if ((idx + 1) == MAX_SOCK_BUFFER)  // Out of room.
            {
              szOutBuf[MAX_SOCK_BUFFER] = '\0';
              iState = 4;
              break;
            }
          }
          if (iBeg2 == iEnd2) iBeg2 = iEnd2 = 0;  // Reset.
          if (iState == 3) iBuf = 1;              // EOL not found yet.
        } else {  // Both buffers empty and still no eol.
          if (idx < MAX_SOCK_BUFFER) {
            iState = 2;  // Still room. Refill the buffers.
          } else {
            iState = 4;  // Out of room. Return.
          }
        }
        break;
      }
      case 4:  // Cleanup and exit.
      {
        iState = 0;
        break;
      }
    }  // End of switch statement.
  }    // End of while loop.

  if (iToast > 0)  // Remove the telnet end of line before returning.
  {
    while ((szOutBuf[idx] == '\r') || (szOutBuf[idx] == '\n')) {
      szOutBuf[idx] = '\0';
      --idx;
      if (idx < 0) break;
    }
  }

  if (iErr == -1) return iErr;

  return (idx + 1);
}

int Socket::Send(char *szBuf, int iLen)  // Send the buffer on this socket
{
  return send(iSock, szBuf, iLen, 0);
}

int Socket::Send(char *szBuf)  // Send the text buffer on this socket
{
  return send(iSock, szBuf, strlen(szBuf), 0);
}

int Socket::Send(const char *szBuf)  // Send the text buffer on this socket
{
  return send(iSock, (char *)szBuf, strlen(szBuf), 0);
}

int Socket::SendText(
    char *szFileName) {  // Send this text file across the socket
  ifstream ifIn;
  char *szBuf;

  ifIn.open(szFileName);
  if (!ifIn) {
    return -1;
  }

  szBuf = new char[BUFSIZE];
  iErr = 0;

  do {
    bzero(szBuf, BUFSIZE);
    ifIn.getline(szBuf, BUFSIZE, '\n');
    iErr += send(iSock, szBuf, strlen(szBuf), 0);
    if (ifIn.eof()) break;
    iErr += send(iSock, CRLF, strlen(CRLF), 0);
  } while (!ifIn.eof());

  ifIn.close();
  delete[] szBuf;
  return iErr;
}

int Socket::SendBinary(char *szFileName) {  // Send this binary file across the socket
  ifstream ifIn;
  char *szBuf;

  ifIn.open(szFileName, ios::binary);
  if (!ifIn){
    return -1;
  }
  szBuf = new char[BUFSIZE];
  
  while (!ifIn.eof()){
    ifIn.read(szBuf, BUFSIZE);
    iErr = send(iSock, szBuf, ifIn.gcount(), 0);
  }

  ifIn.close();
  delete [] szBuf;
  return iErr;
}

int Socket::ResolveName()  // Look up the ip address and name of the peer
{
  struct hostent *hePeer;

  if (szPeerIp == NULL)  // Only if we don't have it already.
  {
    szPeerIp = new char[128];
    strncpy(szPeerIp, inet_ntoa(siThem.sin_addr), 128); // inet_ntoa return static area.
  }
  szPeerName = new char[128];
  hePeer = gethostbyaddr((char *)&(siThem.sin_addr), sizeof(struct in_addr),
                         AF_INET);
  if (hePeer != NULL)  // We found the ip name.
  {
    strncpy(szPeerName, hePeer->h_name, 128);
    iErr = 0;  // Good return.
  } else       // No name available for this host.
  {
    strncpy(szPeerName, szPeerIp, 128);
    iErr = -1;  // Bad return.
  }

  return iErr;
}

int Socket::Close()  // Close this socket
{
  iBeg1 = iEnd1 = iBeg2 = iEnd2 = 0;
  iBuf = 1;
  memset(szOutBuf, 0, MAX_SOCK_BUFFER);
  memset(szBuf1, 0, (MAX_SOCK_BUFFER) / 2);
  memset(szBuf2, 0, (MAX_SOCK_BUFFER) / 2);
  if (szPeerIp) delete[] szPeerIp;
  if (szPeerName) delete[] szPeerName;
  szPeerIp = NULL;
  szPeerName = NULL;
  ulTimeout = 5 * 60;  // 5 minutes default.
  iErr = close(iSock);
  iSock = -1;
  return iErr;
};

void Socket::debug(){
  puterr("szOutbuf", szOutBuf);
}