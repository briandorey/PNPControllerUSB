#include <Wire.h>
#include <AccelStepper.h>

// i2c def
#define  SLAVE_ADDRESS         0x14  
#define  REG_MAP_SIZE             14
#define  MAX_SENT_BYTES       1


byte registerMap[REG_MAP_SIZE];
byte registerMapTemp[REG_MAP_SIZE - 1];
byte receivedCommands[MAX_SENT_BYTES];

// stepper control
// feeder location variables
long feederArray[16]={
  0,
  -106.6666666667,
  -213.3333333333,
  -320,
  -426.6666666667,
  -533.3333333333,
  -640,
  -746.6666666667,
  -853.3333333333,
  -960,
  -1066.6666666667,
  -1173.3333333333,
  -1280,
  -1386.6666666667,
  -1493.3333333333,
  -1600
};


long xcurrentpos = 0;
byte currentFeeder = 0;
byte previousFeeder = 0;
int sensorValue = 0;
int zHeight = -700; //-5600;

byte pickaftermove = 0;


// Arduino pin mapping for stepper and IO control
#define relayZPin1 5
#define relayZPin2 6
#define stopZPin 2
#define dirXPin 4
#define stepXPin 3
#define stopXPin 7
#define pickerRunning A3

//AccelStepper stepperX(1,stepXPin,dirXPin); 
AccelStepper stepperX(AccelStepper::DRIVER, stepXPin, dirXPin);

void setup()
{
  
  // configure i2c inputs
  Wire.begin(SLAVE_ADDRESS); 
  //Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

// serial for debugging 
  //Serial.begin(9600);

  // configure stepper drivers and air valve
  pinMode(relayZPin1, OUTPUT);
  pinMode(relayZPin2, OUTPUT);
  pinMode(stopZPin, INPUT);
  pinMode(dirXPin, OUTPUT);
  pinMode(stepXPin, OUTPUT);
  pinMode(stopXPin, INPUT);
  pinMode(pickerRunning, OUTPUT);
  
 
  digitalWrite(stopZPin, HIGH);
  digitalWrite(stopXPin, LOW);
  stepperX.setMaxSpeed(15000);
  stepperX.setAcceleration(25000);

  //stepperZ.setCurrentPosition(0);
  stepperX.setCurrentPosition(0);
 

  while (!Zero());
}

void loop()
{
  
    stepperX.run();
    stepperX.run();
    stepperX.run();
    stepperX.run();
    stepperX.run();
    stepperX.run();
    stepperX.run();
    stepperX.run();
    stepperX.run();
    stepperX.run();
    
    
    if (stepperX.distanceToGo() != 0){
        digitalWrite(pickerRunning, HIGH); // signal to host that picker is runnning
    }
    else{

        if (pickaftermove == 1){
          digitalWrite(relayZPin1, LOW); // set solenoid relay 1 to on
          digitalWrite(relayZPin2, HIGH); // set solenoid relay 1 to off
          pickaftermove = 0;
        }
        digitalWrite(pickerRunning, LOW); // signal to host that picker is runnning
    }
}

boolean Zero() {
  //Serial.write("zeroing");
  // disable picker ram
  digitalWrite(relayZPin1, HIGH); // set solenoid relay 1 to off
  digitalWrite(relayZPin2, LOW); // set solenoid relay 1 to on
  
  digitalWrite(pickerRunning, HIGH); // signal to host that picker is runnning
  //sensorValue = digitalRead(stopZPin); 
  while (digitalRead(stopZPin)) {
    delayMicroseconds(4000);
    //sensorValue = analogRead(stopZPin);  
  }

  digitalWrite(dirXPin, LOW);
  //sensorValue = analogRead(stopXPin);    

  while (!digitalRead(stopXPin)) {
    digitalWrite(stepXPin, HIGH);
    delayMicroseconds(100); 
    digitalWrite(stepXPin, LOW);
    delayMicroseconds(2000);
    //sensorValue = analogRead(stopXPin); 
  }
  delayMicroseconds(250);
  sensorValue = 0;
  currentFeeder = 0;
  
  digitalWrite(pickerRunning, LOW); // signal to host that picker is stopped
  
  //Serial.write("finished zeroing");
  return true; 
}

