#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 5
#define SS_PIN 53
MFRC522 rfid(SS_PIN, RST_PIN);

#define NUM_LANES 4
const int trigPins[NUM_LANES] = {22, 23, 24, 25};
const int echoPins[NUM_LANES] = {26, 27, 28, 29};
const int lightPins[NUM_LANES][3] = {
  {30, 31, 32},  // Lane 1: Green, Yellow, Red
  {33, 34, 35},
  {36, 37, 38},
  {42, 43, 41}
};

// UID database for each lane
byte laneUIDs[4][4] = {
  {0x9D, 0xA7, 0x9F, 0x3C}, // Lane 1
  {0xF4, 0xFE, 0xC8, 0xAF}, // Lane 2
  {0xE7, 0x4E, 0xF8, 0xB4}, // Lane 3
  {0x05, 0x6B, 0xC0, 0x01}  // Lane 4
};

const int greenTime = 5000;
const int yellowTime = 2000;
const int emergencyTime = 8000;
const int vehicleThreshold = 20;

int currentLane = 0;
bool inEmergency = false;
bool vehicleDetected[NUM_LANES] = {false};
bool rfidVerified[NUM_LANES] = {false};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  for (int i = 0; i < NUM_LANES; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    for (int j = 0; j < 3; j++) {
      pinMode(lightPins[i][j], OUTPUT);
    }
  }
  Serial.println("System Ready");
}

long getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 25000);
  return duration * 0.034 / 2;
}

void setAllRed() {
  for (int i = 0; i < NUM_LANES; i++) {
    digitalWrite(lightPins[i][0], LOW);
    digitalWrite(lightPins[i][1], LOW);
    digitalWrite(lightPins[i][2], HIGH);
  }
}

void setLaneGreen(int lane) {
  setAllRed();
  digitalWrite(lightPins[lane][0], HIGH); // Green ON
  digitalWrite(lightPins[lane][2], LOW);  // Red OFF
}

void setLaneYellow(int lane) {
  setAllRed();
  digitalWrite(lightPins[lane][1], HIGH); // Yellow ON
  digitalWrite(lightPins[lane][2], LOW);  // Red OFF
}

bool checkUID(byte *scannedUID, byte scannedSize, int &matchedLane) {
  for (int lane = 0; lane < NUM_LANES; lane++) {
    bool match = true;
    for (int i = 0; i < scannedSize; i++) {
      if (rfid.uid.uidByte[i] != laneUIDs[lane][i]) {
        match = false;
        break;
      }
    }
    if (match) {
      matchedLane = lane;
      return true;
    }
  }
  return false;
}

void loop() {
  // Check distance at each lane
  for (int i = 0; i < NUM_LANES; i++) {
    long dist = getDistanceCM(trigPins[i], echoPins[i]);
    Serial.print("Lane ");
    Serial.print(i + 1);
    Serial.print(" Distance: ");
    Serial.print(dist);
    Serial.println(" cm");

    if (dist < vehicleThreshold && !vehicleDetected[i]) {
      vehicleDetected[i] = true;
      Serial.print("Vehicle detected in Lane ");
      Serial.println(i + 1);
      rfidVerified[i] = false;
    } else if (dist >= vehicleThreshold && vehicleDetected[i]) {
      vehicleDetected[i] = false;
      rfidVerified[i] = false;
    }
  }

  // Check RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("UID Detected: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print("0x");
      if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
      Serial.print(rfid.uid.uidByte[i], HEX);
      if (i != rfid.uid.size - 1) Serial.print(", ");
    }
    Serial.println();

    int laneMatch = -1;
    if (checkUID(rfid.uid.uidByte, rfid.uid.size, laneMatch)) {
      Serial.print("Emergency vehicle tag matched for Lane ");
      Serial.println(laneMatch + 1);
      if (!rfidVerified[laneMatch] && !inEmergency) {
        rfidVerified[laneMatch] = true;
        inEmergency = true;
        setLaneGreen(laneMatch);
        Serial.println("Green light ON for matched lane!");
        delay(emergencyTime);
        inEmergency = false;
      }
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // Normal round-robin cycle
  if (!inEmergency) {
    Serial.print("Normal Cycle: Lane ");
    Serial.println(currentLane + 1);
    setLaneGreen(currentLane);
    delay(greenTime);

    setLaneYellow(currentLane);
    delay(yellowTime);

    currentLane = (currentLane + 1) % NUM_LANES;
  }
}