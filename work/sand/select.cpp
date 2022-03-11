#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int
main(void)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* stdin (fd 0) を監視し、入力があった場合に表示する。*/
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* 5 秒間監視する。*/
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);
    /* この時点での tv の値を信頼してはならない。*/


    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("今、データが取得できました。\n");
        /* FD_ISSET(0, &rfds) が true になる。*/
    else
        printf("5 秒以内にデータが入力されませんでした。\n");

    exit(EXIT_SUCCESS);
}