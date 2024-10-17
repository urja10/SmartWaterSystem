#include <VirtualWire.h>

#define sensor_pin A0 
#define transmit_pin 12  // Define the pin for RF transmitter

int read_ADC;
int ntu;

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Initialize the RF transmitter
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);  // Bits per second
}

void loop() {
  // Read the analog sensor value
  read_ADC = analogRead(sensor_pin);

  // Cap the ADC reading value at 208
  if (read_ADC > 208) read_ADC = 208;

  // Map the ADC value to turbidity NTU
  ntu = map(read_ADC, 0, 208, 300, 0);

  // Prepare the message to send
  char msg[20];
  sprintf(msg, "Turbidity: %d", ntu);

  // Send the message
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx(); // Wait until the whole message is gone

  // Delay before the next reading
  delay(1000);  // Increased delay to reduce transmission frequency
}