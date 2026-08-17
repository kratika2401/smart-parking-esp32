#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// SMART PARKING - 2 SLOT ULTRASONIC SYSTEM
// ESP32 + 2x HC-SR04 + LEDs + Buzzer + OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =====================================================
// PIN CONFIGURATION
// =====================================================

const int TRIG_PIN_1 = 5;
const int ECHO_PIN_1 = 18;

const int TRIG_PIN_2 = 19;
const int ECHO_PIN_2 = 23;

const int GREEN_LED_1 = 13;
const int RED_LED_1   = 14;

const int GREEN_LED_2 = 25;
const int RED_LED_2   = 26;

const int BUZZER_PIN = 27;

// =====================================================
// PARKING SETTINGS
// =====================================================

const float OCCUPIED_THRESHOLD = 35.0;
const float MIN_DISTANCE = 2.0;
const float MAX_DISTANCE = 400.0;
const int NUM_SAMPLES = 5;
const int REQUIRED_CONFIRMATIONS = 2;
const int NUM_SLOTS = 2;

// =====================================================
// SLOT STATE VARIABLES
// =====================================================

bool currentOccupied[NUM_SLOTS] = {false, false};
bool pendingState[NUM_SLOTS] = {false, false};
int confirmationCount[NUM_SLOTS] = {0, 0};

// =====================================================
// READ ULTRASONIC DISTANCE
// =====================================================

float readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;

  if (distance < MIN_DISTANCE || distance > MAX_DISTANCE) {
    return -1;
  }

  return distance;
}

// =====================================================
// AVERAGE DISTANCE
// =====================================================

float getAverageDistance(int trigPin, int echoPin) {
  float sum = 0;
  int validReadings = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    float distance = readDistanceCM(trigPin, echoPin);

    if (distance > 0) {
      sum += distance;
      validReadings++;
    }

    delay(20);
  }

  if (validReadings == 0) {
    return -1;
  }

  return sum / validReadings;
}

// =====================================================
// UPDATE STABLE STATE
// =====================================================

void updateStableState(int slot, bool newDecision) {
  if (newDecision != pendingState[slot]) {
    pendingState[slot] = newDecision;
    confirmationCount[slot] = 1;
  } else {
    confirmationCount[slot]++;
  }

  if (confirmationCount[slot] >= REQUIRED_CONFIRMATIONS) {
    if (currentOccupied[slot] != pendingState[slot]) {
      currentOccupied[slot] = pendingState[slot];

      Serial.println();
      Serial.print("*** SLOT ");
      Serial.print(slot + 1);
      Serial.println(" STATE CHANGED ***");

      Serial.print("Slot ");
      Serial.print(slot + 1);
      Serial.print(": ");

      if (currentOccupied[slot]) {
        Serial.println("OCCUPIED");
      } else {
        Serial.println("FREE");
      }
    }

    confirmationCount[slot] = 0;
  }
}

// =====================================================
// UPDATE LEDS
// =====================================================

void updateLEDs() {
  if (currentOccupied[0]) {
    digitalWrite(GREEN_LED_1, LOW);
    digitalWrite(RED_LED_1, HIGH);
  } else {
    digitalWrite(GREEN_LED_1, HIGH);
    digitalWrite(RED_LED_1, LOW);
  }

  if (currentOccupied[1]) {
    digitalWrite(GREEN_LED_2, LOW);
    digitalWrite(RED_LED_2, HIGH);
  } else {
    digitalWrite(GREEN_LED_2, HIGH);
    digitalWrite(RED_LED_2, LOW);
  }
}

// =====================================================
// GET FREE SLOT COUNT
// =====================================================

int getFreeSlotsCount() {
  int freeSlots = 0;

  for (int i = 0; i < NUM_SLOTS; i++) {
    if (!currentOccupied[i]) {
      freeSlots++;
    }
  }

  return freeSlots;
}

// =====================================================
// UPDATE BUZZER
// =====================================================

void updateBuzzer(int freeSlots) {
  if (freeSlots == 0) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }
}

// =====================================================
// UPDATE OLED
// =====================================================