void calcXPosition(){
  //Serial.write("moving to feeder");
  
  
  
  
  // disable picker ram
  digitalWrite(relayZPin1, HIGH); // set solenoid relay 1 to off
  digitalWrite(relayZPin2, LOW); // set solenoid relay 1 to on
  while (digitalRead(stopZPin)) {
    delay(100);
  }
  
  
  if (currentFeeder == previousFeeder){
    // enable picker ram
    digitalWrite(relayZPin1, LOW); // set solenoid relay 1 to on
    digitalWrite(relayZPin2, HIGH); // set solenoid relay 1 to off
  }
  
  else{
    long travel = feederArray[currentFeeder];
    
    travel = travel *-1;
    //Serial.print(travel);
    
    stepperX.moveTo(travel);
    delay(100);
    pickaftermove = 1;
    

  }
  previousFeeder = currentFeeder;
}

// i2c code

void requestEvent(){
  //Wire.write(registerMap, REG_MAP_SIZE);  //Set the buffer up to send all 14 bytes of data
  //Wire.write( (const byte *) "a", 1);
  //Serial.write("bytes requested");
}



void receiveEvent(int bytesReceived)
{
  //Serial.write("bytes received");
  ///char c;
  //while(1 < Wire.available()) // loop through all but the last
  //{
  //  c = Wire.read(); // receive byte as a character
   // Serial.print(c);         // print the character
  //}
  int x = 99;
  
  while (Wire.available())
{
  x = Wire.read();    // receive byte as an integer
  //Serial.println(x);         // print the integer
}
  
  
  
  switch(x){
    // pick component commands
  case 0:
    currentFeeder = 0;
    calcXPosition();
    //return; 
    break;
  case 1:
    currentFeeder = 1;
    calcXPosition();
    //return; 
    break;
  case 2:
    currentFeeder = 2;
    calcXPosition();
    //return; 
    break;
  case 3:
    currentFeeder = 3;
    calcXPosition();
    //return; 
    break;
  case 4:
    currentFeeder = 4;
    calcXPosition();
    //return; 
    break;
  case 5:
    currentFeeder = 5;
    calcXPosition();
    //return; 
    break;
  case 6:
    currentFeeder = 6;
    calcXPosition();
    //return; 
    break;
  case 7:
    currentFeeder = 7;
    calcXPosition();
    //return; 
    break;
  case 8:
    currentFeeder = 8;
    calcXPosition();
    //return; 
    break;
  case 9:
    currentFeeder = 9;
    calcXPosition();
    //return; 
    break;
  case 10:// int 10
    currentFeeder = 10;
    calcXPosition();
    //return; 
    break;
  case 11:// int 11
    currentFeeder = 11;
    calcXPosition();
    //return; 
    break;
  case 12:// int 12
    currentFeeder = 12;
    calcXPosition();
    //return; 
    break;
  case 13:// int 13
    currentFeeder = 13;
    calcXPosition();
    //return; 
    break;
  case 14:// int 14
    currentFeeder = 14;
    calcXPosition();
    //return; 
    break;
  case 15: // int 15
    currentFeeder = 15;
    calcXPosition();
    //return; 
    break;
    // reset commands
  case 70: // int 70
    // reset picker
    while (!Zero());
    currentFeeder = 0;
    previousFeeder = 0;
    stepperX.setCurrentPosition(0);
    //return; 
    break;
  case 80: // int 80
    // reset picker z axis, turn off ram
    digitalWrite(relayZPin1, HIGH); // set solenoid relay 1 to off
    digitalWrite(relayZPin2, LOW); // set solenoid relay 1 to on
    //return; 
    break;
    
  default:
    break;
    //return; // ignore the commands and return

  }
  
}



