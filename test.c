////
//// Created by Zening Chen on 2/3/22.
////
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()  {
//    for (int i = 0; i < 10; i++) {
//        printf("hello!\n");
//        sleep(1);
//    }
    sleep(10);
    int fd =  open("temp.txt", O_CREAT | O_RDWR, 0644);
    write(fd, "finish!", 7);
}
