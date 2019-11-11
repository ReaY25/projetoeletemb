/*
  Blink
  The basic Energia example.
  Turns on an LED on for one second, then off for one second, repeatedly.
  Change the LED define to blink other LEDs.
  
  Hardware Required:
  * LaunchPad with an LED
  
  This example code is in the public domain.
*/

// most launchpads have a red LED
//#define LED RED_LED

//see pins_energia.h for more LED definitions
#define LED GREEN_LED
  
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(9600);
  pinMode(LED, OUTPUT); 
  pinMode(A0,INPUT); 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600); // msp430g2231 must use 4800   
}
int  duty_cycle=100;
void loop() {

    // read the input on analog pin A3:
  float sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.println(sensorValue);
  
  
  if (sensorValue > 1020 ){
    duty_cycle= duty_cycle + 10;
  }else if (sensorValue < 900){
      duty_cycle= duty_cycle - 10;
    }
  if(duty_cycle>=255)duty_cycle=255;
  if(duty_cycle<=0)duty_cycle=0;
  analogWrite(LED,duty_cycle);
  Serial.println("Duty_cycle");
  Serial.println(duty_cycle);
  delay(250);
  
  


  
}