void updateOLED(float distance1, float distance2, int freeSlots) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println("SMART PARKING");

  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  display.setCursor(0, 15);
  display.print("SLOT 1: ");
  if (currentOccupied[0]) {
    display.println("OCCUPIED");
  } else {
    display.println("FREE");
  }

  display.setCursor(0, 27);
  display.print("SLOT 2: ");
  if (currentOccupied[1]) {
    display.println("OCCUPIED");
  } else {
    display.println("FREE");
  }

  display.setCursor(0, 39);
  display.print("D1:");
  if (distance1 > 0) {
    display.print(distance1, 0);
    display.print("cm");
  } else {
    display.print("--");
  }

  display.setCursor(65, 39);
  display.print("D2:");
  if (distance2 > 0) {
    display.print(distance2, 0);
    display.print("cm");
  } else {
    display.print("--");
  }

  display.setCursor(0, 51);
  display.print("AVAILABLE: ");
  display.print(freeSlots);
  display.print("/");
  display.print(NUM_SLOTS);

  display.display();
}

// =====================================================
// PRINT PARKING STATUS
// =====================================================

void printParkingStatus() {
  int freeSlots = getFreeSlotsCount();

  Serial.println();
  Serial.println("========== PARKING STATUS ==========");

  Serial.print("Slot 1: ");
  if (currentOccupied[0]) {
    Serial.println("OCCUPIED");
  } else {
    Serial.println("FREE");
  }

  Serial.print("Slot 2: ");
  if (currentOccupied[1]) {
    Serial.println("OCCUPIED");
  } else {
    Serial.println("FREE");
  }

  Serial.print("Available Slots: ");
  Serial.print(freeSlots);
  Serial.print(" / ");
  Serial.println(NUM_SLOTS);

  Serial.print("STATUS: ");
  if (freeSlots == 0) {
    Serial.println("PARKING FULL");
  } else {
    Serial.println("SPACE AVAILABLE");
  }

  Serial.println("====================================");
}

// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  digitalWrite(TRIG_PIN_1, LOW);
  digitalWrite(TRIG_PIN_2, LOW);

  pinMode(GREEN_LED_1, OUTPUT);
  pinMode(RED_LED_1, OUTPUT);
  pinMode(GREEN_LED_2, OUTPUT);
  pinMode(RED_LED_2, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(15, 20);
    display.println("SMART PARKING");
    display.setCursor(25, 35);
    display.println("SYSTEM READY");
    display.display();
    delay(1500);
  }

  updateLEDs();

  Serial.println();
  Serial.println("====================================");
  Serial.println(" SMART PARKING SYSTEM STARTED");
  Serial.println("====================================");
  Serial.println("2 Ultrasonic Sensors Active");
  Serial.print("Occupied Threshold: ");
  Serial.print(OCCUPIED_THRESHOLD);
  Serial.println(" cm");
  Serial.println("====================================");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
  float distance1 = getAverageDistance(TRIG_PIN_1, ECHO_PIN_1);
  float distance2 = getAverageDistance(TRIG_PIN_2, ECHO_PIN_2);

  if (distance1 > 0) {
    bool decision1 = (distance1 <= OCCUPIED_THRESHOLD);
    updateStableState(0, decision1);

    Serial.print("Slot 1 Distance: ");
    Serial.print(distance1, 2);
    Serial.print(" cm | Decision: ");

    if (decision1) {
      Serial.println("OCCUPIED");
    } else {
      Serial.println("FREE");
    }
  } else {
    Serial.println("Slot 1: INVALID READING");
  }

  if (distance2 > 0) {
    bool decision2 = (distance2 <= OCCUPIED_THRESHOLD);
    updateStableState(1, decision2);

    Serial.print("Slot 2 Distance: ");
    Serial.print(distance2, 2);
    Serial.print(" cm | Decision: ");

    if (decision2) {
      Serial.println("OCCUPIED");
    } else {
      Serial.println("FREE");
    }
  } else {
    Serial.println("Slot 2: INVALID READING");
  }

  updateLEDs();

  int freeSlots = getFreeSlotsCount();

  updateBuzzer(freeSlots);

  updateOLED(distance1, distance2, freeSlots);

  printParkingStatus();

  delay(500);
}
