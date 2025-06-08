#include <Arduino.h>
#include <RF24.h>
#include <SPI.h>

#define GAS_PIN A1
#define BRAKE_PIN A3
#define CLUTCH_PIN A6

RF24 radio(10, 9);
const byte address[6] = "rmoza";

struct PedalData {
  uint16_t gas;
  uint16_t brake;
  uint16_t clutch;
};

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  analogReference(DEFAULT);

  if (!radio.begin()) {
    Serial.println("Init error: nRF24L01");
    while (1)
      ;
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  PedalData data;
  data.gas = analogRead(GAS_PIN);
  data.brake = analogRead(BRAKE_PIN);
  data.clutch = analogRead(CLUTCH_PIN);

#ifdef DEBUG
  Serial.print("$");
  Serial.print(data.gas);
  Serial.print(" ");
  Serial.print(data.brake);
  Serial.print(" ");
  Serial.print(data.clutch);
  Serial.print(";");
#endif

  bool result = radio.write(&data, sizeof(data));
}