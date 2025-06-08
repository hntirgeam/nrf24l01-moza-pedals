#include <Arduino.h>
#include <RF24.h>
#include <SPI.h>

#define GAS_PIN 3
#define BRAKE_PIN 5
#define CLUTCH_PIN 6

RF24 radio(10, 9);
const byte address[6] = "rmoza";

struct PedalData {
  uint16_t gas;
  uint16_t brake;
  uint16_t clutch;
};

float filtered_gas = 0;
float filtered_brake = 0;
float filtered_clutch = 0;

const float alpha = 0.3;

int mapToPWM(uint16_t value) { return map(value, 188, 460, 36, 112); }

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  pinMode(GAS_PIN, OUTPUT);
  pinMode(BRAKE_PIN, OUTPUT);
  pinMode(CLUTCH_PIN, OUTPUT);

  if (!radio.begin()) {
    Serial.println("Error while init nRF24L01");
    while (1)
      ;
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    PedalData data;
    radio.read(&data, sizeof(data));

#ifdef DEBUG
    Serial.print("$");
    Serial.print(data.gas);
    Serial.print(" ");
    Serial.print(filtered_gas);
    Serial.print(" ");
    Serial.print(data.brake);
    Serial.print(" ");
    Serial.print(filtered_brake);
    Serial.print(" ");
    Serial.print(data.clutch);
    Serial.print(" ");
    Serial.print(filtered_clutch);
    Serial.println(";");
#endif

    filtered_gas = (1.0f - alpha) * filtered_gas + alpha * data.gas;
    filtered_brake = (1.0f - alpha) * filtered_brake + alpha * data.brake;
    filtered_clutch = (1.0f - alpha) * filtered_clutch + alpha * data.clutch;

    analogWrite(GAS_PIN, mapToPWM(filtered_gas));
    analogWrite(BRAKE_PIN, mapToPWM(filtered_brake));
    analogWrite(CLUTCH_PIN, mapToPWM(filtered_clutch));
  }
}