#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open_update_file(char *file_name){
	int file_start_position = 0;
	int file_end_position   = 0;
	int file_len = 0;
	FILE *fp = fopen(file_name,"r+");
	if(fp == NULL){
		perror("");
		printf("file is %s,funciton is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		return -1;//打开升级文件失败
	}
	if(-1 == fseek(fp,0,SEEK_END)){
		perror("");
		printf("file is %s,funciton is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		fclose(fp);
		return -2;//文件位移操作失败
	}
	file_end_position	= ftell(fp);

	if(-1 == fseek(fp,0,SEEK_SET)){
		perror("");
		printf("file is %s,funciton is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		fclose(fp);
		return -2;//文件位移失败
	}
	file_start_position = ftell(fp);
/*
	if((file_len = file_end_position - file_start_position) <= 0){
		fclose(fp);
		return -3;//打开的文件是一个空文件
	}
*/
	int fd = fileno(fp);//将FILE* 型式的文件描述符指针转变成int型的文件描述符
	return fd;
}

char * read_file_to_cache(int fd,int *file_len){
	int read_num = 0;
	int remain_num = 0;
	int file_size = 0;
	int current_read_position = 0;
	struct stat file_stat;
	memset(&file_stat,'\0',sizeof(file_stat));

	if(fstat(fd,&file_stat) == -1){
		perror("");
		printf("file is %s,funciton is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		return (void *)-1;
	}
	file_size = file_stat.st_size;
	char *data_read = (char *)malloc(file_size+1);
	if(data_read == NULL){
		printf("malloc error\n");
		return (void *)-1;
	}
	fseek(fdopen(fd,"r+"),0,SEEK_SET);
	read_num = read(fd,data_read,file_size);
	if(read_num == -1){
		perror("");
		printf("file is %s,funciton is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		return (void *)-1;
	}
	remain_num = file_size - read_num;
	while(remain_num){
		current_read_position = current_read_position + read_num;
		read_num = remain_num;
		read_num = read(fd,data_read+current_read_position,read_num);
		if(read_num == -1){
			perror("");
			printf("file is %s,funciton is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
			return (void *)-1;
		}
		remain_num = remain_num - read_num;
	}
	memcpy(file_len,&file_size,sizeof(file_size));
	return data_read;
}

int write_to_file(int fd,char *command_send,int send_len){
	int write_num = 0;//实际写入的量
	int remain_num = 0;//写一次剩余没写的量
	int current_write_position = 0;//写数据已经写到了地址

	write_num = write(fd,command_send,send_len);
	if(write_num == -1){
		perror("");
		printf("file is = %s,function is = %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		return -1; 
	}   
	remain_num = send_len - write_num;
	while(remain_num){
		current_write_position = current_write_position + write_num;
		write_num = remain_num;
		write_num = write(fd,&command_send[current_write_position],write_num);
		if(write_num == -1){
			perror("");
			printf("file is = %s,function is = %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
			return -1; 
		}
		remain_num = remain_num - write_num;
	}   
	return send_len;
}

int main(){
	int send_len = 0;
	int read_len = 0;
	char *file_path = "/home/jxj/test/aa1";
	//打开要升级的文件
	int fd = open_update_file(file_path);
	if(-1 == fd||-2 == fd){
		printf("open file error\n");
		return -1;
	}
	//写想要写的信息到文件
/*
	char *s = "jiangxiangjie";
	send_len = write_to_file(fd,s,13);
*/
	//读文件到缓存区
	char *test = read_file_to_cache(fd,&read_len);
	//printf("%s\n",test);
	//测试，读取文件的缓存写入另外一个文件中
	int fd_tmp = open("/home/jxj/test/a2",O_RDWR|O_CREAT|O_TRUNC,0655);
	write_to_file(fd_tmp,test,read_len);
	close(fd_tmp);
	close(fd);
	return 0;
}

