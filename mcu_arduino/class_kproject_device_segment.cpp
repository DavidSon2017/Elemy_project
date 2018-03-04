#include "class_kproject_device_segment.h"

class_kproject_device_segment::class_kproject_device_segment(
	unsigned char pin_e, 
	unsigned char pin_d, 
	unsigned char pin_c, 
	unsigned char pin_dp, 
	unsigned char pin_b, 
	unsigned char pin_a, 
	unsigned char pin_f, 
	unsigned char pin_g, 
	unsigned char pin_gnd, 
	boolean type) : class_kproject_device()
{
  pinMode(pin_a, OUTPUT);
  pinMode(pin_b, OUTPUT);
  pinMode(pin_c, OUTPUT);
  pinMode(pin_d, OUTPUT);
  pinMode(pin_e, OUTPUT);
  pinMode(pin_f, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_dp, OUTPUT);
  pinMode(pin_gnd, OUTPUT);
  m_gnd_pin = pin_gnd;
  m_type = type;
  
  m_pin[0] = pin_a; // a
  m_pin[1] = pin_b; // b
  m_pin[2] = pin_c; // c
  m_pin[3] = pin_d; // d
  m_pin[4] = pin_e; // e
  m_pin[5] = pin_f; // f
  m_pin[6] = pin_g; // g
  m_pin[7] = pin_dp; // dp
  
  status = 0; // ���۽ô� ���� ����
  m_bit_data = 0b00000000; // ��� �� �ʱ�ȭ
  //  device::device();
}

class_kproject_device_segment::class_kproject_device_segment(
	unsigned char pin_e, 
	unsigned char pin_d, 
	unsigned char pin_c, 
	unsigned char pin_dp, 
	unsigned char pin_b, 
	unsigned char pin_a, 
	unsigned char pin_f, 
	unsigned char pin_g, 
	boolean type) : class_kproject_device()
{
	pinMode(pin_a, OUTPUT);
	pinMode(pin_b, OUTPUT);
	pinMode(pin_c, OUTPUT);
	pinMode(pin_d, OUTPUT);
	pinMode(pin_e, OUTPUT);
	pinMode(pin_f, OUTPUT);
	pinMode(pin_g, OUTPUT);
	pinMode(pin_dp, OUTPUT);
	m_gnd_pin = -1;
	m_type = type;
	
	m_pin[0] = pin_a; // a
	m_pin[1] = pin_b; // b
	m_pin[2] = pin_c; // c
	m_pin[3] = pin_d; // d
	m_pin[4] = pin_e; // e
	m_pin[5] = pin_f; // f
	m_pin[6] = pin_g; // g
	m_pin[7] = pin_dp; // dp
	
	status = 0; // ���۽ô� ���� ����
	m_bit_data = 0b00000000; // ��� �� �ʱ�ȭ
//  device::device();
}

class_kproject_device_segment::class_kproject_device_segment(
	class_kproject_device_multi_shift_register *shift, 
	unsigned char pin_e, 
	unsigned char pin_d, 
	unsigned char pin_c, 
	unsigned char pin_dp, 
	unsigned char pin_b, 
	unsigned char pin_a, 
	unsigned char pin_f, 
	unsigned char pin_g, 
	unsigned char pin_gnd, 
	boolean type) : class_kproject_device()
{
  m_shift = shift;
  m_type = type;
  m_pin[0] = pin_a; // a , segment 7
  m_pin[1] = pin_b; // b , segment 6
  m_pin[2] = pin_c; // c , segment 4
  m_pin[3] = pin_d; // d , segment 2
  m_pin[4] = pin_e; // e , segment 1
  m_pin[5] = pin_f; // f , segment 9
  m_pin[6] = pin_g; // g , segment 10
  m_pin[7] = pin_dp; // dp , segment 5
  m_gnd_pin = pin_gnd;
  
  for( int i = 0; i < 8; i++)
  {
    if ( m_pin[i] < 101)
    {
      pinMode(m_pin[i],OUTPUT);
    }
  }
  if ( pin_gnd < 101 ) pinMode(pin_gnd,OUTPUT);
  
  status = 0; // ���۽ô� ���� ����
  m_bit_data = 0b00000000; // ��� �� �ʱ�ȭ
  use_74hc595 = true;
  //  device::device();
}

class_kproject_device_segment::class_kproject_device_segment(
	class_kproject_device_multi_shift_register *shift, 
	unsigned char pin_e, 
	unsigned char pin_d, 
	unsigned char pin_c, 
	unsigned char pin_dp, 
	unsigned char pin_b, 
	unsigned char pin_a, 
	unsigned char pin_f, 
	unsigned char pin_g, 
	boolean type) : class_kproject_device()
{
	m_shift = shift;
	m_type = type;
	m_pin[0] = pin_a; // a , segment 7
	m_pin[1] = pin_b; // b , segment 6
	m_pin[2] = pin_c; // c , segment 4
	m_pin[3] = pin_d; // d , segment 2
	m_pin[4] = pin_e; // e , segment 1
	m_pin[5] = pin_f; // f , segment 9
	m_pin[6] = pin_g; // g , segment 10
	m_pin[7] = pin_dp; // dp , segment 5
	m_gnd_pin = -1;
	
	for( int i = 0; i < 8; i++)
	{
		if ( m_pin[i] < 101)
		{
			pinMode(m_pin[i],OUTPUT);
		}
	}
	
	status = 0; // ���۽ô� ���� ����
	m_bit_data = 0b00000000; // ��� �� �ʱ�ȭ
	use_74hc595 = true;
	//  device::device();
}

void class_kproject_device_segment::setup()
{
}

void class_kproject_device_segment::loop()
{
}

void class_kproject_device_segment::show_int(int number)
{
  int digit1;
  digit1 = number % 10;
  m_bit_data = segment_number_data[digit1];
  status = 1;
}

void class_kproject_device_segment::show_bit(byte bit_data)
{
  m_bit_data = bit_data;
  status = 1;
}

void class_kproject_device_segment::off()
{
  status = 0;
}

void class_kproject_device_segment::refresh()
{
	if ( status == 1 )
	{
		if ( m_type == false )
		{
			if ( m_gnd_pin != -1 ) m_shift->digitalWrite(m_gnd_pin,LOW);
		}
		else
		{
			if ( m_gnd_pin != -1 ) m_shift->digitalWrite(m_gnd_pin,HIGH);
		}
		
		for( int i = 0; i < 8; i++)
		{
			if ( m_type == true )
			{
				if ((m_bit_data & (((long)1) << i)) != 0 )
				{
					m_shift->digitalWrite(m_pin[i], HIGH);
				}
				else
				{
					m_shift->digitalWrite(m_pin[i], LOW);
				}
			}
			else
			{
				if ((m_bit_data & (((long)1) << i)) != 0 )
				{
					m_shift->digitalWrite(m_pin[i], LOW);
				}
				else
				{
					m_shift->digitalWrite(m_pin[i], HIGH);
				}
			}
		}
		if ( use_74hc595 == true ) m_shift->refresh();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
// device 7 segment(1 digit, direct, shift register)
//////////////////////////////////////////////////////////////////////////////////////////

