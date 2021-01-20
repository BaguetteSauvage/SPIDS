#include <NewPing.h>


/////////////////////////////////////////////////
//   What is done in this script : we try to   //
// detect the mouvement in front of the sensor //
/////////////////////////////////////////////////

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 1000; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.
unsigned int tolerance = 10 ;  // The tolerance in the variation of distance mesured by the sensor
unsigned int previous_distance; //the last ditance recorded y the sensor, should be in cm
bool mouvement = true; // when we plug the greenhouse, we image hat someone is next to it... 

void setup() {
  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.  
  previous_distance = sonar.ping_result / US_ROUNDTRIP_CM; 
  delay(50); // juste wait untill the measure is properly done etc.
  
  pingTimer = millis(); // Start now.
}

void loop() {
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
  // Do other stuff here, really. Think of it as multi-tasking.
}

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
