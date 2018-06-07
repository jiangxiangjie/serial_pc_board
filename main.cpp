#include <cstdio>
#include <pthread.h>

pthread_t my_pth;

void *test(void *arg) {
	printf("test\n");
	pthread_exit(NULL);
}

int main()
{
    printf("hello from linux_1!\n");
	pthread_create(&my_pth,NULL,test,NULL);
	pthread_join(my_pth,NULL);
	getchar();
    return 0;
}