#include "util.hpp"
#include "socket.hpp"

int main(void) {
  int s, rc, c;
  struct sockaddr_in server{}, client{};
	socklen_t len;

  // socket作成
  s = socket(AF_INET, SOCK_STREAM, 0);   // AF_INETとSOCK_STREAMを組み合わせるとTCPソケット
  if (s < 0){
    cerr << ERRSOCK << endl;
    return 1;
  }
  len = sizeof(struct sockaddr_in);

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;

  rc = bind(s, (struct sockaddr *) &server, len);
  if (rc < 0){
    cerr << ERRBIND << endl;
    return 1;
  }
  
  rc = listen(s, WAIT_QUEUE_LEN); // 大体5-10 SOMAXCONN:1024
  if (rc < 0){
    cerr << ERRLISN << endl;
    return 1;
  }
  
  while (1){
    bzero(&client, len);
    c = accept(s, (struct sockaddr *)&client, &len);
    if (c < 0){
      cerr << ERRACPT << endl;
      return 1;
    }
    // 新しいソケットcを使ってクライアントとの間の処理を行う
    TalkToClient(c);
//    close(c);
  }
}
