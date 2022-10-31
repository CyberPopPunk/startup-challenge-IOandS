int leftSensorPin = A5;
int leftSensorVal = 0;

int rightSensorPin = A4;
int rightSensorVal = 0;

int LED_PIN = 3;

int newHeatLevel = 0;
int currentHeatLevel = 0;
int previousSeatState = 0;
int currentSeatState = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
  pinMode(LED_PIN, HIGH);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //DEBUG CODE
  leftSensorVal = analogRead(leftSensorPin);
  Serial.print(leftSensorVal);
  rightSensorVal = analogRead(rightSensorPin);
  Serial.print("     ");
  Serial.println(rightSensorVal);
  Serial.print("Previous Seat Occupancy: ");
  Serial.print(previousSeatState);


  // check if seat is occupied
  currentSeatState = seatOccupancy(leftSensorPin, rightSensorPin);


  if (previousSeatState == 0 && currentSeatState == 0) {
    // no one is here, heat off
    newHeatLevel = 0;
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 0 && currentSeatState == 2) {
    // if someone is seated currently, turn the heat all the way on
    newHeatLevel = 100;
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 0 && currentSeatState == 1) {
    // if someone is shuffling, or partially seated, turn heat halfway on
    newHeatLevel = 75;
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 1 && currentSeatState == 0) {
    // user left seat, turn heat off
    newHeatLevel = 0;
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 1 && currentSeatState == 1) {
    // same state
    if (newHeatLevel >= 40) {
      newHeatLevel -= 25;
    } else {
      newHeatLevel = 15;
    }
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 1 && currentSeatState == 2) {
    // from partially to fully seated, turn heat up
    newHeatLevel = 100;
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 2 && currentSeatState == 0) {
    // user got up, turn heat off
    newHeatLevel = 0;
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 2 && currentSeatState == 1) {
    //need to adjust based on how long/previous seating level
    newHeatLevel -= 15;
    updateHeat(newHeatLevel);
  } else if (previousSeatState == 2 && currentSeatState == 2) {
    // same state
    newHeatLevel = 100;
    updateHeat(newHeatLevel);
  } else {
    Serial.print("this is a big problem and shouldn't bhappen!");
  }
  //DEBUG Time
  //delay(1000);

  //actual time
  //delay(5000);
  previousSeatState = currentSeatState;
}

int seatOccupancy(int sensorPinLeft, int sensorPinRight) {
  // INPUT: two pins to read as analog sensors
  //
  // FUNCTION
  // takes two analog FSR sensor Pins meant to be mounted on a chair as input.Samples over 2.5 seconds,
  // Maps Values to a scale from 0-100
  // Averages their values
  //
  // OUTPUT
  // int of 0-2 based on if the chair is vacant (0), partially seated (1), or occupied (2).

  int checkStart = millis();

  int partialThreshold = 20;
  int fullThreshold = 60;

  int NUM_SAMPLES = 50;
  long leftSamples = 0;
  long rightSamples = 0;
  long totalSamplesAvg = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    leftSamples += analogRead(sensorPinLeft);
    rightSamples += analogRead(sensorPinRight);
    delay(50);
  }

  totalSamplesAvg = (rightSamples + leftSamples);
  totalSamplesAvg = totalSamplesAvg / (NUM_SAMPLES * 2);

  totalSamplesAvg = map(totalSamplesAvg, 0, 1023, 0, 100);
  //Serial.print("Total Sample Mapped: ");
  //Serial.println(totalSamplesAvg);

  if (totalSamplesAvg > fullThreshold) {
    //seat is fully occupied
    return 2;
  } else if (totalSamplesAvg > partialThreshold) {
    // seat is partially occupied/shuffling occurs
    return 1;
  } else {
    //seat is vacant
    return 0;
  }
}

void updateHeat(int tempHeatLevel) {
  // Maps heat level from 0-100 scale to 0-255
  // output to LED
  tempHeatLevel = map(tempHeatLevel, 0, 100, 0, 255);
  analogWrite(LED_PIN, tempHeatLevel);
}