#include "ESP32Time.h"
#include "time.h"
#include <sys/time.h>


ESP32Time::ESP32Time(){}


ESP32Time::ESP32Time(unsigned long offset){
	this->offset = offset;
}


void ESP32Time::setTime(int sc, int mn, int hr, int dy, int mt, int yr, int ms) {
  // seconds, minute, hour, day, month, year $ microseconds(optional)
  // ie setTime(20, 34, 8, 1, 4, 2021) = 8:34:20 1/4/2021
  struct tm t = {0};        // Initalize to all 0's
  t.tm_year = yr - 1900;    // This is year-1900, so 121 = 2021
  t.tm_mon = mt - 1;
  t.tm_mday = dy;
  t.tm_hour = hr;
  t.tm_min = mn;
  t.tm_sec = sc;
  time_t timeSinceEpoch = mktime(&t);
  setTime(timeSinceEpoch, ms);
}


void ESP32Time::setTimeStruct(tm t) {
	time_t timeSinceEpoch = mktime(&t);
	setTime(timeSinceEpoch, 0);
}


void ESP32Time::setTime(unsigned long epoch, int ms) {
  struct timeval tv;
  if (epoch > 2082758399){
	  this->overflow = true;
	  tv.tv_sec = epoch - 2082758399;  // epoch time (seconds)
  } else {
	  tv.tv_sec = epoch;  // epoch time (seconds)
  }
  tv.tv_usec = ms;    // microseconds
  settimeofday(&tv, NULL);
}


tm ESP32Time::getTimeStruct(){
  struct tm timeinfo;
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
  time_t tt = mktime (&timeinfo);

  if (this->overflow){
	  tt += 63071999;
  }
  tt += offset;
  struct tm * tn = localtime(&tt);
  if (this->overflow){
	  tn->tm_year += 64;
  }
  return *tn;
}


String ESP32Time::getDateTime(bool mode){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	if (mode)
	{
		strftime(s, 50, "%A, %B %d %Y %H:%M:%S", &timeinfo);
	}
	else
	{
		strftime(s, 50, "%a, %b %d %Y %H:%M:%S", &timeinfo);
	}
	return String(s);
}


String ESP32Time::getTimeDate(bool mode){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	if (mode)
	{
		strftime(s, 50, "%H:%M:%S %A, %B %d %Y", &timeinfo);
	}
	else
	{
		strftime(s, 50, "%H:%M:%S %a, %b %d %Y", &timeinfo);
	}
	return String(s);
}


String ESP32Time::getTime(){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	strftime(s, 50, "%H:%M:%S", &timeinfo);
	return String(s);
}

String ESP32Time::getTime(String format){
	struct tm timeinfo = getTimeStruct();
	char s[128];
	char c[128];
	format.toCharArray(c, 127);
	strftime(s, 127, c, &timeinfo);
	return String(s);
}

String ESP32Time::getDate(bool mode){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	if (mode)
	{
		strftime(s, 50, "%A, %B %d %Y", &timeinfo);
	}
	else
	{
		strftime(s, 50, "%a, %b %d %Y", &timeinfo);
	}
	return String(s);
}


unsigned long ESP32Time::getMillis(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec/1000;
}


unsigned long ESP32Time::getMicros(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec;
}


unsigned long ESP32Time::getEpoch(){
	struct tm timeinfo = getTimeStruct();
	return mktime(&timeinfo);
}


unsigned long ESP32Time::getLocalEpoch(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long epoch = tv.tv_sec;
	if (this->overflow){
		epoch += 63071999 + 2019686400;
	}
	return epoch;
}


int ESP32Time::getSecond(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_sec;
}


int ESP32Time::getMinute(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_min;
}


int ESP32Time::getHour(bool mode){
	struct tm timeinfo = getTimeStruct();
	if (mode)
	{
		return timeinfo.tm_hour;
	}
	else
	{
		int hour = timeinfo.tm_hour;
		if (hour > 12)
		{
			return timeinfo.tm_hour-12;
		}
		else
		{
			return timeinfo.tm_hour;
		}

	}
}


String ESP32Time::getAmPm(bool lowercase){
	struct tm timeinfo = getTimeStruct();
	if (timeinfo.tm_hour >= 12)
	{
		if (lowercase)
		{
			return "pm";
		}
		else
		{
			return "PM";
		}
	}
	else
	{
		if (lowercase)
		{
			return "am";
		}
		else
		{
			return "AM";
		}
	}
}


int ESP32Time::getDay(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_mday;
}


int ESP32Time::getDayofWeek(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_wday;
}


int ESP32Time::getDayofYear(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_yday;
}

int ESP32Time::getMonth(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_mon;
}

int ESP32Time::getYear(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_year+1900;
}
