#include "usart.h"
#include "cmsis_os.h"

void Wifi_SendData(int bifen1 , int bifen2)
{
	uart_print(&huart2, 
    "AT+MQTTUSERCFG=0,1,\"produce1\",\"jY72U2XzAa\","
    "\"version=2018-10-31&res=products%%2FjY72U2XzAa%%2Fdevices%%2Fproduce1&et=2192672778&method=md5&sign=p4GVGP8646WlEb6da8YFwQ%%3D%%3D\",0,0,\"\"\r\n");
	
	osDelay(500);
	uart_print(&huart2,"AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
	
	osDelay(1500);
	uart_print(&huart2,"AT+MQTTSUB=0,\"$sys/jY72U2XzAa/produce1/thing/property/post/reply\",0\r\n");

	osDelay(500);
	uart_print(&huart2,"AT+MQTTPUB=0,\"$sys/jY72U2XzAa/produce1/thing/property/post\",\"{\\\"id\\\":\\\"2463878805\\\"\\,\\\"params\\\":{\\\"bifen1\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n",bifen1);

	osDelay(500);
	uart_print(&huart2,"AT+MQTTPUB=0,\"$sys/jY72U2XzAa/produce1/thing/property/post\",\"{\\\"id\\\":\\\"2463878805\\\"\\,\\\"params\\\":{\\\"bifen2\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n",bifen2);
}
void Wifi_GetTime(void)
{
	
uart_print(&huart2, "AT+CIPSNTPCFG=1,8,\"%s\"\r\n", "pool.ntp.org");
osDelay(500);
uart_print(&huart2, "AT+CIPSNTPTIME?\r\n");
	
}

