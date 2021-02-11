#include <NewPing.h>

/// the pins used 
const int hum_pin = A5;
const int lux_pin=A0;
const int temp_pin=A2;
const int pin_pump=4; //digital pin 4 to command the water pump
const int pin_heater=8; //digital pin 8 to command the heater
const int pin_light = 10; // digital pin 10 to command the relay, to command the light 
//carefull, pin 12 is used by the distance (=> mouvement) detector, see infra 

int sensorValue = 0;
const int N = 100 ;
float lux_array[N];
float temp_array[N];
float moisture_array[N];

// what time is it ?
int heures;
int minutes; 

// Sleepy time 
int h_morning;
int m_morning;
int h_night;
int m_night;

// pump state
bool pump_state = 0; // 0 if not functionning, 1 if functionning

////// the pump control //////////
int auto_pump = 1; 
/*  
by default, the pump is controlled automatically. We can change the mode by changing the value of auto_pump :
0 : entirely manual 
1 : automatic with hygrometrie 
2 : automatic cyclic 
*/
long unsigned int delta_pump = 10 * 60000; // how long should we wait before watering the plant once more (in ms)
long unsigned int last_pump =  0; //the last time plants were watered. The initial value being 0, the hygrometry value will hhave timle to stabilise 
int time_pump = 3000; //how long should we pump each time (ms)
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

// to send information to the raspbery 
const int dt = 1000;
long unsigned int last_display;



/////////////////////////////////////////////////
//   What is done in this script : we try to   //
// detect the mouvement in front of the sensor //
/////////////////////////////////////////////////

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const unsigned int pingSpeed = 1000; // How frequently are we going to send out a ping (in milliseconds).
unsigned long pingTimer;     // Holds the previous ping time.
const unsigned int tolerance = 10 ;  // The tolerance in the variation of distance mesured by the sensor
unsigned int previous_distance; //the last ditance recorded y the sensor, should be in cm
bool mouvement = true; // when we plug the greenhouse, we imagine hat someone is next to it... 


void echoCheck() { 
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    if (abs(previous_distance - sonar.ping_result/US_ROUNDTRIP_CM) > tolerance){
      // this means there is significant change in the distance 
      previous_distance = sonar.ping_result / US_ROUNDTRIP_CM; 
      mouvement = true;
    }
    else{
      mouvement = false; 
    }
  }
  else{mouvement = false;};
  }
/////////////////////////////////////////////////
//            end mouvmeent                    //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
////// Are you allowed to pump ? /////////////////
/////////////////////////////////////////////////

bool allowed(){
  if((heures == h_morning) or (heures == h_night)){
    if (heures == h_morning){return (minutes>=m_morning);}  
    else {return (minutes<=m_night);}  
  }
  else if ((heures > h_morning) and (heures < h_night)){return true;}
  else{return false;}
}

