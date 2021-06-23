// see README for full description

//to do:
//create variables that accept the turbid/clearwater ratios


// needed when dealing with Arduino code outside of the Arduino IDE:
# include <Arduino.h>

// defining the analog pins for the sensors:
// ETAPE measure water level, TURBID measure turbidity:
# define ETAPE1 A0
# define ETAPE2 A2
# define ETAPE3 A4
# define TURBID1 A1
# define TURBID2 A3
# define TURBID3 A5

// value of the resistor on the eTape sensors:
# define RESISTOR 560

// specifying the number of samples to be averaged to smooth (average)
// noisy sensor readings:
# define NUMSAMPLES 5
# define LEVELSAMPLES 5

// handing the sample number to the sample variable:
int samples_t[NUMSAMPLES];
int samples_w[LEVELSAMPLES];

// counts clearwater up values received from Pi:
int ready = 0;

// defining the digital pins for each relay channel:
int exfill1  = 2;    // 1-channel relay, chamber 1 exfill pump:
int exfill2 = 3;     // channel 1 of 8, chamber 2 exfill pump:
int exfill3 = 4;     // channel 2 of 8, chamber 3 exfill pump:
int clearpump1 = 5;     // channel 3 of 8, chamber 1 clear infill pump:
int blackpump1 = 6;     // channel 4 of 8, chamber 1 black infill pump:
int clearpump2 = 7;     // channel 5 of 8, chamber 2 clear infill pump:
int blackpump2 = 8;     // channel 6 of 8, chamber 2 black infill pump:
int clearpump3 = 9;     // channel 7 of 8, chamber 3 clear infill pump:
int blackpump3 = 10;    // channel 8 of 8, chamber 3 black infill pump:


// runs once on start up:
void setup() {
  // opens a port that samples the standard 9600 bits per second:
  Serial.begin(9600);

  // AREF hooked into 5V power to reduce sensor noise:
  analogReference(EXTERNAL);

  // sets up relay channels to receive commands:
  pinMode(exfill1, OUTPUT);
  pinMode(exfill2, OUTPUT);
  pinMode(exfill3, OUTPUT);
  pinMode(clearpump1, OUTPUT);
  pinMode(blackpump1, OUTPUT);
  pinMode(clearpump2, OUTPUT);
  pinMode(blackpump2, OUTPUT);
  pinMode(clearpump3, OUTPUT);
  pinMode(blackpump3, OUTPUT);

  // relays are active-low, starting with all channels closed:
  digitalWrite(exfill1, HIGH);
  digitalWrite(exfill2, HIGH);
  digitalWrite(exfill3, HIGH);
  digitalWrite(clearpump1, HIGH);
  digitalWrite(blackpump1, HIGH);
  digitalWrite(clearpump2, HIGH);
  digitalWrite(blackpump2, HIGH);
  digitalWrite(clearpump3, HIGH);
  digitalWrite(blackpump3, HIGH);
}

// function for responding to Raspberry Pi commands:
void turbidread() {
  // creates an unsigned integer for use as a counter:
  uint8_t i;
  // declaring variables used in turbidread():
  float ave_turb;
  float voltage_t;

  //// reading turbidity sensor 1:
  Serial.println("check~NA~reading_turbidity");  // telling Pi case 1 has been activated:
  samples_t[NUMSAMPLES] = 0;
  analogRead(TURBID1);  // read to align multiplexer to A3:
  delay(10);
  analogRead(TURBID1);  // unused reading to deal with ADC lag:
  delay(50);
  // collecting sensor readings to average:
  for (i=0; i< NUMSAMPLES; i++)  {
    samples_t[i] = analogRead(TURBID1);
    delay(50);
  }

  // resetting the average variable between iterations:
  ave_turb = 0;
  // averaging the above samples:
  for (i=0; i< NUMSAMPLES; i++)  {
    ave_turb += samples_t[i];
  }
  ave_turb /= NUMSAMPLES;

  // converting analog readings (which go from 0-1023) to voltages (0-5v):
  voltage_t = ave_turb*(5.0/1023.0);  //might leave analog readings as-is and learn to just convert NTU from that
  // sending voltage readings through Serial to the Pi:
  // characters before ~ interpreted by Pi parser to to understand this
  // value is a turbidity reading and which chamber it's from:
  Serial.print("t~1~");
  Serial.println(voltage_t);

  //// reading turbidity sensor 2:
  samples_t[NUMSAMPLES] = 0;
  analogRead(TURBID2);  // read to align multiplexer to A4:
  delay(10);
  analogRead(TURBID2);
  delay(50);
  for (i=0; i< NUMSAMPLES; i++)  {
    samples_t[i] = analogRead(TURBID2);
    delay(50);
  }
  ave_turb = 0;
  for (i=0; i< NUMSAMPLES; i++)  {
    ave_turb += samples_t[i];
  }
  ave_turb /= NUMSAMPLES;
  voltage_t = ave_turb*(5.0/1023.0);  //might leave analog readings as-is and learn to just convert NTU from that
  Serial.print("t~2~");
  Serial.println(voltage_t);

  //// reading turbidity sensor 3:
  samples_t[NUMSAMPLES] = 0;
  analogRead(TURBID3);  // read to align multiplexer to A5:
  delay(10);
  analogRead(TURBID3);
  delay(50);
  for (i=0; i< NUMSAMPLES; i++)  {
    samples_t[i] = analogRead(TURBID3);
    delay(50);
  }
  ave_turb = 0;
  for (i=0; i< NUMSAMPLES; i++)  {
    ave_turb += samples_t[i];
  }
  ave_turb /= NUMSAMPLES;
  voltage_t = ave_turb*(5.0/1023.0);  //might leave analog readings as-is and learn to just convert NTU from that
  Serial.print("t~3~");
  Serial.println(voltage_t);
}


