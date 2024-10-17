#include <VirtualWire.h>

#define tds_sensor A0
#define DS18B20_PIN A5
#define G_led 8 
#define R_led 9 
#define buzzer 13
#define transmit_pin 12  // Define the pin for RF transmitter

float aref = 4.3;
float ecCalibration = 1;
float ec = 0;
unsigned int tds = 0;
int raw_temp;
float waterTemp = 0;

void setup() {
  pinMode(tds_sensor, INPUT);
  pinMode(R_led, OUTPUT);
  pinMode(G_led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
  Serial.println("Timestamp, TDS, EC, Temp");

  // Initialize the RF transmitter
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);  // Bits per second
}

void loop() {
  if (ds18b20_read(&raw_temp)) {
    waterTemp = (float)raw_temp / 16;
  }
  float rawEc = analogRead(tds_sensor) * aref / 1024.0;
  float temperatureCoefficient = 1.0 + 0.02 * (waterTemp - 25.0);
  ec = (rawEc / temperatureCoefficient) * ecCalibration;
  tds = (133.42 * pow(ec, 3) - 255.86 * ec * ec + 857.39 * ec) * 0.5;


  if (tds < 50 || tds > 700) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(G_led, LOW);
    digitalWrite(R_led, HIGH);
    delay(300);
  } else {
    digitalWrite(G_led, HIGH);
    digitalWrite(R_led, LOW);
  }

  digitalWrite(buzzer, LOW);

  // Prepare the message to send
  char msg[50];
  char ecStr[10];
  char tempStr[10];
  
  // Convert float to string
  dtostrf(ec, 4, 2, ecStr);
  dtostrf(waterTemp, 4, 2, tempStr);
  
  sprintf(msg, "TDS:%d,EC:%s,Temp:%s", tds, ecStr, tempStr);

  // Send the message
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx(); // Wait until the whole message is gone

  // Print data to the serial monitor
  Serial.println(msg);

  delay(500);  // Increased delay to reduce transmission frequency
}

bool ds18b20_start() {
  bool ret = 0;
  digitalWrite(DS18B20_PIN, LOW); // Send reset pulse to the DS18B20 sensor
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(500); // Wait 500 us
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(100); // Wait to read the DS18B20 sensor response
  if (!digitalRead(DS18B20_PIN)) {
    ret = 1; // DS18B20 sensor is present
    delayMicroseconds(400); // Wait 400 us
  }
  return (ret);
}

void ds18b20_write_bit(bool value) {
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  digitalWrite(DS18B20_PIN, value);
  delayMicroseconds(80);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(2);
}

void ds18b20_write_byte(byte value) {
  byte i;
  for (i = 0; i < 8; i++)
    ds18b20_write_bit(bitRead(value, i));
}

bool ds18b20_read_bit(void) {
  bool value;
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(5);
  value = digitalRead(DS18B20_PIN);
  delayMicroseconds(100);
  return value;
}

byte ds18b20_read_byte(void) {
  byte i, value;
  for (i = 0; i < 8; i++)
    bitWrite(value, i, ds18b20_read_bit());
  return value;
}

bool ds18b20_read(int *raw_temp_value) {
  if (!ds18b20_start()) // Send start pulse
    return (0); // Return 0 if error
  // Send skip ROM command
  ds18b20_write_byte(0xCC); 
  // Send start conversion command
  ds18b20_write_byte(0x44); 
  // Wait for conversion complete
  while (ds18b20_read_byte() == 0); 
  // Send start pulse
  if (!ds18b20_start()) 
    return (0); // Return 0 if error
  // Send skip ROM command
  ds18b20_write_byte(0xCC); 
  // Send read command
  ds18b20_write_byte(0xBE); 
  // Read temperature LSB byte and store it on raw_temp_value LSB byte
  *raw_temp_value = ds18b20_read_byte(); 
  // Read temperature MSB byte and store it on raw_temp_value MSB byte
  *raw_temp_value |= (unsigned int)(ds18b20_read_byte() << 8); 
  return (1);
}
