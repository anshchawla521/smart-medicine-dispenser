#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define EEPROM_SIZE 16
const char *ssid = "LGMF2";
const char *password = "abcdefghij";

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char *dosage_1_monday = "dosage11";
const char *dosage_1_tuesday = "dosage21";
const char *dosage_1_wednesday = "dosage31";
const char *dosage_1_thursday = "dosage41";
const char *dosage_1_friday = "dosage51";
const char *dosage_1_saturday = "dosage61";
const char *dosage_1_sunday = "dosage71";
const char *dosage_2_monday = "dosage12";
const char *dosage_2_tuesday = "dosage22";
const char *dosage_2_wednesday = "dosage32";
const char *dosage_2_thursday = "dosage42";
const char *dosage_2_friday = "dosage52";
const char *dosage_2_saturday = "dosage62";
const char *dosage_2_sunday = "dosage72";


int dosage11= 0;
int dosage21= 0;
int dosage31= 0;
int dosage41= 0;
int dosage51= 0;
int dosage61= 0;
int dosage71= 0;
int dosage12= 0;
int dosage22= 0;
int dosage32= 0;
int dosage42= 0;
int dosage52= 0;
int dosage62= 0;
int dosage72= 0;

const int led = 13;
char index_html[4000];

String formattedDate;
String dayStamp;
String timeStamp;


void get_values_from_eeprom()
{
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); return;
  }
  dosage11 = EEPROM.read(0);
  dosage21 = EEPROM.read(1);
  dosage31 = EEPROM.read(2);
  dosage41 = EEPROM.read(3);
  dosage51 = EEPROM.read(4);
  dosage61 = EEPROM.read(5);
  dosage71 = EEPROM.read(6);
  dosage12 = EEPROM.read(7);
  dosage22 = EEPROM.read(8);
  dosage32 = EEPROM.read(9);
  dosage42 = EEPROM.read(10);
  dosage52 = EEPROM.read(11);
  dosage62 = EEPROM.read(12);
  dosage72 = EEPROM.read(13);
}

void save_values_to_eeprom()
{
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); return;
  }

  EEPROM.write(0,dosage11);
  EEPROM.write(1,dosage21);
  EEPROM.write(2,dosage31);
  EEPROM.write(3,dosage41);
  EEPROM.write(4,dosage51);
  EEPROM.write(5,dosage61);
  EEPROM.write(6,dosage71);
  EEPROM.write(7,dosage12);
  EEPROM.write(8,dosage22);
  EEPROM.write(9,dosage32);
  EEPROM.write(10,dosage42);
  EEPROM.write(11,dosage52);
  EEPROM.write(12,dosage62);
  EEPROM.write(13,dosage72);
  EEPROM.commit();
  
}

void get_time()
{
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  Serial.println(timeClient.getDay());
  
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void setup(void)
{
  
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    get_values_from_eeprom();
    timeClient.begin();
    timeClient.setTimeOffset(19800);
    

    

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                snprintf(index_html, 4000, R"rawliteral(<html>
    <head>
        <title> medicine dispenser</title>
    </head>
    <body>
    <style>
        body{
        background : rgba(0,0,255,0.3);
        text-align: center;
    }
    form{
        margin-top : 100px
    }
    label{
        color:white;
        
        
    }
    input{
        color:Black ;
        background-color: aqua;
        border: 1px solid aqua ;
        border-radius: 8px;
        margin:5px;
    }
    </style>
    
    <form action="/get"method="GET">
        <label for="dosage">Dosage:</label>
        <table>
        <tr class = "day">
        <th>Day</th>
        <th>Medicine 1</th>
        <th>Medicine 2</th>
        </tr> 
        <tr>
        <td>Monday</td>
        <td><input type="number" id="dosage" name="dosage11" step="1" min = 0 max = 10 value="%d"> <!-- detch from memory --></td>
        <td><input type="number" id="dosage" name="dosage12" step="1" min = 0 max = 10 value="%d"> <!-- detch from memory --></td>
        </tr>
        <tr>
        <td>Tuesday</td>
        <td><input type="number" id="dosage" name="dosage21" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        <td><input type="number" id="dosage" name="dosage22" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        </tr>
        <tr>
        <td>Wednesday</td>
        <td><input type="number" id="dosage" name="dosage31" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        <td><input type="number" id="dosage" name="dosage32" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        </tr>
        <tr>
        <td>Thursday</td>
        <td><input type="number" id="dosage" name="dosage41" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        <td><input type="number" id="dosage" name="dosage42" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        </tr>
        <tr>
        <td>Friday</td>
        <td><input type="number" id="dosage" name="dosage51" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        <td><input type="number" id="dosage" name="dosage52" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        </tr>
        <tr>
        <td>Saturday</td>
        <td><input type="number" id="dosage" name="dosage61" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        <td><input type="number" id="dosage" name="dosage62" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        </tr>
        <tr>
        <td>Sunday</td>
        <td><input type="number" id="dosage" name="dosage71" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        <td><input type="number" id="dosage" name="dosage72" step="1" min = 0 max = 10  value="%d"> <!-- detch from memory --></td>
        </tr>
        </table>
        <input type="submit">
    </form>


    </body>
</html>
)rawliteral",dosage11,dosage12,dosage21,dosage22,dosage31,dosage32,dosage41,dosage42,dosage51,dosage52,dosage61,dosage62,dosage71,dosage72);

                request->send(200, "text/html", index_html); });

    // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    
    
    if (request->hasParam(dosage_1_monday)) {
      dosage11 = (request->getParam(dosage_1_monday)->value().toInt());
    }
    if (request->hasParam(dosage_1_tuesday)) {
      dosage21 = (request->getParam(dosage_1_tuesday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_wednesday)) {
      dosage31 = (request->getParam(dosage_1_wednesday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_thursday)) {
      dosage41 = (request->getParam(dosage_1_thursday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_friday)) {
      dosage51 = (request->getParam(dosage_1_friday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_saturday)) {
      dosage61 = (request->getParam(dosage_1_saturday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_sunday)) {
      dosage71 = (request->getParam(dosage_1_sunday)->value()).toInt();
    }

    if (request->hasParam(dosage_2_monday)) {
      dosage12 = (request->getParam(dosage_2_monday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_tuesday)) {
      dosage22 = (request->getParam(dosage_2_tuesday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_wednesday)) {
      dosage32 = (request->getParam(dosage_2_wednesday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_thursday)) {
      dosage42 = (request->getParam(dosage_2_thursday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_friday)) {
      dosage52 = (request->getParam(dosage_2_friday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_saturday)) {
      dosage62 = (request->getParam(dosage_2_saturday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_sunday)) {
      dosage72 = (request->getParam(dosage_2_sunday)->value()).toInt();
    }

    
    save_values_to_eeprom();
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field "); });
    server.onNotFound(notFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
  get_time();
  delay(1000);
}
