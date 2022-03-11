#ifndef DEFINES_HPP
#define DEFINES_HPP

#define STACKSIZE       50000
#define WWW_PORT        80
#define SMALLBUF        1<<12
#define MAX_ALIASES     1<<7
#define MAX_EXTENSIONS  1<<8
#define PATH_LENGTH     1<<9
#define HTTP_1_0        "HTTP/1.0"
#define HTTP_1_1        "HTTP/1.1"
#define MAX_SOCK_BUFFER 1<<14
#define BUFSIZE 1<<14
// #define bzero(x, y) memset(x, 0, y)
#define CRLF "\r\n"
#define HEADER_END "\r\n\r\n"
#define NO_EOL 1
#define REUSE_PORT 1


#endif
