#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>  //https://github.com/bbx10/WebServer_tng
#include <SPIMemory.h> //https://github.com/Marzogh/SPIMemory

const char* ssid = "...";
const char* pass = "...";

WebServer server ( 80 );
SPIFlash flash;

void setup() {
  Serial.begin(115200);
  flash.begin();
  connectWifi();
  startServer();
}

void loop() {
  server.handleClient();

}

void connectWifi(){
  WiFi.enableSTA(true);
  
  delay(2000);

  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void startServer(){

  server.on("/", handleRoot);
  server.on("/write", handleWrite);
  server.on("/read", handleRead);
  server.begin();
  Serial.println ( "HTTP server started\n" );

}

void handleRead(){
  String outString;
  if (flash.readStr(0, outString)) {
    Serial.print(outString);
    server.send(200, "text/html", outString);
  } else {
    server.send(200, "text/html", "Nada");
  }
}

void handleWrite(){
  if (server.hasArg("String")){
    String data = server.arg("String");
    flash.eraseSection(0, 32);
    flash.writeStr(0, data);
    Serial.print(data);
    server.send(200, "text/html", "Enviado!!<br><a href='/read'>Read</a>");
  }
}

void handleNotFound(){
  Serial.print("Not found \n");
  server.send(200, "text/html", "???");
}

void handleRoot(){
  Serial.print("Request /\n");
  server.send(200, "text/html", getPage());

}

String getPage(){
  String page = "<form action='/write' method='POST'>";
  page+="<input type='text' placeholder='write in memory' name='String'>";
  page+="<input type='submit' value='Send'>";
  page+="</form>";
  return page;
} 