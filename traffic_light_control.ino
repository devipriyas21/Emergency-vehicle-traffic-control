// 4-Lane Traffic Light Control System (Round-Robin)

#define NUM_LANES 4

// Traffic light pins: {Green, Yellow, Red}
const int lightPins[NUM_LANES][3] = {
  {30, 31, 32},  // Lane 1
  {33, 34, 35},  // Lane 2
  {36, 37, 38},  // Lane 3
  {42, 43, 41}   // Lane 4
};

const int greenTime  = 5000;  // Green light duration (ms)
const int yellowTime = 2000;  // Yellow light duration (ms)

int currentLane = 0;

void setup() {
  Serial.begin(9600);

  // Set all traffic light pins as OUTPUT
  for (int i = 0; i < NUM_LANES; i++) {
    for (int j = 0; j < 3; j++) {
      pinMode(lightPins[i][j], OUTPUT);
    }
  }

  Serial.println("Traffic Light System Started");
}

// Turn all signals RED
void setAllRed() {
  for (int i = 0; i < NUM_LANES; i++) {
    digitalWrite(lightPins[i][0], LOW);   // Green OFF
    digitalWrite(lightPins[i][1], LOW);   // Yellow OFF
    digitalWrite(lightPins[i][2], HIGH);  // Red ON
  }
}

// Set selected lane GREEN
void setLaneGreen(int lane) {
  setAllRed();
  digitalWrite(lightPins[lane][2], LOW);  // Red OFF
  digitalWrite(lightPins[lane][0], HIGH); // Green ON
}

// Set selected lane YELLOW
void setLaneYellow(int lane) {
  setAllRed();
  digitalWrite(lightPins[lane][2], LOW);  // Red OFF
  digitalWrite(lightPins[lane][1], HIGH); // Yellow ON
}

void loop() {
  Serial.print("Lane ");
  Serial.print(currentLane + 1);
  Serial.println(" GREEN");

  setLaneGreen(currentLane);
  delay(greenTime);

  Serial.print("Lane ");
  Serial.print(currentLane + 1);
  Serial.println(" YELLOW");

  setLaneYellow(currentLane);
  delay(yellowTime);

  // Move to next lane
  currentLane = (currentLane + 1) % NUM_LANES;
}
