#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "global.h"

char *grade_info = NULL;

////////////////////////////////////////////////////////////////////////////////
/*
*函数名:LOG
*功能描述:将日志与时间组合，写入到日志文件中
*输入参数:
	ms:输入参数的格式
	...:按照参数格式输入的参数(可变参数)
*输出参数:
*返回值:
*创建日期:2017-11-10
*修改日期:
*作者:
*附加说明:
*设计修改日志:
*/
///////////////////////////////////////////////////////////////////////////////
void LOG(const char* ms, ... )  
{  
    char wzLog[1024] = {0};  
    char buffer[1024] = {0};  
    va_list args;  
    va_start(args, ms);  
    vsprintf( wzLog ,ms,args);  
    va_end(args);  

    time_t now;  
    time(&now);  
    struct tm *local;  
    local = localtime(&now);  
    printf("%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon+1,  
            local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,  
            wzLog);  
    sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon+1,  
                local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,  
                wzLog);  
    FILE* file = fopen(LOG_FILE,"a+");  
    fwrite(buffer,1,strlen(buffer),file);  
    fclose(file);  
  
    return ;  
}  

////////////////////////////////////////////////////////////////////////////////
/*
*函数名:tpulog
*功能描述:
	根据输入的日志调试模式，来组成不同的字符串，传递给LOG
*输入参数:
	log_grade:日志状态(接收、发送、错误或者调试)
	log_info:想要写入的日志信息
	log_info_len:想要写入的日志信息长度
*输出参数:
*返回值:
*创建日期:2017-11-10
*修改日期:
*作者:姜祥杰
*附加说明:
*设计修改日志:
*/
///////////////////////////////////////////////////////////////////////////////
void tpulog(int log_grade,char *log_info,int log_info_len){
	int i = 0;
	int position = 0;
	char *buf = (char *)malloc(4096);
	switch(log_grade){
		case LOG_RECV:
			grade_info = "recv ";
			for(i=0;i<log_info_len;i++){
				sprintf(&buf[position],"%02x",log_info[i]);
				buf[position+2] = ',';
				position+=3;
			}
			LOG("%s %s",grade_info,buf);
			break;
		case LOG_SEND:
			grade_info = "send ";
			for(i=0;i<log_info_len;i++){
				sprintf(&buf[position],"%02x ",log_info[i]);
				buf[position+2] = ',';
				position+=3;
			}
			LOG("%s %s",grade_info,buf);
			break;
		case LOG_ERROR:
			grade_info = "error";
			LOG("%s %s %s %d",grade_info,__FILE__,__FUNCTION__,__LINE__);
			break;
		default:
			break;
	}
}
