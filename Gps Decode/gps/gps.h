#ifndef GPS_H_
#define GPS_H_

#include <iostream>
#include <fstream>
#include <memory>
#include "string.h"
#include <string>
class gps
    {
     public:
	  gps(char* portName);
      ~gps();
	  double getLatitude();
	  double getLongitude();
	  float getSpeed();
	  float getTrack();
	  float getTime();
	  void getMode();
	  void readFile(const char*file);
	  void readPort();
 	private:
		class impl;
		std::unique_ptr<impl> m_impl;
};

#endif /* GPS_H_ */
