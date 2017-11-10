#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include "global.h"

////////////////////////////////////////////////////////////////////////////////
/*
*函数名:init_serial
*功能描述:初始化串口设备
*输入参数:
	serial_fd:已经打开的串口文件描述符
*输出参数:
*返回值:
*创建日期:2017-11-9
*修改日期:
*作者:
*附加说明:
*设计修改日志:
*/
///////////////////////////////////////////////////////////////////////////////

void init_serial(int serial_fd){
	//串口主要设置结构体termios <termios.h>  
	struct termios options;
	/*
		1. tcgetattr函数用于获取与终端相关的参数。
		*参数fd为终端的文件描述符，返回的结果保存在termios结构体中
	*/
	tcgetattr(serial_fd, &options);
	/**2. 修改所获得的参数*/
	options.c_cflag |= (CLOCAL | CREAD);//设置控制模式状态，本地连接，接收使能  
	options.c_cflag &= ~CSIZE;//字符长度，设置数据位之前一定要屏掉这个位  
	options.c_cflag &= ~CRTSCTS;//无硬件流控  
	options.c_cflag |= CS8;//8位数据长度  
	options.c_cflag &= ~CSTOPB;//1位停止位  
	options.c_iflag |= IGNPAR;//无奇偶检验位  
	options.c_oflag = 0; //输出模式  
	options.c_lflag = 0; //不激活终端模式  
	cfsetospeed(&options, B115200);//设置波特率  
	/**3. 设置新属性，TCSANOW：所有改变立即生效*/
	tcflush(serial_fd, TCIFLUSH);//溢出数据可以接收，但不读  
	tcsetattr(serial_fd, TCSANOW, &options);
}

////////////////////////////////////////////////////////////////////////////////
/*
*函数名:read_from_serial
*功能描述:从串口读数据，最多读DATA_LEN个数据
*输入参数:
	serial_fd:已经打开的串口文件描述符
*输出参数:
	command_recv:一块缓存区的地址，用于接收串口读取的数据
*返回值:
	int:
	-1:读串口失败。
	>0:读取的数据数量。
*创建日期:2017-11-9
*修改日期:
*作者:姜祥杰
*附加说明:
	从串口读数据，返回读取的数据数目，并将读取的数据写入command_recv
*设计修改日志:
*/
///////////////////////////////////////////////////////////////////////////////
int read_from_serial(int serial_fd,char *command_recv){
	fd_set r_set;
	int read_num = 0;
	
	while(1){
		FD_ZERO(&r_set);
		FD_SET(fd,&r_set);
		select(fd+1,&r_set,NULL,NULL,NULL);

		if(FD_ISSET(fd,&r_set)){//表示有数据可读
			usleep(10000);//监听到串口有数据，等待10毫秒左右，数据稳定下来，再读取
			read_num = read(serial_fd,command_recv,DATA_LEN);
			if(read_num == 0||read_num == -1){//有数据可读,但是读取失败
				perror("");
				printf("file is = %s,function is = %s,line is %d\n");
				return -1;
			}
			return read_num;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/*
*函数名:write_to_serial
*功能描述:向串口写数据
*输入参数:
	serial_fd:已经打开的串口文件描述符
	send_len:需要往串口写入的数据大小
*输出参数:
	command_recv:一块缓存区的地址，用于接收串口读取的数据
*返回值:
	int:
	-1:写串口失败。
	>0:实际写入的数据数量。
*创建日期:2017-11-9
*修改日期:
*作者:姜祥杰
*附加说明:
	将数据写入串口，一直写，直到写完send_len个字节
*设计修改日志:
*/
///////////////////////////////////////////////////////////////////////////////
int write_to_serial(int serial_fd,char *command_send,int send_len){
	int write_num = 0;//实际写入的量
	int remain_num = 0;//写一次剩余没写的量
	int current_write_position = 0;//写数据已经写到了地址
	int fd = serial_fd;

	write_num = write(fd,command_send,send_len);
	if(write_num == -1){
		perror("");
		printf("file is = %s,function is = %s,line is %d\n");
		return -1;
	}
	remain_num = send_len - write_num;
	while(remain_num){
		current_write_position = current_write_position + write_num;
		write_num = remain_num;
		write_num = write(fd,&command_send[current_write_position],write_num);
		if(write_num == -1){
			perror("");
			printf("file is = %s,function is = %s,line is %d\n");
			return -1;
		}
		remain_num = remain_num - write_num;
	}
	return send_len;
}

