#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// WIFI
const char* ssid = "BKSZCDKA2";
const char* password = "Tavasz08";

WebServer server(80);

// ===== MOTOR PINOK =====
// Motor 1
int M1_Pin1 = 13;
int M1_Pin2 = 12;
int M1_Pin3 = 14;
int M1_Pin4 = 27;

// Motor 2
int M2_Pin1 = 26;
int M2_Pin2 = 25;
int M2_Pin3 = 33;
int M2_Pin4 = 32;

// Motor 3
int M3_Pin1 = 23;
int M3_Pin2 = 22;
int M3_Pin3 = 21;
int M3_Pin4 = 19;

// ===== LÉPÉSSOR =====
int pole1[] ={0,0,0,0, 0,1,1,1, 0};//pole1, 8 step values
int pole2[] ={0,0,0,1, 1,1,0,0, 0};//pole2, 8 step values
int pole3[] ={0,1,1,1, 0,0,0,0, 0};//pole3, 8 step values
int pole4[] ={1,1,0,0, 0,0,0,1, 0};//pole4, 8 step values


// ===== ÁLLAPOTOK =====
int step1 = 0, dir1 = 0;
int step2 = 0, dir2 = 0;
int step3 = 0, dir3 = 0;

String m1State = "STOP";
String m2State = "STOP";
String m3State = "STOP";

// ===== STEPPER VEZÉRLÉS =====
void driveStepper(int p1, int p2, int p3, int p4, int c){
  digitalWrite(p1, pole1[c]);
  digitalWrite(p2, pole2[c]);
  digitalWrite(p3, pole3[c]);
  digitalWrite(p4, pole4[c]);
}

// ===== WEB OLDAL =====
String htmlPage(){
  String html = "<!DOCTYPE html>";
  html += "<html lang='hu'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Shivraj Control</title>";

  html += "<style>";
  html += "body{margin:0;padding:0;background-color:deepskyblue;}";
  html += "main{padding:20px;justify-items:center;border:3px solid black;border-radius:15px;background-color:rgb(0,159,212);}";
  html += "a{margin:10px;}";
  html += "h2{text-align:center;}";
  html += "button{height:75%;width:75%;}";
  html += "#megy,#mketto,#mharom{border:2px black dashed;padding:10px;margin:10px;text-align:center;}";
  html += "</style>";

  html += "</head>";
  html += "<body>";

  html += "<main>";
  html += "<h1>3 Stepper Motor Control</h1>";

  // Motor 1
  html += "<div id='megy'>";
  html += "<h2>Motor 1</h2>";
  html += "<a href='/m1cw'><button>CW</button></a>";
  html += "<a href='/m1ccw'><button>CCW</button></a>";
  html += "<a href='/m1stop'><button>STOP</button></a>";
  html += "<br><hr>";
  html += "<p>Állapot: " + m1State + "</p>";
  html += "</div>";

  // Motor 2
  html += "<div id='mketto'>";
  html += "<h2>Motor 2</h2>";
  html += "<a href='/m2cw'><button>CW</button></a>";
  html += "<a href='/m2ccw'><button>CCW</button></a>";
  html += "<a href='/m2stop'><button>STOP</button></a>";
  html += "<br><hr>";
  html += "<p>Állapot: " + m2State + "</p>";
  html += "</div>";

  // Motor 3
  html += "<div id='mharom'>";
  html += "<h2>Motor 3</h2>";
  html += "<a href='/m3cw'><button>CW</button></a>";
  html += "<a href='/m3ccw'><button>CCW</button></a>";
  html += "<a href='/m3stop'><button>STOP</button></a>";
  html += "<br><hr>";
  html += "<p>Állapot: " + m3State + "</p>";
  html += "</div>";
  html += "</main>";
  html += "</body></html>";
  
  return html;
}
void handleRoot(){
  
  server.send(200, "text/html", htmlPage());
}

// ===== PARANCSOK =====
void setupRoutes(){
  server.on("/", handleRoot);

  // MOTOR 1
  server.on("/m1cw", [](){ dir1 = 1; handleRoot();m1State = "CW";server.send(200, "text/html", htmlPage()); });
  server.on("/m1ccw", [](){ dir1 = -1; handleRoot();m1State = "CCW";server.send(200, "text/html", htmlPage()); });
  server.on("/m1stop", [](){ dir1 = 0; handleRoot();m1State = "STOP";server.send(200, "text/html", htmlPage()); });

  // MOTOR 2
  server.on("/m2cw", [](){ dir2 = 1; handleRoot();m2State = "CW";server.send(200, "text/html", htmlPage()); });
  server.on("/m2ccw", [](){ dir2 = -1; handleRoot();m2State = "CCW";server.send(200, "text/html", htmlPage()); });
  server.on("/m2stop", [](){ dir2 = 0; handleRoot();m2State = "STOP";server.send(200, "text/html", htmlPage()); });

  // MOTOR 3
  server.on("/m3cw", [](){ dir3 = 1; handleRoot();m3State = "CW";server.send(200, "text/html", htmlPage()); });
  server.on("/m3ccw", [](){ dir3 = -1; handleRoot();m3State = "CCW";server.send(200, "text/html", htmlPage()); });
  server.on("/m3stop", [](){ dir3 = 0; handleRoot();m3State = "STOP";server.send(200, "text/html", htmlPage()); });
}

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
//end of handleNotFound()

// ===== SETUP =====
void setup(){
  Serial.begin(115200);
  Serial.println("Shivraj Control");

  // PIN MODES
  int pins[] = {
    M1_Pin1,M1_Pin2,M1_Pin3,M1_Pin4,
    M2_Pin1,M2_Pin2,M2_Pin3,M2_Pin4,
    M3_Pin1,M3_Pin2,M3_Pin3,M3_Pin4
  };
  for(int i=0;i<12;i++) pinMode(pins[i], OUTPUT);

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: http://");
  Serial.println(WiFi.localIP());

  // MDNS
  if (MDNS.begin("shivraj")) {
    Serial.println("MDNS responder started");
    Serial.println("access via http://shivraj.local");
  }

  setupRoutes();
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

// ===== LOOP =====
void loop(){
  server.handleClient();

  // MOTOR 1
  if(dir1 != 0){
    step1 += dir1;
    if(step1 > 7) step1 = 0;
    if(step1 < 0) step1 = 7;
    driveStepper(M1_Pin1,M1_Pin2,M1_Pin3,M1_Pin4,step1);
  }

  // MOTOR 2
  if(dir2 != 0){
    step2 += dir2;
    if(step2 > 7) step2 = 0;
    if(step2 < 0) step2 = 7;
    driveStepper(M2_Pin1,M2_Pin2,M2_Pin3,M2_Pin4,step2);
  }

  // MOTOR 3
  if(dir3 != 0){
    step3 += dir3;
    if(step3 > 7) step3 = 0;
    if(step3 < 0) step3 = 7;
    driveStepper(M3_Pin1,M3_Pin2,M3_Pin3,M3_Pin4,step3);
  }

  delay(2);
}