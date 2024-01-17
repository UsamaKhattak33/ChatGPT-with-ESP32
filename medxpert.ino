////////////////////////
unsigned long startTime; // Variable to store the start time
// the setup routine runs once when you press reset:
void setup() {
    // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  startTime = millis(); // Record the start time
}

// the loop routine runs over and over again forever:
void loop() {
  if (millis() - startTime < 60000) { // Check if 15 seconds have not passed
     // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  float Volt = sensorValue*5.0/1024.0;
  Serial.print("Sensor Reading: ");
  Serial.print(Volt);
  Serial.print("V");
  float BGL = (158.12 * Volt) - 519.35;
  Serial.print("\t\tBGL: ");
  Serial.print(BGL);
  Serial.print("mg/dL");
  Serial.println();
  delay(2000);        // delay in between reads for stability
  }
  else {
    Serial.println("Time limit reached!");
    delay(1000); // Delay for 1 second to allow the last message to be displayed
    while (true) {} // Enter an infinite loop to stop further execution
  }
}
