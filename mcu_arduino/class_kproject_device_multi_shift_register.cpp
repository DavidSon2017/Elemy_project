#include "class_kproject_device_multi_shift_register.h"

class_kproject_device_multi_shift_register::class_kproject_device_multi_shift_register(unsigned char latch_pin, unsigned char clock_pin, unsigned char data_pin, unsigned char shift_register_no) : class_kproject_device()
{
#ifdef _DEBUG
	Serial.println("class_kproject_device_shift_register::class_kproject_device_shift_register");
#endif

	m_latch_pin = latch_pin;
	m_clock_pin = clock_pin;
	m_data_pin = data_pin;
	m_shift_register_no = shift_register_no;
	pinMode(m_latch_pin, OUTPUT);
	pinMode(m_clock_pin, OUTPUT);
	pinMode(m_data_pin, OUTPUT);
	for( int i = 0; i < m_shift_register_no; i++)
	{
		_data[i] = 0;
	}

#ifdef _DEBUG
	Serial.print("pinMode ");
	Serial.println(m_latch_pin);
	Serial.print("pinMode ");
	Serial.println(m_clock_pin);
	Serial.print("pinMode ");
	Serial.println(m_data_pin);
#endif
}

void class_kproject_device_multi_shift_register::digitalWrite(unsigned char pin, unsigned char status)
{
	if ( pin < 101 )
	{
		::digitalWrite(pin,status);
	}
	else
	{
		unsigned char pin_no = pin - 101;
		byte register_no;
		byte index_no;
		register_no = pin_no / 8;
		index_no = pin_no % 8;
		
		if ( status == HIGH )
		{
			_data[register_no] = _data[register_no] | (((byte)1) << index_no);
		}
		else
		{
			_data[register_no] = _data[register_no] & ~(((byte)1) << index_no);
		}
	}
}


void class_kproject_device_multi_shift_register::refresh()
{
	::digitalWrite(m_latch_pin, LOW);

	for( int i = m_shift_register_no-1; i >= 0; i--)
	{
		shift_out(m_data_pin, m_clock_pin, _data[i]);
	}
	::digitalWrite(m_latch_pin, HIGH);
}

#ifdef _ARDUINO_UNO
// ARDUINO UNO�� �ƴ� ��� �Ʒ� ���� ���
void class_kproject_device_multi_shift_register::shift_out(unsigned char dataPin, unsigned char clockPin, byte val)
{
	int i=0;
	
	for ( i = 7; i >= 0; i-- )  
	{
		::digitalWrite(clockPin, 0);
		::digitalWrite(dataPin, val & ( 1 << i ));
		::digitalWrite(clockPin, 1);
	}
}
#else
// ARDUINO UNO�� ��� �Ʒ� ������ ����ϸ� ���� ������ ó����.
#define sbi(port, bit) (port) |= (1 << (bit))     
#define cbi(port,bit) (port) &= ~(1 << (bit))  

void class_kproject_device_multi_shift_register::shift_out(unsigned char dataPin, unsigned char clockPin, byte val)
{
	uint8_t i,D_Pin,C_Pin;
	
	if(clockPin > 7) 
		C_Pin=clockPin-8;
	else
		C_Pin=clockPin;
	
	if(dataPin > 7)
	{
		D_Pin=dataPin-8;
		{
			for (i = 0; i < 8; i++)  
			{
				if( (val & (1 << (7 - i))) )  
					sbi(PORTB,D_Pin);
				else                               
					cbi(PORTB,D_Pin); 
				
				if(clockPin > 7)
				{ 
					sbi(PORTB,C_Pin);  
					cbi(PORTB,C_Pin);  
				}
				else               
				{ 
					sbi(PORTD,C_Pin);  
					cbi(PORTD,C_Pin);  
				}
			}
		}
	}
	else 
	{
		D_Pin=dataPin;
		{
			for (i = 0; i < 8; i++)  
			{
				if( (val & (1 << (7 - i))) )  
					sbi(PORTD,D_Pin);
				else                               
					cbi(PORTD,D_Pin); 
				if(clockPin > 7)
				{ 
					sbi(PORTB,C_Pin);  
					cbi(PORTB,C_Pin);  
				}
				else               
				{ 
					sbi(PORTD,C_Pin);  
					cbi(PORTD,C_Pin);  
				} 
			}
		}
	}
}
#endif
