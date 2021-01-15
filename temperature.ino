#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

const int N = 100;
float temp_array[N];
int start = 0;

long int last_display;
long int current_time;
long int dt = 1000;

float current_temp;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float get_temp () {
  sensors.requestTemperatures(); 
  return sensors.getTempCByIndex(0); 
}

void update_array() {
  temp_array[start] = get_temp();
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

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  current_time = millis();
  last_display = current_time;
  current_temp = get_temp();
  for (int i = 0; i < N; i++) {
    temp_array[i] = current_temp;
  }
}

void loop () {
  update_array();
  current_time = millis();
  if (current_time - last_display > dt) {
    last_display = current_time;
    current_temp = get_avged_temp();
    Serial.print(current_temp);
    Serial.print("Temperature : ");
    Serial.print("\n");
  }
}
