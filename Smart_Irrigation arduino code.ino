const int sensorPin = A0;
const int relayPin  = 8;
const int ledPin    = 13; 

int dryReading = 800;  
int wetReading = 300;  

int thresholdPercent = 40; 
int hysteresis = 5;       

bool relayActiveLow = true; 
int relayOnState  = relayActiveLow ? LOW : HIGH;
int relayOffState = relayActiveLow ? HIGH : LOW;

unsigned long minPumpRun = 10000UL;   
unsigned long maxPumpRun = 600000UL;  
unsigned long pumpStartTime = 0;
bool pumpState = false;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(relayPin, relayOffState);
  digitalWrite(ledPin, LOW);
  Serial.println("Smart Irrigation system starting...");
  Serial.println("Commands: 'd' = set current reading as DRY, 'w' = set current as WET");
}

void loop() {
  int raw = analogRead(sensorPin);  
  float moisturePercent;
  if (dryReading == wetReading) moisturePercent = 0;
  else moisturePercent = (float)(raw - wetReading) * 100.0 / (float)(dryReading - wetReading);

  if (moisturePercent < 0) moisturePercent = 0;
  if (moisturePercent > 100) moisturePercent = 100;

  Serial.print("Raw: "); Serial.print(raw);
  Serial.print("\tMoisture: "); Serial.print((int)moisturePercent); Serial.print("%");
  Serial.print("\tPump: "); Serial.println(pumpState ? "ON" : "OFF");

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'd') {
      dryReading = raw;
      Serial.print("Set DRY reading = "); Serial.println(dryReading);
    } else if (c == 'w') {
      wetReading = raw;
      Serial.print("Set WET reading = "); Serial.println(wetReading);
    }
  }

  unsigned long now = millis();
  if (!pumpState) {
    if (moisturePercent < (thresholdPercent - hysteresis)) {
      startPump();
    }
  } else {

    if ( ((moisturePercent > (thresholdPercent + hysteresis)) && (now - pumpStartTime >= minPumpRun))
         || (now - pumpStartTime >= maxPumpRun) ) {
      stopPump();
    }
  }

  delay(2000); 
}

void startPump() {
  digitalWrite(relayPin, relayOnState);
  digitalWrite(ledPin, HIGH);
  pumpStartTime = millis();
  pumpState = true;
  Serial.println("Pump started");
}

void stopPump() {
  digitalWrite(relayPin, relayOffState);
  digitalWrite(ledPin, LOW);
  pumpState = false;
  Serial.println("Pump stopped");
}
