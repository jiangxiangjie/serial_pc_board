#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "global.h"

#define PACK_TMP_LEN	4096
#define STX	0x02
#define ETX	0x03
#define DLE	0x10

////////////////////////////////////////////////////////////////////////////////
/*
*函数名:pack
*功能描述:打包要发送的数据，开头添加STX,结尾添加ETX.中间需要转义的添加转义字符
*输入参数:
	string:需要打包的字符串。
	string_len:需要打包的字符串长度。
*输出参数:
	pack_end_string:打包完成后的字符串的内存地址(接收的内存需要提前申请好空间)。
*返回值:
	int:
	-1:申请内存失败，组包失败。
	>0:组完包之后的字符串大小。
*创建日期:2017-11-9
*修改日期:
*作者:姜祥杰
*附加说明:
*设计修改日志:
*/
///////////////////////////////////////////////////////////////////////////////
int pack(char *string,int string_len,char *pack_end_string){
	int i = 0;
	char *pack_string = (char *)malloc(PACK_TMP_LEN);
	char *pack_string_tmp = (char *)malloc(PACK_TMP_LEN);

	if(pack_string == NULL||pack_string_tmp==NULL){
		perror("");
		printf("file is %s,function is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}
	memcpy(pack_string,string,string_len);
	for(i=0;i<string_len;i++){
		if((pack_string[i] == STX)||(pack_string[i] == ETX)||(pack_string[i]==DLE)){
			//memcpy(&pack_string[i+1],&pack_string[i],string_len-i);//不能同一个数组相互移动
			memcpy(pack_string_tmp,&pack_string[i],string_len-i);
			memcpy(&pack_string[i+1],pack_string_tmp,string_len-i);
			pack_string[i] = DLE;
			string_len++;
			i++;
		}
	}
	//memcpy(&pack_string[1],&pack_string[0],string_len);
	memcpy(pack_string_tmp,&pack_string[0],string_len);
	memcpy(&pack_string[1],pack_string_tmp,string_len);
	pack_string[0] = STX;string_len++;
	pack_string[string_len] = ETX;string_len++;
	pack_string[string_len+1] = '\0';
	memcpy(pack_end_string,pack_string,string_len);
	free(pack_string);
	free(pack_string_tmp);
	return string_len;
}

////////////////////////////////////////////////////////////////////////////////
/*
*函数名:unpack
*功能描述:解包接收到的包数据，去掉开头的STX结尾的ETX，以及转义字符DLE。
*输入参数:
	string:需要解包的字符串数据。
	string_len:需要解包的字符串长度。
*输出参数:
	unpack_end_string:解包完成后的字符串的内存地址(接收的内存需要提前申请好空间)。
*返回值:
	int:
	-1:申请内存失败，组包失败。
	-2:要解的包不是一个完整的包数据(接收的包数据不完整)。
	>0:解完包之后的字符串大小。
*创建日期:2017-11-9
*修改日期:
*作者:姜祥杰
*附加说明:
*设计修改日志:
*/
///////////////////////////////////////////////////////////////////////////////
int unpack(char *string,int string_len,char *unpack_end_string){
	int i = 0;
	char *unpack_string = (char *)malloc(PACK_TMP_LEN);
	char *unpack_string_tmp = (char *)malloc(PACK_TMP_LEN);
	if(unpack_string == NULL||unpack_string_tmp==NULL){
		perror("");
		printf("file is %s,function is %s,line is %d\n",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}
	memcpy(unpack_string,string,string_len);
	if(unpack_string[0]!=STX||unpack_string[string_len-1]!=ETX){
		return -2;//要解的包不是一个完整的包(接收包文件的时候出错)
	}
	for(i=0;i<string_len;i++){
		if(((unpack_string[i] == DLE) && (unpack_string[i+1] == STX))||\
			((unpack_string[i] == DLE) && (unpack_string[i+1] == ETX))||\
			((unpack_string[i] == DLE) && (unpack_string[i+1] == DLE))){
				memcpy(unpack_string_tmp,&unpack_string[i+1],string_len-i-1);
				memcpy(&unpack_string[i],unpack_string_tmp,string_len-i-1);
				string_len--;
		}
	}

	memcpy(unpack_string_tmp,&unpack_string[1],string_len-1);
	memcpy(&unpack_string[0],unpack_string_tmp,string_len-1);string_len--;
	unpack_string[string_len-1] = '\0';string_len--;//去掉ETX
	memcpy(unpack_end_string,unpack_string,string_len);
	free(unpack_string);
	free(unpack_string_tmp);
	return string_len;
}

/*
int main(){

	//打包程序
	int i = 0;
	char string[10] = {0x0,0x1,0x2,0x02,0x10,0x10,0x10,0x02,0x03};
	//char string[100] = {0x0,0x1,0x2,0x02,0x10,0x10,0x10,0x02,0x03,0x02,0x02,0x02,0x02,0x02,0x02,0x10,0x10,0x10,0x10,0x10};
	printf("init data = ");
	for(i=0;i<sizeof(string);i++)
		printf("%x ",string[i]);
	printf("\r\n");
	char *pack_end_string = (char *)malloc(4096);
	int len = pack(string,sizeof(string),pack_end_string);
	printf("packed data = ");
	for(i=0;i<len;i++)
		printf("%x ",pack_end_string[i]);
	printf("\r\n");

	//解包程序
	char *unpack_end_string = (char *)malloc(4096);
	len = unpack(pack_end_string,len,unpack_end_string);
	printf("unpacked data = ");
	for(i=0;i<len;i++)
		printf("%x ",unpack_end_string[i]);
	printf("\r\n");
	return 0;
}
*/