// function for initiating water changes
void waterchange(int changetype, float clearwater1, float clearwater2, float clearwater3) {
  // creates an unsigned integer for use as a counter:
  uint8_t i;
  // declaring variables used in waterchange():
  float ave_level;

  Serial.print("check~1~in_waterchange(): ");
  Serial.println(clearwater1);
  Serial.print("check~2~in_waterchange(): ");
  Serial.println(clearwater2);
  Serial.print("check~3~in_waterchange(): ");
  Serial.println(clearwater3);
  
  //I'm thinking cases 1-3 will be normal water chages,
  //cases 5-6 will be clearwater-only tanks
  //and case 7 will be the filming period water drop:
  switch(changetype) {
    case 1:
    Serial.println("check~1~water_change");  // status readout for Pi:
    Serial.print("check~1~in_case1(): ");
    Serial.println(clearwater1);
    Serial.print("check~2~in_case1(): ");
    Serial.println(clearwater2);
    Serial.print("check~3~in_case1(): ");
    Serial.println(clearwater3);
    analogRead(ETAPE3);  // read to align multiplexer to A0:
    delay(10);
    analogRead(ETAPE3);  // unused reading to deal with ADC lag:
    delay(50);
    String waterdown = "yes";
    String clearup = "no";
    String blackup = "no";
    String writeonce = "on";
    // draining the water until the appropriate water level is reached:
    while (waterdown == "yes") {
      // exfill pump on:
      digitalWrite(exfill1, LOW);
      samples_w[LEVELSAMPLES] = 0;
      // collecting multiple readings to stabilize noise:
      for (i=0; i< LEVELSAMPLES; i++) {
        samples_w[i] = analogRead(ETAPE3);
        delay(10);
      }
      // averaging readings to stabilize noise:
      ave_level = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        ave_level += samples_w[i];
      }
      ave_level /= LEVELSAMPLES;
      //exclusively for testing purposes, do not leave in code permanently
      //Serial.print("check~1~");
      //Serial.println(ave_level);
      // calibrated to drop the water to a height of ~18 cm:
      // must be calibrated for each sensor individually!
        // calibrated values: 660.8-661.8:
      if (ave_level >= 661) {  
        digitalWrite(exfill1, HIGH);
        Serial.println("check~1~waterdown_reached");  // status readout for Pi:
        // starting next stage of waterchange:
        waterdown = "no";
        clearup = "yes";
        delay(50);
      }
    }
    // adding clearwater until the value sent by Pi is reached:
    while (clearup == "yes") {
      if (writeonce == "on") {
        Serial.print("check~1~clearup_started");
        Serial.println(clearwater1);
        writeonce = "off";
      }
      digitalWrite(clearpump1, LOW);
      samples_w[LEVELSAMPLES] = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        samples_w[i] = analogRead(ETAPE3);
        delay(10);
      }
      ave_level = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        ave_level += samples_w[i];
      }
      ave_level /= LEVELSAMPLES;
      //Serial.print("check~1~");
      //Serial.println(ave_level);
      if (ave_level <= clearwater1) {
        Serial.println("check~1~clearup_reached");
        digitalWrite(clearpump1, HIGH);
        clearup = "no";
        blackup = "yes";
        writeonce = "on";
        delay(50);
      }
    }
    // adding blackwater until the maximum water amount is reached:
    // max water height is calibrated to ~25 cm:
      // calibrated values: 561.4-562:
    while (blackup == "yes") {
      if (writeonce == "on") {
        Serial.println("check~1~blackup_started");
        writeonce = "off";
      }
      digitalWrite(blackpump1, LOW);
      samples_w[LEVELSAMPLES] = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        samples_w[i] = analogRead(ETAPE3);
        delay(10);
      }
      ave_level = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        ave_level += samples_w[i];
      }
      ave_level /= LEVELSAMPLES;
      if (ave_level <= 561.6) {
        Serial.println("check~1~blackup_reached");
        digitalWrite(blackpump1, HIGH);
        blackup = "no";
        Serial.println("w~1~NA");
        delay(50);
      }
    }
    break;
  }
}

