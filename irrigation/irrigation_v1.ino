const int relayPin = 2;
const int moisturePin = A0;
const int threshold = 500;

int windowState;  // 20 is full open, 180 is closed
int thresholdMod = 0; // the more closed the window is, the drier the soil must be to require watering

int timeNow;
int timeLast;
int timeDiff;

int relayVal;

int debug = 0;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
}

void loop() {
  int value = analogRead(moisturePin); // read the analog value from sensor
  int timeNow = millis();

  if (windowState == 170) {
    value = threshold;    // don't water while window is closed
  } else {
    value = value - (windowState - 20);   // less likely to water the more closed the window is
  }
  
  if (value < threshold) {
    debug and Serial.print("The soil is DRY => turn pump ON");
    digitalWrite(relayPin, HIGH);
  } else {
    debug and Serial.print("The soil is WET => turn pump OFF");
    digitalWrite(relayPin, LOW);
  }
  relayVal = digitalRead(relayPin);
  Serial.write(relayVal);

  debug and Serial.print(" (");
  debug and Serial.print(value);
  debug and Serial.println(")");

  delay(5);
}
