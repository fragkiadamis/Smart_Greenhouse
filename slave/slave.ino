// Include nescessary libraries and files
#include <avr/sleep.h>
#include <bluetooth_com.h>
#include <Servo.h>
#include "DHT.h"
#include "slave.h"

// Initialize Bluetooth
BluetoothCom bt;
// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);
// Initialize Servo
Servo irrigationValve;

// Arduino falls into deep sleep
void mcuSleep(void) {
  sleep_enable(); // Enabling sleep mode
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), wakeUp, CHANGE);
  set_sleep_mode(SLEEP_MODE_ADC); // Setting the sleep mode.
  sleep_cpu(); // Activating sleep mode
}

// Toggle buzzer (turn ON or OFF)
void toggleBuzzer(bool turnOn) {
  if (turnOn)
    return tone(BUZZER_PIN, BUZZER_FREQ);

  noTone(BUZZER_PIN);
}

// Read DHT sensor humidity
float readDHTSensor(uint8_t sensor) {
  float val = 0;

  switch (sensor) {
    case HUMIDITY: val = dht.readHumidity(); break;
    case INNER_TEMP: val = dht.readTemperature(); break;
  }
  
  if (isnan(val))
    return -1;

  return val;
}

// Toggle Irrigation (turn ON or OFF)
void toggleIrrigation(uint8_t turnOn) {
  if (turnOn)
    return irrigationValve.write(180);
  
  irrigationValve.write(0);
}

// Read the outside temperature from LM35 sensor
float readLM35Sensor(void) {
  uint16_t reading = analogRead(LM35_PIN);
  float voltage = reading * (1100 / 1024.0);
  float temperature = voltage / 10;
  return temperature;
}

// Take the percentage of inner luminosity to the outter
float luminocityPercentage(uint8_t sensor) {
  uint16_t inner = analogRead(INNER_LDR_PIN);
  uint16_t outter = analogRead(OUTTER_LDR_PIN);

  return ((float)inner / (float)outter) * 100.0;
}

// Extract the command from the string and execute it
void executeCommand(String cmd) {
  uint8_t splitIndex = cmd.indexOf('|');
  uint8_t action = (cmd.substring(0, splitIndex)).toInt();
  uint8_t value = 0;
  if (splitIndex)
    value = (cmd.substring(splitIndex + 1, cmd.length())).toInt();

  if (action == BZ)
    return toggleBuzzer(value);
  else if (action == DHT_SENS) {
    float sensorValue = readDHTSensor(value);
    bt.send(sensorValue);
  } else if (action == IRG)
    return toggleIrrigation(value);
  else if (action == OUTER_TEMP) {
    float temperature = readLM35Sensor();
    bt.send(temperature);
  } else if (action == LUM) {
    float luminosityPerc = luminocityPercentage(value);
    bt.send(luminosityPerc);
  } else {
    Serial.println(F("Undefined command"));
  }
}

void setup() {
  // Setup hardware serial
  Serial.begin(BAUD_RATE);
  while(!Serial);
  Serial.flush();

  // Setup Outputs
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);

  // Setup inputs
  pinMode(DHT_PIN, INPUT);
  pinMode(LM35_PIN, INPUT);
  pinMode(INNER_LDR_PIN, INPUT);
  pinMode(OUTTER_LDR_PIN, INPUT);

  // Setup Bluetooth
  bt.begin();
  // Setup DHT sensor
  dht.begin();

  // Setup Irrigation valve servo motor
  irrigationValve.attach(VALVE_PIN);
  irrigationValve.write(0);

  // Set the reference voltage for analog input to the built-in 1.1
  analogReference(INTERNAL);
}

void loop() {
  // Fall into deep sleep
  // mcuSleep();

  // If bluetooth buffer has a message
  if (bt.hasMessage()) {
    char cmd[SERIAL_BUFFER_SIZE] = {0};
    bt.receive(cmd);
    executeCommand(String(cmd));
  }
}

// Interrupt from sleep
void wakeUp(void) {
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
}