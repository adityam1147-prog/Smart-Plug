#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "AndroidAP";
const char* password = "jhvk9727";

WebServer server(80);

#define ARD_RX 13
#define ARD_TX 4

String lastData = "";
String voltage = "--", current = "--", power = "--", energy = "--";
String freq = "--", pf = "--", cost = "--", relayState = "OFF";

void parseData(String data) {
  int idx = 0;
  String values[8];
  while (data.length() > 0 && idx < 8) {
    int comma = data.indexOf(',');
    values[idx] = (comma == -1) ? data : data.substring(0, comma);
    data = (comma == -1) ? "" : data.substring(comma + 1);
    idx++;
  }
  if (idx == 8) {
    voltage    = values[0];
    current    = values[1];
    power      = values[2];
    energy     = values[3];
    freq       = values[4];
    pf         = values[5];
    cost       = values[6];
    relayState = values[7];
  }
}

void handleRoot() {
  String html = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <meta http-equiv='refresh' content='2'>
  <title>Smart Plug Dashboard</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      background: #0a0a0a;
      color: #fff;
      min-height: 100vh;
      padding: 20px;
    }
    .header {
      text-align: center;
      margin-bottom: 24px;
    }
    .header h1 {
      font-size: 22px;
      color: #f90;
      letter-spacing: 1px;
    }
    .header p {
      font-size: 12px;
      color: #555;
      margin-top: 4px;
    }
    .grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      max-width: 480px;
      margin: 0 auto 20px auto;
    }
    .card {
      background: #161616;
      border-radius: 14px;
      padding: 16px;
      border: 1px solid #222;
    }
    .card .label {
      font-size: 11px;
      color: #666;
      text-transform: uppercase;
      letter-spacing: 1px;
      margin-bottom: 6px;
    }
    .card .value {
      font-size: 26px;
      font-weight: 700;
      color: #fff;
    }
    .card .unit {
      font-size: 13px;
      color: #888;
      margin-left: 3px;
    }
    .card.highlight .value { color: #f90; }
    .card.green .value { color: #2ecc71; }
    .card.blue .value { color: #3498db; }
    .card.purple .value { color: #9b59b6; }
    .relay-card {
      background: #161616;
      border-radius: 14px;
      padding: 20px;
      max-width: 480px;
      margin: 0 auto 12px auto;
      border: 1px solid #222;
      display: flex;
      align-items: center;
      justify-content: space-between;
    }
    .relay-info .label {
      font-size: 11px;
      color: #666;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    .relay-info .status {
      font-size: 20px;
      font-weight: 700;
      margin-top: 4px;
    }
    .status.on { color: #2ecc71; }
    .status.off { color: #e74c3c; }
    .relay-buttons {
      display: flex;
      gap: 10px;
    }
    .btn {
      padding: 12px 22px;
      font-size: 15px;
      font-weight: 600;
      border: none;
      border-radius: 10px;
      cursor: pointer;
      text-decoration: none;
      display: inline-block;
    }
    .btn-on  { background: #2ecc71; color: #000; }
    .btn-off { background: #e74c3c; color: #fff; }
    .refresh-card {
      max-width: 480px;
      margin: 0 auto;
      text-align: center;
    }
    .btn-refresh {
      background: #1a1a1a;
      color: #888;
      border: 1px solid #333;
      width: 100%;
      padding: 14px;
      border-radius: 12px;
      font-size: 14px;
    }
    .cost-bar {
      max-width: 480px;
      margin: 12px auto 0 auto;
      background: #161616;
      border-radius: 14px;
      padding: 16px;
      border: 1px solid #222;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .cost-bar .label { font-size: 11px; color: #666; text-transform: uppercase; letter-spacing: 1px; }
    .cost-bar .amount { font-size: 28px; font-weight: 700; color: #f90; }
    .cost-bar .sub { font-size: 12px; color: #555; margin-top: 2px; }
  </style>
</head>
<body>
  <div class='header'>
    <h1>&#9889; Smart Plug</h1>
    <p>ECE360 — Real-Time Energy Monitor</p>
  </div>

  <div class='relay-card'>
    <div class='relay-info'>
      <div class='label'>Appliance</div>
      <div class='status )rawhtml";

  html += (relayState == "ON") ? "on'>&#9679; ON" : "off'>&#9679; OFF";

  html += R"rawhtml(</div>
    </div>
    <div class='relay-buttons'>
      <a href='/relay?state=ON'><button class='btn btn-on'>ON</button></a>
      <a href='/relay?state=OFF'><button class='btn btn-off'>OFF</button></a>
    </div>
  </div>

  <div class='grid'>
    <div class='card highlight'>
      <div class='label'>Voltage</div>
      <div class='value'>)rawhtml";
  html += voltage;
  html += R"rawhtml(<span class='unit'>V</span></div>
    </div>
    <div class='card green'>
      <div class='label'>Current</div>
      <div class='value'>)rawhtml";
  html += current;
  html += R"rawhtml(<span class='unit'>A</span></div>
    </div>
    <div class='card blue'>
      <div class='label'>Active Power</div>
      <div class='value'>)rawhtml";
  html += power;
  html += R"rawhtml(<span class='unit'>W</span></div>
    </div>
    <div class='card purple'>
      <div class='label'>Frequency</div>
      <div class='value'>)rawhtml";
  html += freq;
  html += R"rawhtml(<span class='unit'>Hz</span></div>
    </div>
    <div class='card'>
      <div class='label'>Power Factor</div>
      <div class='value'>)rawhtml";
  html += pf;
  html += R"rawhtml(</div>
    </div>
    <div class='card'>
      <div class='label'>Energy Used</div>
      <div class='value'>)rawhtml";
  html += energy;
  html += R"rawhtml(<span class='unit'>kWh</span></div>
    </div>
  </div>

  <div class='cost-bar'>
    <div>
      <div class='label'>Estimated Cost</div>
      <div class='sub'>@ &#8377;8 per kWh</div>
    </div>
    <div class='amount'>&#8377;)rawhtml";
  html += cost;
  html += R"rawhtml(</div>
  </div>

  <br>
  <div class='refresh-card'>
    <button class='btn btn-refresh' onclick='location.reload()'>&#8635; Refresh Data</button>
  </div>

</body>
</html>
)rawhtml";

  server.send(200, "text/html", html);
}

void handleRelay() {
  if (server.hasArg("state")) {
    String state = server.arg("state");
    Serial2.println(state);
    Serial.println("Sent to Arduino: " + state);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleData() {
  server.send(200, "text/plain", lastData);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, ARD_RX, ARD_TX);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/relay", handleRelay);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();
    if (data.length() > 0) {
      lastData = data;
      parseData(data);
      Serial.println("From Arduino: " + data);
    }
  }
}