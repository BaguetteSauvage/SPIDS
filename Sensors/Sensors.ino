#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4


const int OUT_PIN = A5;
const int IN_PIN = A0;
const float IN_STRAY_CAP_TO_GND = 24.48;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;
const float R_PULLUP = 34.8;  
const int MAX_ADC_VALUE = 1023;
float capacitance;


long int dt = 1000;
const int N = 100 ;
long int last_display;
long int current_time;
float temp_array[N];

float hum_array[N];
int start =0;
float current_hum;
float current_temp;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float get_temp () {
  sensors.requestTemperatures(); 
  return sensors.getTempCByIndex(0); 
}

float get_hum (){
    pinMode(IN_PIN, INPUT);
    digitalWrite(OUT_PIN, HIGH);
    int val = analogRead(IN_PIN);
    digitalWrite(OUT_PIN, LOW);
     pinMode(IN_PIN, OUTPUT);
    float  capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
    if (capacitance < 110){
      return (0);
    }
    if (110 < capacitance <= 134){
    return (capacitance-106)/0.4; 
    }

   if( 134 <=capacitance<=143){
    return (capacitance-89)/0.6;
   }
}

void update_array_temp() {
  temp_array[start] = get_temp();
  start++;
  if (start == N) {
    start = 0;
  }
}

void update_array_hum() {
  hum_array[start] = get_hum();
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

float get_avged_hum () {
  float acc = 0;
  for (int i = 0; i < N; i++) {
    acc = acc + hum_array[i];
  }
  return (acc / N);
}

void setup(void)
{
  pinMode(OUT_PIN, OUTPUT);
  pinMode(IN_PIN, OUTPUT);
  Serial.begin(9600);
  sensors.begin();
  current_time = millis();
  last_display = current_time;
  current_hum = get_hum();
  current_temp = get_temp();
  for (int i = 0; i < N; i++) {
    hum_array[i] = current_hum;
    temp_array[i] = current_temp;
  }
}
void loop()
{
  update_array_temp();
  update_array_hum();
  current_time = millis();
  if (current_time - last_display > dt) {
    last_display = current_time;
    current_hum = get_avged_hum();
    current_temp = get_avged_temp();
    Serial.print("{\"Temperature\":");
    Serial.print(current_temp);
    Serial.print(",\"Humidity\":");
    Serial.print(current_hum);
    Serial.println("}");
  }
}
