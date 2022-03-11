#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>

using namespace std;
using ll = long long;
#define rep(i, n) for (ll i = 0; i < n; ++i)
#define PORT 8080
#define ERRSOCK "Error! Cannot create socket"
#define ERRBIND "Error! Cannot create socket"
#define ERRLISN "Error! Listen failed"
#define ERRACPT "Error! Accept failed"
#define WAIT_QUEUE_LEN 5
#define max(a, b) a > b ? a : b
#define min(a, b) a > b ? b : a

int RecvLine(int iSocket, char *szBuf, int iLen);
void TalkToClient(int iSocket);
