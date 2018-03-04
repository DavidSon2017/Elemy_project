#include <arduino.h>
#include "class_kproject_device.h"
#include "class_kproject_device_segment.h"
#include "class_kproject_device_multi_shift_register.h"
#include <SoftwareSerial.h>
#define DEBUG true

SoftwareSerial esp8266(5,6); //tx, rx(hardware rx, tx reverse)

#define latch_pin    12
#define clock_pin    13
#define data_pin     11

char buf[12];           //receive data from server
String data;            //spend data from arduino
int cnt=0, current=0;   //receive data  counting, current floor

int bef[2] = {101,101}; //엘리베이터 LED제어 차이값 불켜기
int count  = 1;         //엘리베이터 제어 카운트 %2 하기위한값
int index =0;
class_kproject_device_multi_shift_register s_register(latch_pin, clock_pin, data_pin, 5);
class_kproject_device_multi_shift_register shift(3,4,2,2);
class_kproject_device_segment segment1(&shift,101,102,103,104,105,106,107,108,true);
class_kproject_device_segment segment2(&shift,109,110,111,112,113,114,115,116,true);

void setup()
{
  // Connect to server
  Serial.begin(9600);
  esp8266.begin(9600); // esp8266 baud rate

  Serial.println("");
  Serial.println("-------ESP8266 Client Setup Start!-------");
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=1\r\n",1000,DEBUG);  //set sta mode
  sendData("AT+CWJAP=\"AndroidHotspot7743\",\"123454321\"\r\n", 8000, DEBUG);  //wifi connect
  sendData("AT+CIFSR\r\n",2000,DEBUG);  //get ip address
  sendData("AT+CIPMUX=0\r\n",1000,DEBUG); // configure for single connections
  sendData("AT+CIPSTART=\"TCP\",\"192.168.43.126\",8888\r\n",2000,DEBUG);  //client connect
  Serial.println("-------ESP8266 Client Setup End!-------");
}

void loop()
{
  long one,ten,value1;
  for(int i =0; i < 12; i++) buf[i] = '\0';  //receive buffer init
  data="";  //spend buffer init

  if(esp8266.available()) {  //running until receive data

    for(int i = 0; i <12; i++) {  //receive 12char from server
      char c = esp8266.read();  //read 1byte from esp8266
      buf[i] = c;   //save receive 1byte data
      Serial.print("buf[");
      Serial.print(i);
      Serial.print("]=");
      Serial.println(buf[i]);  //print receive data to Serial monitor
      cnt++;
    }

    if(current == atoi(buf+9)) {
       Serial.println("Receive same floor!");
       bufferFlush();
    }

    else if (current < atoi(buf+9)) {
      data="up_move\r\n";
      sendData("AT+CIPSEND=9\r\n",1000,DEBUG);  //send 3byte
      delay(300);  //send delay
      sendData(data,2000,DEBUG);
      delay(1000);
      data="";
      bufferFlush();

      value1 = atoi(buf+9);

      one = value1%10;
      ten= value1/10;

      segment1.show_int(one);
      segment1.refresh();
      segment2.show_int(ten);
      segment2.refresh();
      Serial.print(value1);
      Serial.println(" F   MOVE ");
      bef[count] = 100+value1;
      rise(bef[count],bef[index]);

      if(count==1)
        index=1;
      else
        index=0;

      count++;
      count= count%2;

      data="stop\r\n";
      sendData("AT+CIPSEND=6\r\n",1000,DEBUG);  //send 3byte
      delay(300);  //send delay
      sendData(data,2000,DEBUG);
      delay(1000);
      data="";
      bufferFlush();

      sendReceive();
    }

    else {
      data="down_move\r\n";
      sendData("AT+CIPSEND=11\r\n",1000,DEBUG);  //send 3byte
      delay(300);  //send delay
      sendData(data,2000,DEBUG);
      delay(1000);
      data="";
      bufferFlush();

      value1 = atoi(buf+9);

      one = value1%10;
      ten= value1/10;

      segment1.show_int(one);
      segment1.refresh();
      segment2.show_int(ten);
      segment2.refresh();
      Serial.print(value1);
      Serial.println(" F   MOVE ");
      bef[count] = 100+value1;
      rise(bef[count],bef[index]);

      if(count==1)
        index=1;
      else
        index=0;

      count++;
      count= count%2;

      data="stop\r\n";
      sendData("AT+CIPSEND=6\r\n",1000,DEBUG);  //send 3byte
      delay(300);  //send delay
      sendData(data,2000,DEBUG);
      delay(1000);
      data="";
      bufferFlush();

      sendReceive();
    }
  }
}


void rise(int pre, int after) // elevator up
{
  if(pre <= after)
  {
    for(after; pre<after; after--)
    {
      s_register.digitalWrite(after,LOW);
      delay(300);
      s_register.refresh();
    }
  }
  else
  {
    for(after; after<= pre; after++)
    {
      s_register.digitalWrite(after,HIGH);
      delay(300);
      s_register.refresh();
    }
  }
}

String sendData(String command, const int timeout, boolean debug) // send elevator data to server
{
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();

    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window
        char c = esp8266.read(); // read the next character.
        response+=c;
      }
    }

    if(debug) Serial.print(response);

    return response;
}

void sendReceive() {
  if(buf[10] == '\r') {   //if receive data is single number (data < 10)
    data+=buf[9];  //single number
    data+="\r\n";
    current = atoi(buf+9);
    Serial.print("Current floor = ");
    Serial.println(current);
    Serial.print("Receive Data = ");
    Serial.println(data);
    sendData("AT+CIPSEND=3\r\n",1000,DEBUG);  //send 3byte
    delay(300);  //send delay
    sendData(data,2000,DEBUG);  //send data
    delay(1000);
    cnt = 0;
  }
  else {  //receive data is double number(data >= 10)
    data+=buf[9];
    data+=buf[10];
    data+="\r\n";
    current = atoi(buf+9);
    Serial.print("Current floor = ");
    Serial.println(current);
    Serial.print("Receive Data = ");
    Serial.println(data);
    sendData("AT+CIPSEND=4\r\n",1000,DEBUG);  //send 4byte
    delay(300);  //send delay
    sendData(data,2000,DEBUG);  //send data
    delay(1000);
    cnt = 0;
  }
}

void bufferFlush() {  //clear esp8266 buffer
  while(esp8266.available()) {
    esp8266.read();
  }
}
