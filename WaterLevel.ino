#include <VirtualWire.h>

#define SensorPin A0
#define transmit_pin 12  // Define the pin for RF transmitter

void setup() {  
  // Initialize serial communication at 9600 baud
  Serial.begin(9600);

  // Initialize the RF transmitter
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);  // Bits per second
}

void loop() {
  // Read the analog sensor value
  int SensorValue = analogRead(SensorPin);

  // Calculate the voltage value 
  // (assuming 5V reference and 10-bit ADC)
  float SensorVolts = SensorValue * 5.0 / 1023.0; 
  
  // Output the voltage 
  // value to the Serial Monitor
  Serial.print("Voltage: ");
  // Print voltage with 3 decimal places
  Serial.println(SensorVolts);
  
  // Prepare the message to send
  char msg[50];
  char voltsStr[10];
  
  // Convert float to string
  dtostrf(SensorVolts, 4, 3, voltsStr);
  
  sprintf(msg, "Analog:%d,Volts:%s", SensorValue, voltsStr);

  // Send the message
  vw_send((uint8_t *)msg, strlen(msg));
  // Wait until the whole message is gone
  vw_wait_tx(); 

  // Wait before taking the next reading
  delay(1000);
}