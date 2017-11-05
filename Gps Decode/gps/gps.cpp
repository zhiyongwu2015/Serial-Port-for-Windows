#include "gps.h"
#include "SerialPort.h"
#include <fstream>
#include <cstdio>
#include <cstdlib>

#ifdef DEBUG
#define DEBUG 1
#else 
#define DEBUG 0
#endif

#define MAX_LONGITUDE 180
#define MAX_LATITUDE 90
char* Pre_comingData= NULL;//存贮读取的数据
void Reverse(char *pBegin, char *pEnd);//传出的数据会出现 ‘\n’的情况，用于翻转读取的数据格式

using namespace std;
class gps::impl{
 public:
		  impl( char* portName);//输入串口
		  ~impl();
		  double getLatitude();
		  double getLongitude();
		  float getSpeed();
		  float getTrack();
		  float getTime();

		  std::string londir;//东西经
		  std::string latdir;//南北纬
		  std::string Mode;//接受模式
		  float check;//协议校验位

		  char* Gps_pos = NULL;

		  void readFile(const char*file);
		  void readPort();
private:
	double latitude;//纬度
	double longitude;//经度
	
	double speed;//Speed over ground  cm/s
	float track_true;//Track made good,degree true
	float gps_time;//测定位置的时间 hh.mm.ss
	

	char* lat_dir = NULL;//纬度判断
	char* lon_dir = NULL;//经度判断
	char* pos_status = NULL;//数据的有效性
	char* indicator = NULL;//Positioning system mode indicator

	SerialPort* gps_serial;

	void decodeGPRMC(char* line);
	void clearGPRMC();
};
/***** Constructor *******/
gps::impl::impl(char* portName){
	try{
		gps_serial = new SerialPort(portName);
	}
	catch (std::exception &e){
		std::cout << "\n" << e.what() << "\n";
		throw std::runtime_error("[gps] Error communication with device \n");
	}
	if (gps_serial->isConnected())
		std::cout << "Connected to esatblished at port: " << portName << endl;
	 clearGPRMC();
}
/******* Destructor ***********/
gps::impl::~impl(){
	gps_serial->disConnected();
}

/*********** Decode the Data of GPS *****************/
void gps::impl::decodeGPRMC(char *line){
        clearGPRMC();
		int counter= 0;
        char* delimiters=",*";//分割数据标识符
		char* next_token = NULL;
		char*GPS_msg = NULL ;
		GPS_msg = strtok_s(line, delimiters, &next_token);

		while (GPS_msg != NULL){
			switch (counter)
			{
			case 1:
				gps_time = atof(GPS_msg);
				break;
			case 2:
				if (*GPS_msg == 'V'){
					std::cout << "The data is invalid !!!" << std::endl;
					return ;
				}
				break;
			case 3:
				latitude = atof(GPS_msg);//提取纬度
				break;
			case 4:
				if (*GPS_msg == 'N'){
					latdir = "N";
				}
				else{
					latdir = "S";
				}
				break;
			case 5:
				longitude = atof(GPS_msg);//提取经度
				break;
			case 6:
				if (*GPS_msg == 'W'){
					londir = "W";
				}
				else{
					londir = "E";
				}
				break;
			case 7:
				speed = atof(GPS_msg);//行驶速度 一节=51.44 cm/s
				break;
			case 8:
				track_true = atof(GPS_msg);//偏转角度
				break;
			case 12:
				if (*GPS_msg == 'A'){
					Mode = "Autonomous";
				}
				else if(*GPS_msg=='D'){
					Mode = "Differential";
				}
				else if (*GPS_msg == 'E'){
					Mode = "Estimated";
				}
				else if (*GPS_msg == 'M'){
					Mode = "Manual input";
				}
				else{
					Mode = "Invalid";
				}
				break;
			case 13:
				check = atof(GPS_msg);
				break;
			default:
				break;
			}
			GPS_msg = strtok_s(NULL, delimiters, &next_token);
			counter++;
		}
}

