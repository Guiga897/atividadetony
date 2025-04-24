const int TEMP_SENSOR_PIN = A2;
const int POTENTIOMETER_PIN = A0;
const int LED_PIN = 12;
const int RGB_RED_PIN = 9;
const int RGB_GREEN_PIN = 10;
const int RGB_BLUE_PIN = 11;
const int STATUS_LED_PIN = 13;

// Estados globais dos LEDs
struct LedStates {
  bool mainLed;
  bool statusLed;
  int rgb[3];
} leds;

void setup() {
  initializePins();
  Serial.begin(9600);
  resetLedStates();
}

void loop() {
  readAndSendSensorData();
  processSerialCommands();
  delay(50);
}

void initializePins() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  resetLeds();
}

void resetLedStates() {
  leds.mainLed = false;
  leds.statusLed = false;
  leds.rgb[0] = 0;
  leds.rgb[1] = 0;
  leds.rgb[2] = 0;
}

void resetLeds() {
  digitalWrite(LED_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  analogWrite(RGB_RED_PIN, 0);
  analogWrite(RGB_GREEN_PIN, 0);
  analogWrite(RGB_BLUE_PIN, 0);
}

void readAndSendSensorData() {
  digitalWrite(STATUS_LED_PIN, HIGH);
  
  float temperature = readTemperature();
  Serial.print("TEMP:");
  Serial.println(temperature);

  float voltagePercent = readVoltagePercentage();
  Serial.print("VOLT:");
  Serial.println(voltagePercent);

  digitalWrite(STATUS_LED_PIN, LOW);
  delay(1000);
}

float readTemperature() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  return (sensorValue * (5.0 / 1023.0) * 100);
}

float readVoltagePercentage() {
  int potValue = analogRead(POTENTIOMETER_PIN);
  return (potValue / 1023.0) * 100;
}

void processSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "GET_STATES") {
      sendLedStates();
    }
    else if (command.startsWith("LED:")) {
      setMainLed(command.substring(4).toInt());
    }
    else if (command.startsWith("STATUS:")) {
      setStatusLed(command.substring(7).toInt());
    }
    else if (command.startsWith("RGB:")) {
      setRgbLed(command);
    }
  }
}

void sendLedStates() {
  Serial.print("STATES:");
  Serial.print(leds.mainLed ? "1" : "0");
  Serial.print(",");
  Serial.print(leds.statusLed ? "1" : "0");
  Serial.print(",");
  Serial.print(leds.rgb[0]);
  Serial.print(",");
  Serial.print(leds.rgb[1]);
  Serial.print(",");
  Serial.println(leds.rgb[2]);
}

void setMainLed(int state) {
  leds.mainLed = (state == 1);
  digitalWrite(LED_PIN, leds.mainLed ? HIGH : LOW);
}

void setStatusLed(int state) {
  leds.statusLed = (state == 1);
  digitalWrite(STATUS_LED_PIN, leds.statusLed ? HIGH : LOW);
}

void setRgbLed(String command) {
  for (int i = 0; i < 3; i++) {
    leds.rgb[i] = getValue(command, ':', i + 1).toInt();
  }
  analogWrite(RGB_RED_PIN, leds.rgb[0]);
  analogWrite(RGB_GREEN_PIN, leds.rgb[1]);
  analogWrite(RGB_BLUE_PIN, leds.rgb[2]);
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}