#include "util.hpp"

int main(int argc, char *argv[]) {
  int s, rc;
  char szBuf[256];
  struct sockaddr_in server {};

  if (argc != 2) {
    cerr << "argc" << endl;
    return 1;
  }
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    cerr << ERRSOCK << endl;
    return 1;
  }

  bzero(&server, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = inet_addr(argv[1]);

  rc = connect(s, (struct sockaddr *)&server, sizeof(server));
  if (rc < 0) {
    cerr << "connect" << endl;
    return 1;
  }

  strcpy(szBuf, "HELLO\r\n");
  rc = send(s, szBuf, strlen(szBuf), 0);
  if (rc < 0) return 1;
  rc = RecvLine(s, szBuf, 256);
  if (rc < 0) return 1;

  strcpy(szBuf, "GOODBYE\r\n");
  rc = send(s, szBuf, strlen(szBuf), 0);
  if (rc < 0) return 1;
  rc = RecvLine(s, szBuf, 256);
  cout << "RecvLine:" << szBuf << endl;
  if (rc < 0) return 1;

  close(s);
  return 0;
}
