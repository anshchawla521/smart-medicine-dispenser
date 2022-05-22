#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


//save time to eeprom
//communication between arduino
//check schedule
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


byte dosage11= 0;
byte dosage21= 0;
byte dosage31= 0;
byte dosage41= 0;
byte dosage51= 0;
byte dosage61= 0;
byte dosage71= 0;
byte dosage12= 0;
byte dosage22= 0;
byte dosage32= 0;
byte dosage42= 0;
byte dosage52= 0;
byte dosage62= 0;
byte dosage72= 0;

const int led = 13;
char index_html[6000];
int day_current = 0;


class my_time{
  public:
  byte hours;
  byte minutes;
  char str[16]= "";
  my_time()
  {
    hours = 0;
    minutes = 0;
    strcpy(str , "00:00:00.000" );
  }
void set_time_from_string(const String &time_from_html)
  {
    byte len = time_from_html.length();
    int end_index;
    int substring_number ;
    int start_index;
    
    for (end_index = 0 , substring_number = 0 , start_index = 0;end_index < len ; end_index++)
    {
      if(time_from_html[end_index] == ':')
      {
        
         Serial.println(time_from_html);
        if (substring_number == 0)
        {
          
          hours = (time_from_html.substring(start_index , end_index)).toInt();
        }else if (substring_number == 1)
        {
          minutes = (time_from_html.substring(start_index , end_index)).toInt();
        }
        substring_number++;
        start_index = end_index + 1;
      }else if(end_index == len-1)
      {
        //reached last index so remaing part put in string
        if (substring_number == 0)
        {
          hours = (time_from_html.substring(start_index )).toInt();
        }else if (substring_number == 1)
        {
          minutes = (time_from_html.substring(start_index)).toInt();
        }
      }
    }
    Serial.println("hello1");
    this->return_string_from_time();
    Serial.println("hello2");
  }
void return_string_from_time(){
  snprintf(str,16,"%.2d:%.2d:00.000",hours,minutes);
}
  
};

my_time time11;
my_time time21;
my_time time31;
my_time time41;
my_time time51;
my_time time61;
my_time time71;
my_time time12;
my_time time22;
my_time time32;
my_time time42;
my_time time52;
my_time time62;
my_time time72;


//void write_str_to_eeprom(int addr , const String &strtowrite)
//{
//  byte len = strtowrite.length();
//  EEPROM.write(addr , len);
//  for (int i = 0 ; i < len ; i++)
//  {
//    EEPROM.write(addr +1+i, strtowrite[i]);
//  }
//}
//String read_str_from_eeprom(int addr){
//  byte len = EEPROM.read(addr);
//  char my_string[len+1];
//  for(int i= 0; i< len ; i++)
//  {
//    my_string[i] = EEPROM.read(i+1+addr)  ;
//  }
//  my_string[len] = '\0';
//
//  return String(my_string);
//}


