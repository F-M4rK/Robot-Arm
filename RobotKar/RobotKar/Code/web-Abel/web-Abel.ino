int Pin1 = 13;//IN1 is connected to 13
int Pin2 = 12;//IN2 is connected to 12 
int Pin3 = 14;//IN3 is connected to 14
int Pin4 = 27;//IN4 is connected to 27

int pole1[] ={0,0,0,0, 0,1,1,1, 0};//pole1, 8 step values
int pole2[] ={0,0,0,1, 1,1,0,0, 0};//pole2, 8 step values
int pole3[] ={0,1,1,1, 0,0,0,0, 0};//pole3, 8 step values
int pole4[] ={1,1,0,0, 0,0,0,1, 0};//pole4, 8 step values

int poleStep = 0;
int  dirStatus = 3;// stores direction status 3= stop (do not change)

String buttonTitle1[] ={"CCW", "CW"};
String buttonTitle2[] ={"CCW", "CW"};
String argId[] ={"ccw", "cw"};

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char *ssid = "BKSZCDKA2";
const char *password = "Tavasz08";

WebServer server(80);

void handleRoot() {
  // Teljesen újraírt, modern HTML és CSS Raw String Literal használatával
  String HTML = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
    <title>ESP32 Motor Vezérlés</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        background-color: #e9ecef;
        margin: 0;
        padding: 20px;
        display: flex;
        justify-content: center;
        align-items: center;
        min-height: 90vh;
      }
      .card {
        background: white;
        width: 100%;
        max-width: 450px;
        padding: 40px 30px;
        border-radius: 16px;
        box-shadow: 0 10px 30px rgba(0,0,0,0.1);
        text-align: center;
      }
      h1 {
        color: #343a40;
        font-size: 26px;
        margin-top: 0;
        margin-bottom: 25px;
      }
      .status {
        font-size: 16px;
        font-weight: bold;
        padding: 15px 20px;
        border-radius: 10px;
        margin-bottom: 30px;
        display: block;
        text-transform: uppercase;
        letter-spacing: 1px;
      }
      .status.cw { background-color: #fff3cd; color: #856404; border: 1px solid #ffeeba; }
      .status.ccw { background-color: #cff4fc; color: #055160; border: 1px solid #b6effb; }
      .status.off { background-color: #f8d7da; color: #842029; border: 1px solid #f5c2c7; }
      .btn-container {
        display: flex;
        justify-content: space-between;
        gap: 15px;
        flex-wrap: wrap;
      }
      .btn {
        text-decoration: none;
        font-size: 18px;
        font-weight: 600;
        padding: 15px 0;
        border-radius: 10px;
        color: white;
        transition: all 0.2s ease;
        flex: 1;
        min-width: 130px;
        box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        cursor: pointer;
      }
      .btn:active {
        transform: translateY(3px);
        box-shadow: 0 1px 2px rgba(0,0,0,0.1);
      }
      .btn-start-ccw { background-color: #0d6efd; }
      .btn-start-ccw:hover { background-color: #0b5ed7; }
      .btn-start-cw { background-color: #198754; }
      .btn-start-cw:hover { background-color: #157347; }
      .btn-stop { background-color: #dc3545; }
      .btn-stop:hover { background-color: #bb2d3b; }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>Stepper Motor Vezérlő</h1>
  )=====";

  // Állapot sáv generálása
  if(dirStatus == 2){
    HTML += "<div class=\"status cw\">🟢 Állapot: Forgás Jobbra (CW)</div>";
  } else if(dirStatus == 1){
    HTML += "<div class=\"status ccw\">🔵 Állapot: Forgás Balra (CCW)</div>";
  } else {
    HTML += "<div class=\"status off\">🔴 Állapot: KIKAPCSOLVA</div>";
  }

  HTML += "<div class=\"btn-container\">";

  // CCW (Balra) Gomb logika
  if(dirStatus == 1){
    HTML += "<a class=\"btn btn-stop\" href=\"/motor?" + argId[0] + "=off\">Stop CCW</a>";
  } else {
    HTML += "<a class=\"btn btn-start-ccw\" href=\"/motor?" + argId[0] + "=on\">Start CCW</a>";
  }

  // CW (Jobbra) Gomb logika
  if(dirStatus == 2){
    HTML += "<a class=\"btn btn-stop\" href=\"/motor?" + argId[1] + "=off\">Stop CW</a>";
  } else {
    HTML += "<a class=\"btn btn-start-cw\" href=\"/motor?" + argId[1] + "=on\">Start CW</a>";
  }

  HTML += R"=====(
      </div>
    </div>
  </body>
  </html>
  )=====";

  server.send(200, "text/html", HTML); 
}//handleRoot()

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
 pinMode(Pin1, OUTPUT);
 pinMode(Pin2, OUTPUT);   
 pinMode(Pin3, OUTPUT);   
 pinMode(Pin4, OUTPUT);   

  Serial.begin(115200);
  Serial.println("Robojax 28BYJ-48 Stepper Motor Control");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: http://");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("robojaxESP32")) {
    Serial.println("MDNS responder started");
    Serial.println("access via http://robojaxESP32");
  }

  server.on("/", handleRoot);
  server.on("/motor", HTTP_GET, motorControl);           
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  if(dirStatus == 1){
    poleStep++;
    driveStepper(poleStep);   
  } else if(dirStatus == 2){
    poleStep--;
    driveStepper(poleStep);   
  } else {
    driveStepper(8);   
  }

  if(poleStep > 7){ poleStep = 0; }
  if(poleStep < 0){ poleStep = 7; }

  delay(1);
}

void motorControl() {
    if(server.arg(argId[0]) == "on") {
      dirStatus = 1;// CCW           
    } else if(server.arg(argId[0]) == "off"){
      dirStatus = 3;  // motor OFF
    } else if(server.arg(argId[1]) == "on"){
      dirStatus = 2;  // CW 
    } else if(server.arg(argId[1]) == "off"){
      dirStatus = 3;  // motor OFF
    } 
  handleRoot();
}

void driveStepper(int c) {
     digitalWrite(Pin1, pole1[c]); 
     digitalWrite(Pin2, pole2[c]);
     digitalWrite(Pin3, pole3[c]);
     digitalWrite(Pin4, pole4[c]);
}