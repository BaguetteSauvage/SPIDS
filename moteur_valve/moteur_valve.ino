/* Inclut la lib Servo pour manipuler le servomoteur */ 
#include <Servo.h>

/* Créer un objet Servo pour contrôler le servomoteur */ 
Servo monServomoteur; 

int etat = 1; // 0 for inactive, the motor doesn't run; 1 for active, the motor turns 

// the button to command our servo-motor
int button = 2; 

void setup() { 
  // Attache le servomoteur à la broche D9 
  monServomoteur.attach(9); 
  monServomoteur.write(0);
  delay(1000);

  /*moniteur série*/
  Serial.begin(9600);

  /* notre bouton*/
  pinMode(button, INPUT);
  attachInterrupt(digitalPinToInterrupt(button), command, RISING);
} 

void command(){
  etat = 1- etat;
  Serial.print(etat);
}
    
void loop() { 
  if (etat == 0){    
    monServomoteur.write(0);    
  }
  else{    /* Le bras bouge */
    // Fait bouger le bras de 0° à 180° 
    for (int position = 0; position <= 180; position++) { 
      monServomoteur.write(position); 
      delay(20); 
      Serial.println(position);
    } 
    // Fait bouger le bras de 180° à 10° 
    for (int position = 180; position >= 0; position--) { 
      monServomoteur.write(position); 
      delay(20); 
      Serial.println(position);    
    }
  }
}
