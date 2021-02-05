const int hum_pin = A5;
const int lux_pin=A0;
const int temp_pin=A2;
const int pin_pump=4; //digital pin 4 to command the water pump
const int pin_heater=8; //digital pin 8 to command the heater


//time in the Arduino, can be changed 
int heures = 0; //par défaut, 0
int minutes = 0; // par défaut, 0
int secondes = 0; //par défaut,0 

int sensorValue = 0;
const int N = 100 ;
float lux_array[N];
float temp_array[N];
float moisture_array[N];

////// the pump control //////////
int auto_pump = 1; 
/*  
by default, the pump is controlled automatically. We can change the mode :
0 : entirely manual 
1 : automatic with hygrometrie 
2 : automatic cyclic 
*/

long unsigned int delta_pump = 10 * 60000; // how long should we wait before watering the plant once more (in ms)
long int last_pump =  -delta_pump; //the last time plants were watered. The initial value being 0, the hygrometry value will hhave timle to stabilise 
int time_pump = 1000; //how long should we pump each time (ms)
int lim_hygro = 300; // the inferior limit of acceptable hygrometry of soil 
//////


int start =0;
int s = 0;

float current_moisture;
float current_temp;
float current_lux;
float voltage;
float reading;
float lux;

float MinT=16;
float MaxT=25;

long int dt = 1000;
long int last_display;
long int current_time;
/////////////////////////////////////////////////
//              Lux                            //
/////////////////////////////////////////////////

/////////////////////////////////////////////////
//   Fin lux                                   // 
/////////////////////////////////////////////////



/////////////////////////////////////////////////
//   What is done in this script : we try to   //
// detect the mouvement in front of the sensor //
/////////////////////////////////////////////////
#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 1000; // How frequently are we going to send out a ping (in milliseconds).
unsigned long pingTimer;     // Holds the next ping time.
unsigned int tolerance = 10 ;  // The tolerance in the variation of distance mesured by the sensor
unsigned int previous_distance; //the last ditance recorded y the sensor, should be in cm
bool mouvement = true; // when we plug the greenhouse, we imagine hat someone is next to it... 


void echoCheck() { 
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    if (abs(previous_distance - sonar.ping_result/US_ROUNDTRIP_CM) > tolerance){
      // this means there is significant change in the distance 
      previous_distance = sonar.ping_result / US_ROUNDTRIP_CM; 
      mouvement = true;
    }
    else{
      mouvement = false; 
    }
  }
  }
/////////////////////////////////////////////////
//            end mouvmeent                    //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
//            pumping strategy                 //
/////////////////////////////////////////////////
/*
Given the fact that the pump has a very important water flow, 
and the fact that the moisture sensor won't be directly under the pipe, 
we adopt the folowing strategy :
if (at some point the grass is to dry, we pump water for 1 second. Then we wait 10 minutes. 
And 10 minutes later, we check again if the soil is too dry etc. 
*/

void pump(){
  /*When this function is called, the water pump flows for 500 ms */
  digitalWrite(pin_pump, 0);
  digitalWrite(pin_pump, 1);
  Serial.println("pump on");
  delay(time_pump); 
  digitalWrite(pin_pump, 0);  
  Serial.println("pump off");
  last_pump = millis();
}


/////////////////////////////////////////////////
//              end pumping                    //
/////////////////////////////////////////////////


float get_temp () {
  reading = analogRead(temp_pin);
  //reading = analogRead(A2);
  voltage = (reading * 5.0)/1024.0;
  return ((voltage-0.5)*100);
}

float get_moisture (){
    sensorValue = analogRead(hum_pin);
    //sensorValue = analogRead(hum_pin);
    return (sensorValue);
}

float get_lux (){
    lux = analogRead(lux_pin)*0.5279+2.386;
    return lux;
}

void update_array_temp() {
  temp_array[s] = get_temp();
  s++;
  if (s == N) {
    s = 0;
  }
}

void update_array_moisture() {
  moisture_array[start] = get_moisture();
  start++;
  if (start == N) {
    start = 0;
  }
}

void update_array_lux() {
  lux_array[start] = get_lux();
  start++;
  if (start == N) {
    start = 0;
  }
}

float get_avged_temp () {
  float acc = 0;
  for (int i = 0; i < N; i++) {
    acc = acc + temp_array[i];
  }
  return (acc / N);
}

float get_avged_moisture () {
  float acc = 0;
  for (int i = 0; i < N; i++) {
    acc = acc + moisture_array[i];
  }
  return (acc / N);
}

float get_avged_lux () {
  float acc = 0;
  for (int i = 0; i < N; i++) {
    acc = acc + lux_array[i];
  }
  return (acc / N);
}
/////////////////////////////////////////////////
/////////// Functions for the Heater/////////////
/////////////////////////////////////////////////

// Function to activate manually the heater

void heater_manual(){
  int state_Heater = digitalRead(pin_heater);
  
  if (state_Heater == 0){
    digitalWrite(pin_heater, HIGH);
    Serial.println("Heater activated manually");
    }
  else if (state_Heater == 1){
    digitalWrite(pin_heater, LOW);
    Serial.println("Heater desactivated manually");
    }
}

// Function to activate automatically the heater