// runs repeatedly:
void loop() {
  int command;
  int signpost;
  String message;
  String clear1;
  String clear2;
  String clear3;
  float clearwater1;
  float clearwater2;
  float clearwater3;

  // checking if Raspberry Pi has sent anything through Serial:
  if (Serial.available() > 0) {
    // saving Pi command:
    // reads until the end of the line:
    String pi_out = Serial.readStringUntil('\n');
    // removing any excess whitespace the Pi might have sent with the command:
    pi_out.trim();


    //// parsing messages coming from Pi:

    // divides Pi messages into a signpost that defines the message type
    // (everything before the "~")
    // and the main body of the message (everything after the "~"):
    String signpost = pi_out.substring(0, pi_out.indexOf("~"));
    // the "+1" ensures the "~" is not included in the message:
    String message = pi_out.substring(pi_out.indexOf("~") + 1);
    // received command readout for Pi:
    Serial.print("check~NA~received:");
    Serial.print(signpost);
    Serial.print(";");
    Serial.println(message);

    // starting function to gather turbidity readings:
    if (signpost.equals("1")) {
      turbidread();
    }

    // accepting clear water proportions from Pi for chamber 1:
    if (signpost.equals("2")) {
      float clearwater1 = message.toFloat();
      Serial.print("check~1~chamber1_clear:");
      Serial.println(clearwater1);  //troubleshooting- values correct here:
      ready = ready + 1;
      // alerts Pi the Arduino is ready for waterchanges only if
      // the Arduino has received all three clearwater values:
      if (ready == 3) {
        Serial.println("r~NA~ready_for_waterchanges");
      }
    }

    // accepting clear water proportions from Pi for chamber 2:
    if (signpost.equals("3")) {
      float clearwater2 = message.toFloat();
      Serial.print("check~2~chamber2_clear:");
      Serial.println(clearwater2);  //*troubleshooting- values correct here:*
      ready = ready + 1;
      if (ready == 3) {
        Serial.println("r~NA~ready_for_waterchanges");
      }
    }

    // accepting clear water proportions from Pi for chamber 3:
    if (signpost.equals("4")) {
      float clearwater3 = message.toFloat();
      Serial.print("check~3~chamber3_clear:");
      Serial.println(clearwater3);  //*troubleshooting- values correct here:*
      ready = ready + 1;
      if (ready == 3) {
        Serial.println("r~NA~ready_for_waterchanges");
      }
    }

    // beginning water changes:
    if (signpost.equals("5") and ready == 3) {
      // changetype will be used in the waterchange() switch-case to initiate
      // different types of waterchanges (or the filming water drop) for the
      // different tank chambers:
      int changetype = message.toInt();
      //*troubleshooting: values are now 0.00:*
      Serial.print("check~1~clearwater1: ");
      Serial.println(clearwater1);
      Serial.print("check~2~clearwater2: ");
      Serial.println(clearwater2);
      Serial.print("check~3~clearwater3: ");
      Serial.println(clearwater3);
      waterchange(changetype, clearwater1, clearwater2, clearwater3);
    }
  }
}

//notes: make sure you are calling on the right eTape when testing
//remember to change waterchange() ETAPE values to correct ones for final version

//once this is piloted add a failsafe where if a relay is on for too long (pilot water change time)
//then it automatically turns off (and sends me a message?)