void check_schedule()
{
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int days= timeClient.getDay() != 0 ? timeClient.getDay():7; // 0 is sunday

  switch(days){
    case 1: //monday
      if(time11.hours>hours && time11.minutes>minutes && medicine not taken )
      {
        //send arduino signal to give
      }
      break;


    
  }
  
  
}

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
                snprintf(index_html, 6000, R"rawliteral(<html>
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
        <table>
        <tr class = "day">
        <th>Day</th>
        <th>Medicine 1</th>
        <th>Medicine 2</th>
        </tr> 
        <tr>
        <td>Monday</td>
        <td><input type="number" id="dosage" name="dosage11" step="1" min = 0 max = 10 value="%d">
            <input type="time" name="time11" value = "%s"></td>
        <td><input type="number" id="dosage" name="dosage12" step="1" min = 0 max = 10 value="%d">
            <input type="time" name="time12" value = "%s"></td>
        </tr>
        <tr>
        <td>Tuesday</td>
        <td><input type="number" id="dosage" name="dosage21" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time21" value = "%s"></td>
        <td><input type="number" id="dosage" name="dosage22" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time22" value = "%s"></td>
        </tr>
        <tr>
        <td>Wednesday</td>
        <td><input type="number" id="dosage" name="dosage31" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time31" value = "%s"></td>
        <td><input type="number" id="dosage" name="dosage32" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time32" value = "%s"></td>
        </tr>
        <tr>
        <td>Thursday</td>
        <td><input type="number" id="dosage" name="dosage41" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time41" value = "%s"></td>
        <td><input type="number" id="dosage" name="dosage42" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time42" value = "%s"></td>
        </tr>
        <tr>
        <td>Friday</td>
        <td><input type="number" id="dosage" name="dosage51" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time51" value = "%s"></td>
        <td><input type="number" id="dosage" name="dosage52" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time52" value = "%s"></td>
        </tr>
        <tr>
        <td>Saturday</td>
        <td><input type="number" id="dosage" name="dosage61" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time61" value = "%s"></td>
        <td><input type="number" id="dosage" name="dosage62" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time62" value = "%s"></td>
        </tr>
        <tr>
        <td>Sunday</td>
        <td><input type="number" id="dosage" name="dosage71" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time71" value = "%s"></td>
        <td><input type="number" id="dosage" name="dosage72" step="1" min = 0 max = 10  value="%d">
            <input type="time" name="time72" value = "%s"></td>
        </tr>
        </table>
        <label for="dosage">Dosage:</label><input type="submit">
    </form>


    </body>
</html>
)rawliteral" ,dosage11,time11.str,dosage12,time12.str,dosage21,time21.str,dosage22,time22.str,dosage31,time31.str,dosage32,time32.str,dosage41,time41.str,dosage42,time42.str,dosage51,time51.str,dosage52,time52.str,dosage61,time61.str,dosage62,time62.str,dosage71,time71.str,dosage72,time72.str);
for ( int i = 0 ; i < 4000 ; i++)
    {
      Serial.print(index_html[i]);
    }
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

    if (request->hasParam("time11")) {
      
      time11.set_time_from_string(request->getParam("time11")->value());
    }
    if (request->hasParam("time21")) {
      
      time21.set_time_from_string(request->getParam("time21")->value());
    }
    if (request->hasParam("time31")) {
      
      time31.set_time_from_string(request->getParam("time31")->value());
    }
    if (request->hasParam("time41")) {
      
      time41.set_time_from_string(request->getParam("time41")->value());
    }
    if (request->hasParam("time51")) {
      
      time51.set_time_from_string(request->getParam("time51")->value());
    }
    if (request->hasParam("time61")) {
      
      time61.set_time_from_string(request->getParam("time61")->value());
    }
    if (request->hasParam("time71")) {
      
      time71.set_time_from_string(request->getParam("time71")->value());
    }
    if (request->hasParam("time12")) {
      
      time12.set_time_from_string(request->getParam("time12")->value());
    }
    if (request->hasParam("time22")) {
      
      time22.set_time_from_string(request->getParam("time22")->value());
    }
    if (request->hasParam("time32")) {
      
      time32.set_time_from_string(request->getParam("time32")->value());
    }
    if (request->hasParam("time42")) {
      
      time42.set_time_from_string(request->getParam("time42")->value());
    }
    if (request->hasParam("time52")) {
      
      time52.set_time_from_string(request->getParam("time52")->value());
    }
    if (request->hasParam("time62")) {
      
      time62.set_time_from_string(request->getParam("time62")->value());
    }
    if (request->hasParam("time72")) {
      
      time72.set_time_from_string(request->getParam("time72")->value());
    }

    
    save_values_to_eeprom();
    Serial.println(time11.hours);
    Serial.println(time11.minutes);
    
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field "); });
    
    
    server.onNotFound(notFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
  timeClient.update();
  delay(1000);
}