void heater_auto(float MIN,float MAX){
  float Temperature = get_temp();
 
  if(Temperature<MIN){
    digitalWrite(pin_heater,HIGH);
    Serial.println("Switched on Heater (Mode Auto)");
    }
   else if(Temperature>MAX){
    digitalWrite(pin_heater,LOW);
    Serial.println("Switched off Heater (Mode Auto)");
    }
  }

/////////////////////////////////////////////////
////////End code Functions for the Heater////////
/////////////////////////////////////////////////
void add_heure(){
  if(heures == 24){
    heures = 0;
  }
  else{
    heures = heures + 1;
  };
}

void add_min(){
  // modifie la varaible minutes 
  if (minutes == 59){
    minutes = 0; 
    add_heure();
  }
  else{
    minutes = minutes + 1;
  }
}

void add_sec(){
  // renvoie heures, minutes, secondes
  if (secondes == 59){
    secondes = 0; 
    add_min();
  }
  else{
    secondes = secondes + 1;
  }
}

void setup(void){
  pinMode(pin_pump, OUTPUT);  // for the pump
  pinMode(pin_heater, OUTPUT); // heater
  /*digitalWrite(pin_heater,LOW);*/
  Serial.begin(9600);
  
  current_time = millis();
  last_display = current_time;
  current_moisture = get_moisture();
  current_temp = get_temp();
  current_lux = get_lux();
  for (int i = 0; i < N; i++) {
    moisture_array[i] = current_moisture;
    
  }
  for (int a = 0; a < N; a++) {
    temp_array[a] = current_temp;
  }

  for (int b = 0; b < N; b++) {
    temp_array[b] = current_temp;
  }

  //for the mouvmeent 
  previous_distance = sonar.ping_result / US_ROUNDTRIP_CM; 
  delay(50); // juste wait untill the measure is properly done etc.  
  pingTimer = millis(); // Start now.
  // end mouveùent 

  
  
}

void loop()
{
  update_array_moisture();
  delay(10);
  update_array_temp();
  delay(10);
  update_array_lux();
  current_time = millis();
  if (current_time - last_display > dt) {
    last_display = current_time;
    current_temp = get_avged_temp();
    current_moisture = get_avged_moisture();
    current_lux = get_avged_lux();
    Serial.print("{\"Temperature\":");
    Serial.print(current_temp);
    Serial.print(",\"Moisture\":");
    Serial.print(current_moisture);
//    Serial.println("}");    
    Serial.print(",\"Lux\":");
    Serial.print(current_lux);
    Serial.print(",\"Mouvement\":");
    Serial.print(mouvement);
    Serial.println("}");
  }

  // change hour 
  if (millis()/1000 - secondes - 60* minutes - 60*60*heures >= 0){
    // c'est à dire si un nombre entier de seconde s'est écoulé depuis le début
    add_sec();
  }
  //to pump or not to pump, that is the question
  if (auto_pump == 0){/*you don't do anything herebecause you are in full manual mode*/}
  else if (auto_pump == 1){
    if ((current_moisture < lim_hygro) and (heures>8) and (heures<22)){
      // if the pump is in auto mode and the soil is too dry 
      if (millis()-last_pump>= delta_pump){
        // if it has been more than delta_pump since the last pumping event 
        pump();
      }
    }
  }
  else if(auto_pump == 2){/* don't do anything, instructions are sent by NodeRed itself via the SerialPort*/  }

  // to change the parameters of the arduino 
  if (Serial.available()){
    String msg_code = Serial.readString();
    //extract the information
    String id = msg_code.substring(0,2);
    String info = msg_code.substring(2);

    ///// START CODE FOR THE HEATER/////////
      
    //MANUAL
    if(id == "06"){
      //id=06 Turn on/off the Heater manually
      heater_manual();     
    }


    //AUTO
    else if (id=="09"){
      //id=09 change the Heater to mode automatically
      //T_min and T_max are set by default      
      heater_auto(MinT,MaxT);              
      }

    else if(id=="01"){
      float T_min = info.toFloat();
      heater_auto(T_min,MaxT);
      }
    else if(id=="02"){
      float T_max = info.toFloat();
      heater_auto(MinT,T_max);
      }
 
    ///// END CODE FOR THE HEATER/////////

    
    // to see the correspondance between the code and the action to do, chek the git 
    
    //////////////////////////////////////////
    ///////// the pump's parametrers /////////
    //////////////////////////////////////////
    //force watering
    if (id == "05"){
      pump();
    }
    //change the mode 
    else if (id == "08"){   
      if (info == "0000"){ //full manual
        auto_pump = 0;
      }      
      else if (info == "0001"){ //automatic and hygro
        auto_pump = 1 ;
      }      
      else if (info == "0002"){ //automatic cylcic 
        auto_pump = 2;
      }      
    }

    //change the minimum hygrometry 
    else if (id == "07"){
      lim_hygro = info.toInt();
    }


    //change time_pump
    else if (id == "11"){ 
      time_pump = info.toInt();
    }
    
    //////////////////////////////////////////
    ///////// end pump's parametrers /////////
    //////////////////////////////////////////

    // change the hour of the arduino
    else if (id == "10"){      
      heures = (info.substring(0,2)).toInt();
      minutes = (info.substring(2)).toInt();   
    }


  }
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
    // allumer l'écran ici
  } 
  
}