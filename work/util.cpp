#include "util.hpp"

int RecvLine(int iSocket, char *szBuf, int iLen) {
  int iBytesRead, iIdx, bNotDone;

  iBytesRead = recv(iSocket, &szBuf[0], 1, 0);
  iIdx = 1;
  bNotDone = true;

  while (bNotDone == true) {
    iBytesRead = recv(iSocket, &szBuf[iIdx], 1, 0);
    if (iBytesRead < 0) return -1;
    ++iIdx;
    if ((szBuf[iIdx - 2] == '\r') && (szBuf[iIdx - 1] == '\n')) {
      bNotDone = false;
    }
    if (iIdx == iLen) return -1;
  }
  szBuf[iIdx - 2] = '\0';
  return true;
}

void TalkToClient(int iSocket){
  int iRc, bNotDone;
  char szBuf[256] ;
  char szOk[] = "OK" ;
  char szErr[] = "ERR" ;
  
  bNotDone = true;
  while (bNotDone == true){
    iRc = RecvLine(iSocket, szBuf, 256);
    if (iRc < 0){
      cerr << "RecvLine" << endl;
      bNotDone = false;
    }
    if (strcmp(szBuf, "HELLO") == 0){
      iRc = send(iSocket, szOk, strlen(szOk), 0);
      if (iRc < 0){
        cerr << "send" << endl;
        bNotDone = false;
      }
    }
    else if (strcmp(szBuf, "GOODBYE") == 0){
      iRc = send(iSocket, szOk, strlen(szOk), 0);
      if (iRc < 0){
        cerr << "send" << endl;
      }
      bNotDone = false;
    }
    else{
      iRc = send(iSocket, szErr, strlen(szErr), 0);
      bNotDone = false;
    }
  }
  close(iSocket);
}
