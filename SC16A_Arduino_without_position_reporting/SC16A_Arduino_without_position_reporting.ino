const char tx = 1;  //assign tx with pin 1 (transmit)
const char led_red = 13;  // assign led_red with pin 13
const char button = 12; //assign button as  pin 12
void setup()
{
 pinMode(button,INPUT); //set button (or pin 12) as input
 pinMode(tx, OUTPUT);  //set rx (or pin 0) as input
 pinMode(led_red, OUTPUT); //set led_red (or pin 13) as output
 Serial.begin(9600);  //initialize serial communication for UART with baudrate 9600 bps 
 while(digitalRead(button));  //waiting button to be pressed
 while(!digitalRead(button));  //waiting button to be released
}

void loop()
{
  for(int count = 0; count < 16; count++)  //set 1st position, speed for all servo motor channels (servo motors), assignable speed value (0-63)
  {
     set_ch_pos_spd(count+1, 1250, 50);  //set position = 1250, speed = 50
  }
  digitalWrite(led_red,HIGH);  //led is on (indication)
  delay(5000);  //delay for a while  *(delay have to set longer for lower, because the servo motor require more time to reach its position)
  
  for(int count = 0; count < 16; count++)  //set 2nd position, speed for all servo motor channels (servo motors), assignable speed value (0-63)
  {
     set_ch_pos_spd(count+1, 150, 50);   //set position = 150, speed = 50
  }
  digitalWrite(led_red,LOW);  //led is off (indication)
  delay(5000); //delay for a while *(delay have to set longer for lower, because the servo motor require more time to reach its position)
}

void set_ch_pos_spd(unsigned char channel, unsigned int position, unsigned char velocity)
{
 /*****Position and Speed Command*****
 - 4 bytes involved
 - 1st byte: Start byte + Servo motor channel
   eg: 
     0b 0 1 x x x x x x 
      Start|Servo channel
       bit
 - 2 MSBs (01) is mode for position and speed command
 - the last xxxxxx can be assigned with value 1-32, where
   1 ---> select channel 1
   2 ---> select channel 2
   ....
   ....
   32 ---> select channel 32
 - 2nd byte: Position (High byte) higher 6-bit  
   eg: 
     0b 0 0 x x x x x x
 - the last xxxxxx can be assigned with value 0 - 63
 - 3nd byte: Position (Low byte) lower 6-bit
   eg: 
     0b 0 0 x x x x x x
 - the last xxxxxx can be assigned with value 0 - 63
   **2nd byte and 3byte is the position value when combined together into 12 bits position
 - 4th byte: Speed (0-63)
   eg:
     0b 0 0 x x x x x x
 - the last xxxxxx can be assigned with value 0 - 63
 ************************************/
 unsigned char first_byte = 0;
 unsigned char high_byte = 0;
 unsigned char low_byte = 0; 
 
 first_byte = 0b01000000 | channel;  //make up the 1st byte
 high_byte = (position >> 6) & 0b00011111;  //obtain the high byte of 12 bits position value
 low_byte = position & 0b00111111;  //obtain the low byte of 12 bits position value
 
 Serial.write(first_byte);  //send the 1st byte
 Serial.write(high_byte);  //send the 2nd byte
 Serial.write(low_byte);  //send the 3rd byte
 Serial.write(velocity);  //send the 4th byte
}

