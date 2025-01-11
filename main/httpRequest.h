#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

extern char tempS[7];
extern char humidS[7];
extern int year; 
extern int month;
extern int day;
extern int hour;
extern int minute;
extern int second;

void openweather_api_http(void);
void worldTime_api_http(void);

#endif // HTTPREQUEST_H