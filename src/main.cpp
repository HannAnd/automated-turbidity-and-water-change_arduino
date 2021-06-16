// see README for full description

//to do:
//create variables that accept the turbid/clearwater ratios


// needed when dealing with Arduino code outside of the Arduino IDE:
# include <Arduino.h>

// defining the analog pins for the sensors:
// ETAPE measure water level, TURBID measure turbidity:
# define ETAPE1 A0
# define ETAPE2 A1
# define ETAPE3 A2
# define TURBID1 A3
# define TURBID2 A4
# define TURBID3 A5

// specifying the number of samples to be averaged to smooth (average)
// noisy sensor readings:
# define NUMSAMPLES 5
# define LEVELSAMPLES 3

// handing the sample number to the sample variable:
int samples_t[NUMSAMPLES];
int samples_w[LEVELSAMPLES];

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
void serialparse(int command) {
  // creates an unsigned integer for use as a counter:
  uint8_t i;
  // declaring variables used in serialparse:
  float ave_turb;
  float ave_level;
  float voltage_t;
  //I am not entirely sure yet that this function is properly retrieving these
  //variable values from the main loop()
  float clearwater1;
  float blackwater1;
  float clearwater2;
  float blackwater2;
  float clearwater3;
  float blackwater3;

  // all commands sent to a switch-case must be integers, no strings:
  switch(command) {

  // cases 1 - read all turbidity sensors:
  // cases 2-n - initiating water changes:
    // read turbidity sensors:
    case 1:
    //// reading turbidity sensor 1:
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
    
    // short delay, then sending Pi the signal Arduino is ready for water changes:
    delay(100);
    Serial.println("r~NA~NA");
    break;

    // begining water change for tank chamber 1:
    case 2:
    analogRead(ETAPE1);  // read to align multiplexer to A0:
    delay(10);
    analogRead(ETAPE1);  // unused reading to deal with ADC lag:
    delay(50);
    String waterdown = "yes";
    String clearup = "no";
    String blackup = "no";
    // draining the water until the appropriate water level is reached:
    while (waterdown = "yes") {
      digitalWrite(exfill1, LOW);
      samples_w[LEVELSAMPLES] = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        samples_w[i] = analogRead(ETAPE1);
        delay(10);
      }
      ave_level = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        ave_level += samples_w[i];
      }
      ave_level /= LEVELSAMPLES;
      // calibrated to drop the water to a height of ~18 cm:
        // calibrated values: 120.44-120.76:
      if (ave_level >= 120.50) {  
        digitalWrite(exfill1, HIGH);
        waterdown = "no";
        clearup = "yes";
      }
    }
    // adding clearwater until the value sent by Pi is reached:
    while (clearup = "yes") {
      digitalWrite(clearpump1, LOW);
      samples_w[LEVELSAMPLES] = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        samples_w[i] = analogRead(ETAPE1);
        delay(10);
      }
      ave_level = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        ave_level += samples_w[i];
      }
      ave_level /= LEVELSAMPLES;
      if (ave_level >= clearwater1) {
        digitalWrite(clearpump1, HIGH);
        clearup = "no";
        blackup = "yes";
      }
    }
    // adding blackwater until the maximum water amount is reached:
    // max water height is calibrated to ~25 cm:
      // calibrated values: 95.08-95.74:
    while (blackup = "yes") {
      digitalWrite(blackpump1, LOW);
      samples_w[LEVELSAMPLES] = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        samples_w[i] = analogRead(ETAPE1);
        delay(10);
      }
      ave_level = 0;
      for (i=0; i< LEVELSAMPLES; i++) {
        ave_level += samples_w[i];
      }
      ave_level /= LEVELSAMPLES;
      if (ave_level >= 95.10) {
        digitalWrite(blackpump1, HIGH);
        blackup = "no";
        Serial.println("w~1~NA");
      }
    }
  break;

  //cases 3 & 4 woud be regular water changes for the other chambers
  //I'm thinking cases 5, 6 & 7 could be water changes for clearwater-only tanks
    //so like, half the tank's clearwater with one pump and half from another?
  //then case 8 would be the observation period water drop 
  }
}

// runs repeatedly:
void loop() {
  int command;
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
    // the "+1" ensures the "~" is not included in the message
    String message = pi_out.substring(pi_out.indexOf("~") + 1);

    /// parsing the Pi output based on the signpost:
    // signpost "a" prefaces commands to start cases in the switch-case:
    if (signpost.equals("a")) {
      // converting the Serial string to integer for use by the switch-case:
      int command = message.toInt();
    }
    // signpost "1" indicates amount of clearwater to add to tank chamber 1:
    if (signpost.equals("1")) {
      float clearwater1 = message.toFloat();
    }
    // signpost "2" indicates amount of clearwater to add to tank chamber 2:
    if (signpost.equals("2")) {
      float clearwater2 = message.toFloat();
    }
    // signpost "3" indicates amoung of clearwater to add to tank chamber 3:
    if (signpost.equals("3")) {
      float clearwater3 = message.toFloat();
    }
  }

    // runs the response function coded above:
    serialparse(command);

}
