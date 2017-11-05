/***
//Author: WuzhiYong
//Time: 2017-11-5
//Project: Decode the GPRMC of GPS in windows.
**/
#include "gps.h"
#include "SerialPort.h"
#include <windows.h>

char* portName = "COM4";

int main()
{
	gps msg(portName);
	while (1){
		msg.readPort();
		/*********/
		msg.getMode();//数据采集方式及校验位
		std::cout << "Time: " << msg.getTime() << std::endl;
		std::cout << "纬度: " << msg.getLatitude() << std::endl;
		std::cout << "经度: " << msg.getLongitude() << std::endl;
		std::cout << "Speed: " << msg.getSpeed() << " cm/s" << std::endl;
		std::cout << "Track: " << msg.getTrack() << std::endl;
		//Sleep(2000);
		std::cout << std::endl;
	}
}