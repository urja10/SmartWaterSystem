#include <VirtualWire.h>

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  vw_set_rx_pin(11);   // Set the receive pin
  vw_setup(2000);      // Bits per second
  vw_rx_start();       // Start the receiver
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) {  // Check if data is available
    Serial.print("Received: ");
    for (int i = 0; i < buflen; i++) {
      Serial.print((char)buf[i]);  // Print the received message
    }
    Serial.println();
  }
}