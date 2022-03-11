#include "socket.hpp"
#include "ftutil.hpp"

#define PORT 8080
#define STACKSIZE 50000

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

bool bDnsLookup;

/* proto type function */
static void set_signal_handler(int signame);
static void signal_handler(int signame);

static void set_signal_handler(int signame) {
  if (SIG_ERR == signal(signame, signal_handler)) {
    fprintf(stderr, "Error. Cannot set signal handler.\n");
    exit(1);
  }
}

static void signal_handler(int signame) {
  printf("Signal(%d) : simple-server stopped.\n", signame);
  exit(0);
}


// dummy
int DoHttp11(Socket *sClient, char *szMethod, char *szUri)
{
  return 0;
}

// void _Optlink W3Conn(void *client)
void *W3Conn(void *client) {
  printf("simple-server started.\n");

  Socket *sClient;
  char *szRequest, *szUri, *szVer;
  int iRc;

  sClient = (Socket *)client;  // Get the pointer to the socket

  // Resolve the IP Name if requested.
  if (bDnsLookup == true) {
    sClient->ResolveName();
  }

  szRequest = new char[SMALLBUF];
  szUri = new char[SMALLBUF];
  szVer = new char[SMALLBUF];

  iRc = sClient->RecvTeol(NO_EOL);  // Get the message

//  sClient->debug();

  // Parse the components of the request
  // "GET / HTTP/1.1"
  sscanf(sClient->szOutBuf, "%s %s %s", szRequest, szUri, szVer);

  puterr("szRequest", szRequest);
  puterr("szUri", szUri);
  puterr("szVer", szVer);

  if (ft::stricmp(szVer, "http/1.0") == 0) {
    //      DoHttp10(sClient, szRequest, szUri);
  } else if (ft::stricmp(szVer, "http/1.1") == 0) {
    iRc = DoHttp11(sClient, szRequest, szUri);
    while (iRc == true)  // Do persistent connections.
    {
      sClient->RecvTeol(NO_EOL);
      sscanf(sClient->szOutBuf, "%s %s %s", szRequest, szUri, szVer);
      iRc = DoHttp11(sClient, szRequest, szUri);
    }
  } else  // Treat this request as a HTTP/0.9 request.
  {
    //      DoHttp09(sClient, szRequest, szUri);
  }
  delete[] szRequest;
  delete[] szUri;
  delete[] szVer;
  delete sClient;
  return NULL;
}

void Server() {
  Socket server, *client;
  pthread_t thread;

  // socket
  server.Create();
  // bind listen
  server.Passive(PORT);
  while (1) {
    // accept
    client = server.Accept();
    if (client == NULL) continue;

    pthread_create(&thread, NULL, &W3Conn, client);
    pthread_join(thread, NULL);
  }
}

int main(void) {
  // signal
  set_signal_handler(SIGINT);

  Server();

  // while
  // accept
  // parse request
  // set response
  // send
  // close
}
