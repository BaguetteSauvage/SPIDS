int sensorPin = A0;
int sensorValue = 0;
long int dt = 1000;
const int N = 100 ;
long int last_display;
long int current_time;
float temp_array[N];
float moisture_array[N];
int start =0;
int s = 0;
float current_moisture;
float current_temp;
float voltage;
float reading;

float get_temp () {
  reading = analogRead(A2);
  voltage = (reading * 5.0)/1024.0;
  return ((voltage-0.5)*100);
}

float get_moisture (){
    sensorValue = analogRead(sensorPin);
    return (sensorValue);
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

void setup(void)
{
  Serial.begin(9600);
  current_time = millis();
  last_display = current_time;
  current_moisture = get_moisture();
  current_temp = get_temp();
  for (int i = 0; i < N; i++) {
    moisture_array[i] = current_moisture;
    
  }
  for (int a = 0; a < N; a++) {
    temp_array[a] = current_temp;
  }
}
void loop()
{
  update_array_moisture();
  update_array_temp();
  current_time = millis();
  if (current_time - last_display > dt) {
    last_display = current_time;
    current_temp = get_avged_temp();
    current_moisture = get_avged_moisture();
    Serial.print("{\"Temperature\":");
    Serial.print(current_temp);
    Serial.print(",\"Moisture\":");
    Serial.print(current_moisture);
    Serial.println("}");
  }
}
