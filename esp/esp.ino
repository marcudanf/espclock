#include "SSD1306.h"

#include <NTPtimeESP.h>

#define threshold 22

SSD1306 display(0x3c, 5, 4);


NTPtime NTPch("ch.pool.ntp.org");

//change those
char *ssid=".......";
char *password=".........";
float timezone=2.0;

byte err=0;
byte lastMin=-1;
bool state=0;
int prev=45;

strDateTime dateTime;

int touch(int pin){
  int out=(50-touchRead(pin));
  if(out>0)  {
    out=(out+2);
  }else{
    out= 0;
  }
  return out;
}

const char Day[7][10]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
const char Month[12][6]={"Jan.","Feb.","March","Apr.","May","June","July","Aug.","Sept.","Oct","Nov","Dec."};

void setup() {
  display.init();
  display.flipScreenVertically();
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin (ssid, password);
  WiFi.softAP("network","12345678",1,13);
  while(WiFi.status()!=WL_CONNECTED){
    delay(1);
  }
  retry:
  dateTime=NTPch.getNTPtime(timezone, 1);
  if(!dateTime.valid)
  goto retry;
  String time;
  if(dateTime.hour<10)
    time+='0';
  time+=String(dateTime.hour)+':';
  if(dateTime.minute<10)
    time+='0';
  time+=String(dateTime.minute);
  display.clear();
  display.setColor(WHITE);
  display.setFont(Crushed_64);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0,time);
  display.display();
  uint64_t started_at=millis();
  for(int i=started_at;i<started_at+1000*(60-dateTime.second);i=millis()){
    delay(100);
    int t=touch(T1);
    if(t>threshold&&prev<threshold){
      state=!state;
      if(!state){
        String time;
        if(dateTime.hour<10)
          time+='0';
        time+=String(dateTime.hour)+':';
        if(dateTime.minute<10)
          time+='0';
        time+=String(dateTime.minute);
        display.clear();
        display.setColor(WHITE);
        display.setFont(Crushed_64);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 0,time);
        display.display();
      }else{
        String date;
        if(dateTime.day<10)
        date+='0';
        date+=dateTime.day;
        date+=Month[dateTime.month-1];
        date+=dateTime.year;
        display.clear();
        display.setColor(WHITE);
        display.setFont(ArialMT_Plain_24);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 5,date);
        display.setFont(ArialMT_Plain_24);
        display.drawString(64, 35,String(Day[dateTime.dayofWeek-1]));
        display.display();
      }
    }
    prev=t;
  }
}

void loop(){
  if(WiFi.status()!=WL_CONNECTED){
    WiFi.begin(ssid,password);
    display.clear();
    display.setColor(WHITE);
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 0,"No WiFiconnection\nretrying...");
    display.display();
  }
  while(WiFi.status()!=WL_CONNECTED)
  delay(1);
  dateTime=NTPch.getNTPtime(timezone, 1);
  if(dateTime.valid==0){
    err++;
    if(err>1500){
      display.clear();
      display.setColor(WHITE);
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 0,"Could not get\ndate and time,\nreset router");
      display.display();
      ESP.deepSleep(1000000*60*24*10);
    }
    return;
  }
  if(dateTime.valid&&dateTime.minute!=lastMin){
    err=0;
    lastMin=dateTime.minute;
    String time;
    if(dateTime.hour<10)
      time+='0';
    time+=String(dateTime.hour)+':';
    if(dateTime.minute<10)
      time+='0';
    time+=String(dateTime.minute);
    display.clear();
    display.setColor(WHITE);
    display.setFont(Crushed_64);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 0,time);
    display.display();
  }
  uint64_t started_at=millis();
  for(int i=started_at;i<started_at+1000*(60-dateTime.second);i=millis()){
    delay(100);
    int t=touch(T1);
    if(t>threshold&&prev<threshold){
      state=!state;
      if(!state){
        String time;
        if(dateTime.hour<10)
          time+='0';
        time+=String(dateTime.hour)+':';
        if(dateTime.minute<10)
          time+='0';
        time+=String(dateTime.minute);
        display.clear();
        display.setColor(WHITE);
        display.setFont(Crushed_64);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 0,time);
        display.display();
      }else{
        String date;
        if(dateTime.day<10)
        date+='0';
        date+=dateTime.day;
        date+=Month[dateTime.month-1];
        date+=dateTime.year;
        display.clear();
        display.setColor(WHITE);
        display.setFont(ArialMT_Plain_24);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 5,date);
        display.setFont(ArialMT_Plain_24);
        display.drawString(64, 35,String(Day[dateTime.dayofWeek-1]));
        display.display();
      }
    }
    prev=t;
  }
}

