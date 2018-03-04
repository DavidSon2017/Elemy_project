#include <arduino.h>
#include "class_kproject_device.h"
#include "class_kproject_device_multi_shift_register.h"

//////////////////////////////////////////////////////////////////////////////////////////
// device 7 segment(1 digit, direct, shift register)
//////////////////////////////////////////////////////////////////////////////////////////
// pin_a , segment 7
// pin_b , segment 6
// pin_c , segment 4
// pin_d , segment 2
// pin_e , segment 1
// pin_f , segment 9
// pin_g , segment 10
// pin_dp , segment 5
//////////////////////////////////////////////////////////////////////////////////////////
extern byte segment_number_data[11];

class class_kproject_device_segment : public class_kproject_device
{
public:
  boolean status; // 0 : off , 1 : on
  byte m_bit_data;
  class_kproject_device_multi_shift_register *m_shift;
  unsigned char m_gnd_pin;
  unsigned char m_pin[8]; // a,b,c,d,e,f,g,dp
  boolean m_type; // type : 1 : anode , 2 : cathodic
  boolean use_74hc595;
// gnd���� �����ϴ� ���, shift registor ��� ����.
  class_kproject_device_segment(
	  unsigned char pin_e, 
	  unsigned char pin_d, 
	  unsigned char pin_c, 
	  unsigned char pin_dp, 
	  unsigned char pin_b, 
	  unsigned char pin_a, 
	  unsigned char pin_f, 
	  unsigned char pin_g, 
	  unsigned char pin_gnd, 
	  boolean type = true);
// gnd���� �������� �ʴ� ���, shift registor ��� ����.
  class_kproject_device_segment(
	  unsigned char pin_e, 
	  unsigned char pin_d, 
	  unsigned char pin_c, 
	  unsigned char pin_dp, 
	  unsigned char pin_b, 
	  unsigned char pin_a, 
	  unsigned char pin_f, 
	  unsigned char pin_g, 
	  boolean type = true);
// gnd���� �����ϴ� ���, shift registor ���
  class_kproject_device_segment(
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
	  boolean type = true
	  );
// gnd���� �������� �ʴ� ���, shift registor ���
  class_kproject_device_segment(
	  class_kproject_device_multi_shift_register *shift, 
	  unsigned char pin_e, 
	  unsigned char pin_d, 
	  unsigned char pin_c, 
	  unsigned char pin_dp, 
	  unsigned char pin_b, 
	  unsigned char pin_a, 
	  unsigned char pin_f, 
	  unsigned char pin_g, 
	  boolean type = true
	  );
  void show_int(int value);
  void show_bit(byte bit_data);
  void off();
  void refresh();
  virtual void setup();
  virtual void loop();
};

