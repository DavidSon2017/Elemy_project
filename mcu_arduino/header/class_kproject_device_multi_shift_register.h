#ifndef _class_kproject_device_multi_shift_registger
#define _class_kproject_device_multi_shift_registger

#include <arduino.h>
#include "class_kproject_device.h"

#define _ARDUINO_UNO

//////////////////////////////////////////////////////////////////////////////////////////
// multi shift register
//////////////////////////////////////////////////////////////////////////////////////////
// 74HC595�� �̿��� Ȯ��  PIN�� PIN��ȣ 101���� ����
// SHIFT REGISTER�� 1���� ��� 101~108�� ���
// SHIFT REGISTER�� 2���� ��� 101~116�� ���
// SHIFT REGISTER�� 3���� ��� 101~124�� ���
//  ........
// SHIFT REGISTER�� �ִ� 6������ ����
// register_data����
// register_data[0] 0b0000000000 => DATA BIT MATCH  �տ������� ��� �ɹ�ȣ  = 108, 107, 106, 105, 104, 103, 102, 101
// register_data[1] 0b0000000000 => DATA BIT MATCH  �տ������� ��� �ɹ�ȣ  = 116, 115, 114, 113, 112, 111, 110, 109
//////////////////////////////////////////////////////////////////////////////////////////
// 74HC595 PIN ������
//////////////////////////////////////////////////////////////////////////////////////////
// 74HC595, PIN 15 = OUT PIN 15
// 74HC595, PIN 1 =  OUT PIN 1
// 74HC595, PIN 2 =  OUT PIN 2
// 74HC595, PIN 3 =  OUT PIN 3
// 74HC595, PIN 4 =  OUT PIN 4
// 74HC595, PIN 5 =  OUT PIN 5
// 74HC595, PIN 6 =  OUT PIN 6
// 74HC595, PIN 7 =  OUT PIN 7
// 74HC595, PIN 8 =  GND ����
// 74HC595, PIN 9 =  SERIAL OUT(���� IC�� ������ �ѱ�)
// 74HC595, PIN 10 =  MASTER RECLEAR, ACTIVE LOW(+�� ����)
// 74HC595, PIN 11 =  �Ƶ��̳� CLOCK PIN�� ����
// 74HC595, PIN 12 =  �Ƶ��̳� LATCH PIN�� ����
// 74HC595, PIN 13 =  GND ����(OUTPUT ENABLE, ACTIVE LOW)
// 74HC595, PIN 14 =  �Ƶ��̳� DATA PIN�� ����
// 74HC595, PIN 16 =  VCC(+�� ����)
// ###########################################################
//              VCC  OUT0  DATA GND LATCH CLOCK (+)   SERIAL(NULL)  
//               16     15     14     13     12     11     10     09
//               |      |      |      |      |      |      |      |
//                            74HC595                   
//               |      |      |      |      |      |      |      |
//               01     02     03     04     05     06     07     08
//              OUT1 OUT2 OUT3 OUT4 OUT5 OUT6 OUT7 GND
// ###########################################################
//////////////////////////////////////////////////////////////////////////////////////////

// _data[6] : shift register 6������ �����ϵ��� ���� pin�� Setting�� �����ϴ� ����
class class_kproject_device_multi_shift_register:public class_kproject_device
{
public:
	byte _data[6];				// pin ���¸� ����
	unsigned char m_latch_pin;	// 74HC595 12�� ��
	unsigned char m_clock_pin;	// 74HC595 11�� ��
	unsigned char m_data_pin;		//  74HC595 14�� ��
	unsigned char m_shift_register_no; 
public:
	class_kproject_device_multi_shift_register(unsigned char latch_pin, unsigned char clock_pin, unsigned char data_pin, unsigned char shift_register_no = 1);
	void digitalWrite(unsigned char pin, unsigned char status); // pin : 101~ , status : HIGH, LOW
	void refresh();
	void shift_out(unsigned char dataPin, unsigned char clockPin, byte val);
};

#endif
