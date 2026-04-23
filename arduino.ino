#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

// PZEM on pins 2,3
SoftwareSerial pzemSerial(2, 3);
PZEM004Tv30 pzem(pzemSerial);

// ESP32 on pins 10,11
SoftwareSerial espSerial(10, 11);

#define RELAY_PIN 7

unsigned long lastSend = 0;
const long SEND_INTERVAL = 15000;
bool relayState = false;

void setup() {
  Serial.begin(9600);
  pzemSerial.begin(9600);
  espSerial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
  // Listen to ESP32 for commands
  espSerial.listen();
  delay(10);
  if (espSerial.available()) {
    String cmd = espSerial.readStringUntil('\n');
    cmd.trim();
    Serial.println("CMD: " + cmd);
    if (cmd == "ON") {
      digitalWrite(RELAY_PIN, LOW);
      relayState = true;
      Serial.println("Relay ON");
    } else if (cmd == "OFF") {
      digitalWrite(RELAY_PIN, HIGH);
      relayState = false;
      Serial.println("Relay OFF");
    }
  }

  unsigned long now = millis();

  // Send data to ESP32 every 15 seconds
  if (now - lastSend >= SEND_INTERVAL) {
    lastSend = now;

    pzemSerial.listen();
    delay(10);
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power   = pzem.power();
    float energy  = pzem.energy();
    float pf      = pzem.pf();
    float freq    = pzem.frequency();
    float cost    = energy * 8.0;

    Serial.println("V:" + String(voltage) + " I:" + String(current) + " P:" + String(power));

    if (!isnan(voltage)) {
      String data = String(voltage, 1) + "," +
                    String(current, 2) + "," +
                    String(power, 1) + "," +
                    String(energy, 3) + "," +
                    String(freq, 1) + "," +
                    String(pf, 2) + "," +
                    String(cost, 2) + "," +
                    (relayState ? "ON" : "OFF");
      espSerial.listen();
      espSerial.println(data);
      Serial.println("Sent: " + data);
    }
  }
}