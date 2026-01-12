#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 5
MFRC522 rfid(SS_PIN, RST_PIN);

// Define UIDs for lanes
byte laneUIDs[4][4] = {
  {0x9D, 0xA7, 0x9F, 0x3C}, // Lane 1
  {0xF4, 0xFE, 0xC8, 0xAF}, // Lane 2
  {0xE7, 0x4E, 0xF8, 0xB4}, // Lane 3
  {0x05, 0x6B, 0xC0, 0x01}  // Lane 4
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Scan RFID tag...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID Detected: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print("0x");
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i != rfid.uid.size - 1) Serial.print(", ");
  }
  Serial.println();

  // Check against each lane's UID
  for (int lane = 0; lane < 4; lane++) {
    bool match = true;
    for (byte j = 0; j < 4; j++) {
      if (rfid.uid.uidByte[j] != laneUIDs[lane][j]) {
        match = false;
        break;
      }
    }
    if (match) {
      Serial.print("Emergency vehicle tag matched for Lane ");
      Serial.println(lane + 1);
      break;
    }
  }

  rfid.PICC_HaltA(); // Stop communication
}