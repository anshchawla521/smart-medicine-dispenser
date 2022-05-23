#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


//communication between arduino
//check schedule
#define EEPROM_SIZE 64
#define dispense_trigger 13
#define was_dispensed 14
#define timeout 27


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

const int led = 2;
char index_html[6000];
int day_current = 0;


class my_time{
  public:
  byte hours;
  byte minutes;
  char str[16]= "";
  bool dispensed = false;
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

    this->return_string_from_time();
  }
void return_string_from_time(){
  snprintf(str,16,"%.2d:%.2d:00.000",hours,minutes);
}

void save_to_eeprom(int addr)
{
  EEPROM.write(addr,byte(hours));
  EEPROM.write(addr+1,byte(minutes));
}
void get_from_eeprom(int addr){
  hours = EEPROM.read(addr);
  minutes = EEPROM.read(addr+1);
  
  this->return_string_from_time();
}

bool dispense(int &current_hours , int &current_minutes)
{
  int total_minutes = hours*60 + minutes;
  int current_total_minutes = current_hours*60 + current_minutes;
  if ((current_total_minutes >= total_minutes)&&(!dispensed)){
    //dispense    
    return true;
  }
  return false;
  
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

void send_alert()
{
  Serial.println("medicine not taken");
}

void dispense_medicine(byte medicine_number , byte &dosage , my_time &time_obj)
{
  //arduino code for dispensing
  if (medicine_number == 2){
  time_obj.dispensed = true;
  Serial.print("dispensing medicine "); 
  Serial.print(medicine_number);
  Serial.print(" with dosage ");
  Serial.println(dosage);
  }
  if (medicine_number == 1)
  {
    digitalWrite(dispense_trigger , HIGH);
    if (digitalRead(was_dispensed)){
      // dispensed 
      time_obj.dispensed = true;
      digitalWrite(dispense_trigger , LOW);
    }else if (digitalRead(timeout)){
      send_alert();
    }
  }
}



void check_schedule()
{
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int days= timeClient.getDay() != 0 ? timeClient.getDay():7; // 0 is sunday

  switch(days){
    case 1: //monday
      if(time11.dispense(hours , minutes))
      {
        //dispense medicine
        //time11.dispensed = true;
        dispense_medicine(1 , dosage11 , time11);
      }
      if(time12.dispense(hours , minutes))
      {
        //dispense medicine
        //time12.dispensed = true;
        dispense_medicine(2 , dosage12 , time12);
      }
      break;

      case 2: //tuesday
      if(time21.dispense(hours , minutes))
      {
        //dispense medicine
        //time21.dispensed = true;
        dispense_medicine(1 , dosage21 , time21);
      }
      if(time22.dispense(hours , minutes))
      {
        //dispense medicine
        //time22.dispensed = true;
        dispense_medicine(2 , dosage22 , time22);
      }
      break;

      case 3: //wednesday
      if(time31.dispense(hours , minutes))
      {
        //dispense medicine
        //time31.dispensed = true;
        dispense_medicine(1 , dosage31 , time31);
      }
      if(time32.dispense(hours , minutes))
      {
        //dispense medicine
        //time32.dispensed = true;
        dispense_medicine(2 , dosage32 , time32);
      }
      break;

      case 4: //thursday
      if(time41.dispense(hours , minutes))
      {
        //dispense medicine
        //time41.dispensed = true;
        dispense_medicine(1 , dosage41 , time41);
      }
      if(time42.dispense(hours , minutes))
      {
        //dispense medicine
        //time42.dispensed = true;
        dispense_medicine(2 , dosage42 , time42);
      }
      break;

      case 5: //friday      
      if(time51.dispense(hours , minutes))
      {
        //dispense medicine
        //time51.dispensed = true;
        dispense_medicine(1 , dosage51 , time51);
      }
      if(time52.dispense(hours , minutes))
      {
        //dispense medicine
        //time52.dispensed = true;
        dispense_medicine(2 , dosage52 , time52);
      }
      break;

      case 6: //saturday
      if(time61.dispense(hours , minutes))
      {
        //dispense medicine
        //time61.dispensed = true;
        dispense_medicine(1 , dosage61 , time61);
      }
      if(time62.dispense(hours , minutes))
      {
        //dispense medicine
        //time62.dispensed = true;
        dispense_medicine(2 , dosage62 , time62);
      }
      break;

      case 7: //sunday
      if(time71.dispense(hours , minutes))
      {
        //dispense medicine
        //time71.dispensed = true;
        dispense_medicine(1 , dosage71 , time71);
      }
      if(time72.dispense(hours , minutes))
      {
        //dispense medicine
        //time72.dispensed = true;
        dispense_medicine(2 , dosage72 , time72);
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
  time11.get_from_eeprom(14);
  time21.get_from_eeprom(16);
  time31.get_from_eeprom(18);
  time41.get_from_eeprom(20);
  time51.get_from_eeprom(22);
  time61.get_from_eeprom(24);
  time71.get_from_eeprom(26);
  time12.get_from_eeprom(28);
  time22.get_from_eeprom(30);
  time32.get_from_eeprom(32);
  time42.get_from_eeprom(34);
  time52.get_from_eeprom(36);
  time62.get_from_eeprom(38);
  time72.get_from_eeprom(40);
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
  time11.save_to_eeprom(14);
  time21.save_to_eeprom(16);
  time31.save_to_eeprom(18);
  time41.save_to_eeprom(20);
  time51.save_to_eeprom(22);
  time61.save_to_eeprom(24);
  time71.save_to_eeprom(26);
  time12.save_to_eeprom(28);
  time22.save_to_eeprom(30);
  time32.save_to_eeprom(32);
  time42.save_to_eeprom(34);
  time52.save_to_eeprom(36);
  time62.save_to_eeprom(38);
  time72.save_to_eeprom(40);
  EEPROM.commit();
  
}



void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void setup(void)
{
  
    pinMode(led, OUTPUT);
    pinMode(dispense_trigger , OUTPUT);
    digitalWrite(dispense_trigger, LOW);
    pinMode(was_dispensed , INPUT);
    pinMode(timeout , INPUT);

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
    digitalWrite(led, 1);
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

    
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field "); });
    
    
    server.onNotFound(notFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
  timeClient.update();
  check_schedule();
  delay(1000);
}