/********** Clear GPS Date *************/
void gps::impl::clearGPRMC(){
    gps_time=0.0;
    longitude=0.0;
    latitude=0.0;
    speed=0.0;
    track_true=0.0;
	lat_dir = NULL;//纬度判断
	lon_dir = NULL;//经度判断
	pos_status = NULL;//数据的有效性
	indicator = NULL;
}
/****** Read From the SerialPort **********/
void gps::impl::readPort(){
	char comingData[80] = {0};//存贮读取的数据
	gps_serial->readSerialPort(comingData,79);//数据存入cmd中
	//printf("%s\n", comingData);//打印读取数据
	std::string cmd = comingData;
	if (cmd.find("$GPRMC") != std::string::npos&&cmd.find("\n") == std::string::npos){
		//decodeGPRMC((char*)cmd.c_str());
		//Gps_pos = strstr(comingData, "$GPRMC");
		//Gps_pos = Gps_pos + 3;
		//Gps_pos = strstr(comingData, "$GPRMC");
		//Gps_pos = Gps_pos + 3;
		Pre_comingData = comingData;//存贮读取的数据
		Gps_pos = strstr(comingData, "$GPRMC");//返回字串中首次出现"$GPRMC"的地址
		decodeGPRMC(Gps_pos);
	}
	else if (cmd.find("\n") != std::string::npos)//读取的数据中存在回车键的情况
	{
		char *NewData = NULL;
		std::string str1;
		std::string str2;
		int counter = 0;
		char* delimiters = "\n";//分割数据标识符
		char* next_token = NULL;
		char*Message = NULL;
		Message = strtok_s(comingData, delimiters, &next_token);
		while (Message!= NULL){
			switch (counter)
			{
			case 0:
				str1 = Message;
				break;
			case 1:
				str2= Message;
				break;
			default:
				break;
			}
			Message = strtok_s(NULL, delimiters, &next_token);
			counter++;
		}
		std::string str = str2 + str1;
		strncpy_s(comingData, str.c_str(), str.length());
		Gps_pos = strstr(NewData, "$GPRMC");//返回字串中首次出现"$GPRMC"的地址
	    decodeGPRMC(Gps_pos);
	}
}

/******* Read file from file *********/
 void gps::impl::readFile(const char* file){
	char line[255];
	ifstream infile;
	infile.open(file);
	while (!infile.eof()){
		infile.getline(line, 255);

	}
	decodeGPRMC(line);
	infile.close();
}

 /******* The parameter of GPS***************/
double gps::impl::getLatitude() {
	if (latitude<0.0 && latitude>MAX_LATITUDE)
	{
		return 0;
	}
	latitude = (int)latitude / 100+(latitude/100-(int)latitude/100)*100/60;
    return latitude;
}

double gps::impl::getLongitude() {
		if (longitude<0.0 && longitude>MAX_LONGITUDE)
		{
			return 0;
		}
		longitude = (int)longitude / 100 + (longitude / 100 - (int)longitude / 100) * 100 / 60;
  return longitude;
}

float gps::impl::getSpeed() {

	return speed*51.44;//行驶速度 一节=51.44 cm/s;
}

float gps::impl::getTrack() {
    return track_true;
}

float gps::impl::getTime() {
    return gps_time;
}

/**** GPS Public API ************/
gps::gps(char* portName) :m_impl(new gps::impl(portName)){

}

gps::~gps(){

}

void gps::readPort(){
	return m_impl->readPort();
}
void gps::readFile(const char*file){
	return m_impl->readFile(file);
}

float gps::getSpeed() {
	return m_impl->getSpeed();
}

float gps::getTrack() {
	return m_impl->getTrack();
}

float gps::getTime() {
	return m_impl->getTime();
}

double gps::getLatitude() {
	std::cout << m_impl->latdir<<"  ";
	return m_impl->getLatitude();
}
double gps::getLongitude() {
	std::cout << m_impl->londir<<"  ";
	return m_impl->getLongitude();
}
void gps::getMode(){
	std::cout << "Mode Indicator: " << m_impl->Mode<<std::endl;
	std::cout << "Check: " << m_impl->check << std::endl;
}

//暂时不用
void Reverse(char *pBegin, char *pEnd)
{
	if (pBegin == NULL || pEnd)
		return;
	while (pBegin<pEnd)
	{
		char temp = *pBegin;
		*pBegin = *pEnd;
		*pEnd = temp;

		pEnd--; 
		pBegin++;
	}
}