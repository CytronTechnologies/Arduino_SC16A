const char tx = 1;  //assign tx with pin 1 (transmit)
const char rx = 0;  //assign rx with pin 0 (receive)
const char led_red = 13;  // assign led_red with pin 13
const char button = 12; // assign button with pin 12
void setup()
{
 pinMode(button,INPUT);  //set button (or pin 12) as input
 pinMode(tx, OUTPUT);  //set rx (or pin 0) as input
 pinMode(rx, INPUT);  //set tx (or pin 1) as output
 pinMode(led_red, OUTPUT); //set led_red (or pin 13) as output
 Serial.begin(9600);  //initialize serial communication for UART with baudrate 9600 bps 
 while(digitalRead(button)); //waiting button to be pressed
 while(!digitalRead(button)); //waiting button to be released
}

void loop()
{
  for(int count = 0; count < 16; count++)  //set 1st position, speed for all servo motor channels (servo motors)
  {                                       //In position reporting operation, setting speed = 0 will cause the servo motor oscillates due to limitation of software position feedback
     set_ch_pos_spd(count+1, 1250, 50);  //set position = 1250, speed = 50
  }
  digitalWrite(led_red, HIGH);  //led is on (indicator) 
  while(reading_position(1) < 1150);  //wait untill the (position-100) is reached
  
  for(int count = 0; count < 16; count++)  //set 2nd position, speed for all servo motor channels (servo motors)
  {                                        //In position reporting operation, setting speed = 0 will cause the servo motor oscillates due to limitation of software position feedback
    set_ch_pos_spd(count+1, 150, 50);  //set position = 150, speed = 50
  }  
  digitalWrite(led_red,LOW);  //led is off (indicator)
  while(reading_position(1) > 250);  //wait untill the (position + 100) is reached
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

unsigned int reading_position(unsigned char channel)
{
 /*****Servo position reporting Command*****
 - 5 bytes involved (send 2 byte, received 3 bytes)
 - 1st byte: Start byte (Send)
   Start byte:
     0b 0 1 0 0 0 0 0 0 or 0x40
 - request mode for position reporting command
 - 2nd byte: Start bit + Servo motor channel (Send)
   eg:
     0b 0 1 x x x x x x 
      Start|Servo channel
       bit
 - the last xxxxxx can be assigned with value 1-32, where
   1 ---> select channel 1
   2 ---> select channel 2
   ....
   ....
   32 ---> select channel 32
 - 3rd byte: Start bit + Servo motor number (1st received byte)
   eg:
     0b 0 1 x x x x x x 
      Start| Servo motor number
 - Received value vary 1-32, where
   1 ---> indicate channel 1
   2 ---> indicate channel 2
   ....
   ....
   32 ---> indicate channel 32
 - 4th byte: Position (High byte) higher 6-bit (2nd received byte) 
   eg:
     0b 0 0 x x x x x x 
 - Received value vary between 0 - 63
 - 5th byte: Position (Low byte) lower 6-bit (3rd received byte)
 - Received value vary between 0 - 63
 **2nd byte and 3byte is the position value when combined together into 12 bits
******************************************/
  unsigned char first_byte = 0b01000000;  //make up the 1st byte
  unsigned char second_byte = 0;
  unsigned char buffer[3];
  unsigned char high_byte = 0;
  unsigned char low_byte = 0;
  unsigned int reading_position = 0;
  
  second_byte = 0b01000000 | channel; //make up the 2nd byte
  Serial.write(first_byte); //send the 1st byte
  Serial.write(second_byte);  //send the 2nd byte
  delay(200);  //short delay for sending the data 
 
  while(!Serial.available());  //wait untill data received
  for(int count = 0; count < 3; count++)  //store three bytes received in an array
  {
     buffer[count] = Serial.read(); 
  }  
  
  //combine received high byte & low byte into 12 bits position value
  high_byte = buffer[1] & 0b00011111;  
  low_byte = buffer[2] & 0b00111111;  
  reading_position = high_byte;
  reading_position = (reading_position << 6) | low_byte;
  return reading_position;  //return position value to the main
}

