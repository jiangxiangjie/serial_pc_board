#ifndef __GLOBAL_H
#define __GLOBAL_H

#define DATA_LEN	4096
#define STX			0x02
#define ETX			0x03
#define DLE			0x10

unsigned short crc_ccitt(char *string,int string_len);//crc_ccitt校验
int md5(char* fileName,char *md5_data);//md5校验
void init_serial(int serial_fd);//初始化串口
int read_from_serial(int serial_fd,char *command_recv);//从串口读命令或者信息
int write_to_serial(int serial_fd,char *command_send,int send_len);//写串口，一直写完send_len个字节
int pack(char *string,int string_len,char *pack_end_string);//组包函数
int unpack(char *string,int string_len,char *unpack_end_string);//解包函数

//错误码
#define CrcCheckError		 0x31	//crc校验失败
#define Md5CheckError		 0x32	//md5校验失败
#define ReadSerialError		 0x33	//读串口失败
#define WriteSerialError	 0x34	//写串口失败
#define CreateCacheFileError 0x35	//创建缓存文件失败
#define OpenFileError		 0x36	//打开升级程序或文件失败
#define FrameDiscontinuous	 0x37	//帧号不连续
#define RecvFileSizeNotSame	 0x38	//接收文件大小与首帧描述文件大小不一致
#define ReSendTimesOut		 0x39	//重发次数超出3次  
#define PackError			 0x40	//组包失败
#define UnPackError			 0x41	//解包失败
#define RecvPackageError	 0x42	//接收的包不是一个完整的包(缺少开头STX或者缺少结尾ETX)
#define UpdateFileIsEmpty	 0x43	//要升级的程序或者要下载的文件是一个空文件
#define FileFseekError		 0x44	//文件指针位移操作失败
#define ReadFileError		 0x45	//读文件失败
#define WriteFileError		 0x46	//写文件失败
#endif