/////////////////////////////////////////////////
////////// End allowed to pump ? /////////////////
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
    lux = (analogRead(lux_pin)*0.5279+2.386)*5-80;
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
void setup(){
  pinMode(pin_pump, OUTPUT);  // for the pump
  pinMode(pin_heater, OUTPUT); // heater
  pinMode(pin_light, OUTPUT); //the light
  digitalWrite(pin_light,HIGH); //even if it is counter intuitive, high means lamp off, low means lamp on
  Serial.begin(9600);
  
  last_display = millis();
  current_moisture = get_moisture();
  current_temp = get_temp();
  current_lux = get_lux();
  for (int i = 0; i < N; i++) {
    moisture_array[i] = current_moisture;
    
  }
  for (int a = 0; a < N; a++) {
    temp_array[a] = current_temp;
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

  /*
  >>>>>>>>>>>>>>> About the use of millis <<<<<<<<<<<<<<<<<
  After a certain amount of time, millis() will overflow. Therefore, our test won't be correct because our last values (such as last_display).
  Therefore, because we only use millis() in this kind of context : 
    > last_watever = millis()
    > if (millis() - last_watever > delta_watever){do something; usually including the previous line;}
  So we can say that : 
    > we add the condition (millis()-last_watever < 0) to the previous one, 
    therefore, when millis overflows, the condition is quite automatically fulfilled. The good point of that is that it will change the last_watever to a correct value
    The bad point is that we enter in the loop even if is has been less than delta_watever since previous_watever (for example, we will send the report of informations too eraly). 
    But we will accept this since it does not cause major disturbance. 

    Concerning the use of millis() in detecting the distance, overflow will eventually cause a disturbance lasting maximum once evry 50 days... 
  */ 
 
  if ((millis() - last_display > dt) or (millis() - last_display < 0)){
    last_display = millis();
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

  //to pump or not to pump, that is the question
  if (auto_pump == 0){/*you don't do anything here because you are in full manual mode*/}
  else if (auto_pump == 1){
    if ((current_moisture < lim_hygro) and allowed()){
      // if the pump is in auto mode and the soil is too dry 
      if ((millis()-last_pump>= delta_pump) or (millis()-last_pump<=0)){
        // if it has been more than delta_pump since the last pumping event (or millis() overflowed)
        last_pump = millis();           
        digitalWrite(pin_pump, 0);
        digitalWrite(pin_pump, 1);
        pump_state = 1;
        Serial.println("pump on");
        // pump will be set off after time_pump
      }
    }
  }
  else if(auto_pump == 2){/* don't do anything, instructions are sent by NodeRed itself via the SerialPort*/  }

  //to turn off the pump
  if(((millis() - last_pump > time_pump) or (millis() - last_pump <= 0)) and (pump_state)){
    // it means that we have been pumping for more than time_pump
    digitalWrite(pin_pump, 0);
    pump_state = 0;
    Serial.println("pump off");    
  }

  // to change the parameters of the arduino, communication with node red 
  if (Serial.available()){
    String msg_code = Serial.readString();
    //extract the information
    String id = msg_code.substring(0,2);
    String info = msg_code.substring(2);
    
    // to see the correspondance between the code and the action to do, check the git 

    ////////////////////////////////////////
    ///// START CODE FOR THE HEATER/////////
    ////////////////////////////////////////  
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

    
    //////////////////////////////////////////
    ///////// the pump's parametrers /////////
    //////////////////////////////////////////
    //force watering
    else if (id == "05"){
      last_pump = millis();           
      digitalWrite(pin_pump, 0);
      digitalWrite(pin_pump, 1);
      pump_state = 1;
      Serial.println("pump on");
      // pump will be set off after time_pump
    }
    //change the mode 
    else if (id == "08"){   
      if (info.toInt() == 0){ //full manual
        auto_pump = 0;
        Serial.println("auto pump 0");
      }      
      else if (info.toInt() == 1){ //automatic and hygro
        auto_pump = 1 ;
        Serial.println("auto pump 1");
      }      
      else if (info.toInt() == 2){ //automatic cylcic 
        auto_pump = 2;
        Serial.println("auto pump 2");
      }      
    }
    //change the minimum hygrometry 
    else if (id == "07"){
      Serial.println(lim_hygro);
      lim_hygro = info.toInt();      
      Serial.println(lim_hygro);
    }
    //change time_pump
    else if (id == "11"){ 
      Serial.println(time_pump);
      time_pump = info.toInt();      
      Serial.println(time_pump);
    }  

    //change h_morning and m_morning 
    else if (id == "03"){     
      Serial.println(h_morning);
      Serial.println(m_morning);
      h_morning = (info.substring(0,2)).toInt();
      m_morning = (info.substring(2)).toInt();    
      Serial.println(h_morning);
      Serial.println(m_morning);
    }      
    
    //change h_night and m_night 
    else if (id == "04"){      
      Serial.println(h_night);
      Serial.println(m_night);  
      h_night = (info.substring(0,2)).toInt();
      m_night = (info.substring(2)).toInt();  
      Serial.println(h_night);
      Serial.println(m_night);  
    }  


    //////////////////////////////////////////
    ///////// end pump's parametrers /////////
    //////////////////////////////////////////

    // change current time (should receive a message like this one about every minute) 
    else if (id == "10"){      
      Serial.println(heures);
      Serial.println(minutes); 
      heures = (info.substring(0,2)).toInt();
      minutes = (info.substring(2)).toInt();     
      Serial.println(heures);
      Serial.println(minutes); 
    }


    // the lamp control
    else if (id == "12"){
      if (info.toInt() == 0){digitalWrite(pin_light, HIGH);   
      Serial.println("lamp off"); } 
      else if (info.toInt() == 1111) {digitalWrite(pin_light, LOW);  
      Serial.println("lamp on");}      
    }

  }
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
 if ((millis() - pingTimer >= pingSpeed) or (millis() - pingTimer <= 0)) {   // pingSpeed milliseconds since last ping, do another ping.   
    pingTimer = millis();   
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck", you can check the ping status, where echoChek is defined before
 }
}
