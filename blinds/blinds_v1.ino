#include <Servo.h>

int servoPin = 9;
int photocellPin = A1;
int buttonPin = 5;

int pos = 0;
int spd = 50;

int timeSinceWater;

Servo myServo;
int photocellReading;
int buttonVal;
int state = 0;
int prevState = 0;
int dest = 0;

int timeBase = 0;
int timeNow;
int timeDiff;
int buttonPos = 1;
int timeThreshold = 5000;

int debug = 1;

void setup() {
  Serial.begin(9600);

  myServo.attach(servoPin);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  photocellReading = analogRead(photocellPin);

  if (debug) {
    Serial.print("Light Reading: ");
    Serial.print(photocellReading);
    Serial.print(" | Pos: ");
    Serial.print(pos);
    Serial.print(" | State: ");
  }

  if (photocellReading > 0 && photocellReading < 200) {
    debug and Serial.println("Night");
    dest = 170;
    state = 1;
  } else if (photocellReading > 210 && photocellReading < 400) {
    debug and Serial.println("Dusk");
    dest = 135;
    state = 2;
  } else if (photocellReading > 410 && photocellReading < 840) {
    debug and Serial.println("Day");
    dest = 105;
    state = 3;
  } else if (photocellReading > 850 && photocellReading < 925) {
    debug and Serial.println("Bright Day");
    dest = 20;
    state = 4;
  } else if (photocellReading > 1030) {
    debug and Serial.println("Not in range");
  }

  timeNow = millis();

  buttonPress();

  timeDiff = timeNow - timeBase;
  if (timeDiff < 0) {
    timeDiff = 0;
  }
  debug and Serial.print("timeDiff: ");
  debug and Serial.println(timeDiff);

  debug and Serial.print("timeDiff (");
  debug and Serial.print(timeDiff);
  debug and Serial.print(") > timeThreshold (");
  debug and Serial.print(timeThreshold);
  debug and Serial.print("): ");
  debug and Serial.println(timeDiff > timeThreshold);

  if ((state != prevState) && (timeDiff > timeThreshold)) {
    debug and Serial.print("State change to: ");
    debug and Serial.println(state);

    ldrPress();
    
    prevState = state;
    timeBase = millis();
  }
  //Serial.write(state);
}

void ldrPress() {
  debug and Serial.println("State change");

  myServo.attach(servoPin);
  if (debug) {
    Serial.print("Pos: ");
    Serial.print(pos);
    Serial.print("| Dest: ");
    Serial.println(dest);
  }
  
  if (pos > dest) {
    while (pos > dest) {
      debug and Serial.print("Was: ");
      debug and Serial.print(pos);
      myServo.write(pos);
      delay(spd);
      pos--;
    
      debug and Serial.print(" | Is: ");
      debug and Serial.println(pos);
    }
  } else {
    while (pos < dest) {
      debug and Serial.print("Was: ");
      debug and Serial.print(pos);
      myServo.write(pos);
      delay(spd);
      pos++;
    
      debug and Serial.print(" | Is: ");
      debug and Serial.println(pos);
    }
    myServo.detach();
  }
}

void buttonPress() {
  buttonVal = digitalRead(buttonPin);
  debug and Serial.println(buttonVal);

  if (buttonVal == LOW) {
    spd = 20;
    debug and Serial.println("button pressed");

    if (pos <= 20) {
      dest = 105;
      buttonPos = 0;
    } else if (pos <= 105 && buttonPos == 0) {
      dest = 135;
    } else if (pos <= 135 && buttonPos == 0) {
      dest = 179;
    } 
    
    else if (pos <= 105 && buttonPos == 1) {
      dest = 20;
    } else if (pos <= 135 && buttonPos == 1) {
      dest = 105;
    } else if (pos <= 180) {
      dest = 135;
      buttonPos = 1;
    }

    debug and Serial.print(dest);
    debug and Serial.println(" Degress");
    ldrPress();
  }
  
  spd = 50;
}

void serialEvent() {
  int inByte = Serial.read();
  debug and Serial.print("InByte: ");
  debug and Serial.println(inByte);
  // if plant has recently been watered, don't close the blinds
  if (inByte == 1) {
    timeBase = millis();
    debug and Serial.println("Recently watered");
  }
}
