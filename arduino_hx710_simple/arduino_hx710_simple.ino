// Simple 1-hose project for measuring traffic counts over 24 hour period.
//
//   |   Arduino Nano  |    |      HX710B     |  
//   |     Ctl Data    |    |                 |
//   | Gnd  D3  D2  5V |    | VCC OUT CLK GND |
//   +-----------------+    +-----------------+
//      |   |   |   +----------+   |   |   |
//      |   |   +------------------+   |   |
//      |   +--------------------------+   |
//      +----------------------------------+

#define pressure_ctrl 3
#define pressure_data 2

void setup() {
  Serial.begin(9600);
  pinMode(pressure_ctrl, OUTPUT);
  pinMode(pressure_data, INPUT);
}

int32_t read_air_pressure() {
  int32_t air_pressure = 0;
  digitalWrite(pressure_ctrl, LOW);
  while (digitalRead(pressure_data) == HIGH) {
    // HIGH means HX710B is still converting and not ready yet.
  }
  
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(pressure_ctrl, HIGH);
    delayMicroseconds(1);
    air_pressure <<= 1;
    air_pressure += digitalRead(pressure_data);
    digitalWrite(pressure_ctrl, LOW);
    delayMicroseconds(1);
  }

  // This should put us in 40 Hz sampling
  uint8_t control_cycles[] = {HIGH, LOW, HIGH, LOW, HIGH};
  for (const auto value : control_cycles) {
    digitalWrite(pressure_ctrl, value);
    delayMicroseconds(1);
  }

  return air_pressure;
}

void loop() {
  const int NUM_MEASUREMENTS = 33;
  int32_t max_pressure = 0;
  const auto start = millis();
  for (int _ = 0; _ < NUM_MEASUREMENTS; ++_) {
    int32_t pressure = read_air_pressure();
    if (pressure > max_pressure) {
      max_pressure = pressure;
    }
  }
  Serial.print("max_pressure: ");
  Serial.print(max_pressure);
  Serial.print(" from ");
  Serial.print(NUM_MEASUREMENTS);
  Serial.print(" measurments in ");
  Serial.print(millis() - start);
  Serial.println(" milliseconds");
}
