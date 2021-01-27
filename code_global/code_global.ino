int sensorPin = A5;
int sensorValue = 0;
long int dt = 2000;
const int N = 100 ;
long int last_display;
long int current_time;
float temp_array[N];
float moisture_array[N];
int start =0;
int s = 0;
float current_moisture;
float current_temp;
float current_lux;
float voltage;
float reading;
float lux;


/////////////////////////////////////////////////
//   Lux                                       //
/////////////////////////////////////////////////
const int SensorPin=A0;
float lux_array[N];
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

unsigned int pingSpeed = 1000; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.
unsigned int tolerance = 10 ;  // The tolerance in the variation of distance mesured by the sensor
unsigned int previous_distance; //the last ditance recorded y the sensor, should be in cm
bool mouvement = true; // when we plug the greenhouse, we image hat someone is next to it... 


void echoCheck() { 
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    if (abs(previous_distance - sonar.ping_result/US_ROUNDTRIP_CM) > tolerance){
      // this means there is significant change in the distance 
      previous_distance = sonar.ping_result / US_ROUNDTRIP_CM; 
      mouvement = true;
      
      Serial.print("Mouvement : ");
      Serial.print(mouvement);
      Serial.print("           Ping (cm): ");
      Serial.println(sonar.ping_result/US_ROUNDTRIP_CM);    
    }
    else{
      mouvement = false; 
      Serial.print("Mouvement : ");
      Serial.print(mouvement);
      Serial.print("           Ping (cm): ");
      Serial.println(sonar.ping_result/US_ROUNDTRIP_CM);     
    }
  }
}
/////////////////////////////////////////////////
//            end mouvmeent                    //
/////////////////////////////////////////////////

float get_temp () {
  reading = analogRead(A2);
  //reading = analogRead(A2);
  voltage = (reading * 5.0)/1024.0;
  return ((voltage-0.5)*100);
}

float get_moisture (){
    sensorValue = analogRead(sensorPin);
    //sensorValue = analogRead(sensorPin);
    return (sensorValue);
}

float get_lux (){
    lux = analogRead(SensorPin)*0.5279+2.386;
    return (lux);
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

void setup(void)
{
  
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
    Serial.println("}");    
    Serial.print(",\"Lux\":");
    Serial.print(current_lux);
    Serial.println("}");
  }
    
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
    // allumer l'écran ici
  }
}
