#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// communication between arduino
// check schedule

#define BOTtoken "5225503031:AAFcshlLqmddsVeqWmN0oBo8Dxs_HQNo0t4"
#define CHAT_ID "1171279947"
#define EEPROM_SIZE 64
#define dispense_trigger 13
#define was_dispensed 14
#define timeout 27

const char *ssid = "LGMF2";
const char *password = "abcdefghij";

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

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

// byte dosage11= 0;
// byte dosage21= 0;
// byte dosage31= 0;
// byte dosage41= 0;
// byte dosage51= 0;
// byte dosage61= 0;
// byte dosage71= 0;
// byte dosage12= 0;
// byte dosage22= 0;
// byte dosage32= 0;
// byte dosage42= 0;
// byte dosage52= 0;
// byte dosage62= 0;
// byte dosage72= 0;

const int led = 2;
char index_html[6000];
char redirect_html[500];


class Medicine
{
public:
  byte hours = 0;
  byte minutes = 0;
  char str[16] = "";
  bool dispensed = false;
  byte amount_dispensed = 0;
  byte dosage = 0;

  Medicine()
  {
    strcpy(str, "00:00:00.000");
    return;
  }
  void set_time_from_string(const String &time_from_html)
  {
    byte len = time_from_html.length();
    int end_index;
    int substring_number;
    int start_index;

    for (end_index = 0, substring_number = 0, start_index = 0; end_index < len; end_index++)
    {
      if (time_from_html[end_index] == ':')
      {

        Serial.println(time_from_html);
        if (substring_number == 0)
        {

          hours = (time_from_html.substring(start_index, end_index)).toInt();
        }
        else if (substring_number == 1)
        {
          minutes = (time_from_html.substring(start_index, end_index)).toInt();
        }
        substring_number++;
        start_index = end_index + 1;
      }
      else if (end_index == len - 1)
      {
        // reached last index so remaing part put in string
        if (substring_number == 0)
        {
          hours = (time_from_html.substring(start_index)).toInt();
        }
        else if (substring_number == 1)
        {
          minutes = (time_from_html.substring(start_index)).toInt();
        }
      }
    }

    this->return_string_from_time();
  }
  void return_string_from_time()
  {
    snprintf(str, 16, "%.2d:%.2d:00.000", hours, minutes);
  }

  void save_to_eeprom(int addr)
  {
    EEPROM.write(addr, dosage);
    EEPROM.write(addr + 1, hours);
    EEPROM.write(addr + 2, minutes);
  }
  void get_from_eeprom(int addr)
  {
    dosage = EEPROM.read(addr);
    hours = EEPROM.read(addr + 1);
    minutes = EEPROM.read(addr + 2);

    this->return_string_from_time();
  }

  bool dispense(int &current_hours, int &current_minutes)
  {
    int total_minutes = hours * 60 + minutes;
    int current_total_minutes = current_hours * 60 + current_minutes;
    if ((current_total_minutes >= total_minutes) && (!dispensed))
    {
      // dispense
      return true;
    }
    return false;
  }
};

Medicine time11;
Medicine time21;
Medicine time31;
Medicine time41;
Medicine time51;
Medicine time61;
Medicine time71;
Medicine time12;
Medicine time22;
Medicine time32;
Medicine time42;
Medicine time52;
Medicine time62;
Medicine time72;

void send_alert()
{
  Serial.println("medicine not taken");
  bot.sendMessage(CHAT_ID, "Medicine 1 has not been taken", "");
}

void dispense_medicine(byte medicine_number, byte dosage, Medicine &time_obj)
{
  // arduino code for dispensing
  if (medicine_number == 2)
  {
    time_obj.dispensed = true;
    Serial.print("dispensing medicine ");
    Serial.print(medicine_number);
    Serial.print(" with dosage ");
    Serial.println(dosage);
    bot.sendMessage(CHAT_ID, "Medicine 2  has been dispensed and taken", "");
  }
  if (medicine_number == 1)
  {
    digitalWrite(dispense_trigger, HIGH);
    if (digitalRead(was_dispensed))
    {
      // dispensed
      time_obj.dispensed = true;
      digitalWrite(dispense_trigger, LOW);
      bot.sendMessage(CHAT_ID, "Medicine 1 has been dispensed", "");
    }
    else if (digitalRead(timeout))
    {
      time_obj.dispensed = true;
      send_alert();
      delay(1000);
      digitalWrite(dispense_trigger, LOW);
    }
  }
}

void check_schedule()
{
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int days = timeClient.getDay() != 0 ? timeClient.getDay() : 7; // 0 is sunday

  switch (days)
  {
  case 1: // monday
    if (time11.dispense(hours, minutes))
    {
      // dispense medicine
      // time11.dispensed = true;
      dispense_medicine(1, time11.dosage, time11);
    }
    if (time12.dispense(hours, minutes))
    {
      // dispense medicine
      // time12.dispensed = true;
      dispense_medicine(2, time12.dosage, time12);
    }
    break;

  case 2: // tuesday
    if (time21.dispense(hours, minutes))
    {
      // dispense medicine
      // time21.dispensed = true;
      dispense_medicine(1, time21.dosage, time21);
    }
    if (time22.dispense(hours, minutes))
    {
      // dispense medicine
      // time22.dispensed = true;
      dispense_medicine(2, time22.dosage, time22);
    }
    break;

  case 3: // wednesday
    if (time31.dispense(hours, minutes))
    {
      // dispense medicine
      // time31.dispensed = true;
      dispense_medicine(1, time31.dosage, time31);
    }
    if (time32.dispense(hours, minutes))
    {
      // dispense medicine
      // time32.dispensed = true;
      dispense_medicine(2, time32.dosage, time32);
    }
    break;

  case 4: // thursday
    if (time41.dispense(hours, minutes))
    {
      // dispense medicine
      // time41.dispensed = true;
      dispense_medicine(1, time41.dosage, time41);
    }
    if (time42.dispense(hours, minutes))
    {
      // dispense medicine
      // time42.dispensed = true;
      dispense_medicine(2, time42.dosage, time42);
    }
    break;

  case 5: // friday
    if (time51.dispense(hours, minutes))
    {
      // dispense medicine
      // time51.dispensed = true;
      dispense_medicine(1, time51.dosage, time51);
    }
    if (time52.dispense(hours, minutes))
    {
      // dispense medicine
      // time52.dispensed = true;
      dispense_medicine(2, time52.dosage, time52);
    }
    break;

  case 6: // saturday
    if (time61.dispense(hours, minutes))
    {
      // dispense medicine
      // time61.dispensed = true;
      dispense_medicine(1, time61.dosage, time61);
    }
    if (time62.dispense(hours, minutes))
    {
      // dispense medicine
      // time62.dispensed = true;
      dispense_medicine(2, time62.dosage, time62);
    }
    break;

  case 7: // sunday
    if (time71.dispense(hours, minutes))
    {
      // dispense medicine
      // time71.dispensed = true;
      dispense_medicine(1, time71.dosage, time71);
    }
    if (time72.dispense(hours, minutes))
    {
      // dispense medicine
      // time72.dispensed = true;
      dispense_medicine(2, time72.dosage, time72);
    }
    break;
  }
}

void get_values_from_eeprom()
{
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
    return;
  }
  time11.get_from_eeprom(0);
  time21.get_from_eeprom(3);
  time31.get_from_eeprom(6);
  time41.get_from_eeprom(9);
  time51.get_from_eeprom(12);
  time61.get_from_eeprom(15);
  time71.get_from_eeprom(18);
  time12.get_from_eeprom(21);
  time22.get_from_eeprom(24);
  time32.get_from_eeprom(27);
  time42.get_from_eeprom(30);
  time52.get_from_eeprom(33);
  time62.get_from_eeprom(36);
  time72.get_from_eeprom(39);
}

void save_values_to_eeprom()
{
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
    return;
  }
  time11.save_to_eeprom(0);
  time21.save_to_eeprom(3);
  time31.save_to_eeprom(6);
  time41.save_to_eeprom(9);
  time51.save_to_eeprom(12);
  time61.save_to_eeprom(15);
  time71.save_to_eeprom(18);
  time12.save_to_eeprom(21);
  time22.save_to_eeprom(24);
  time32.save_to_eeprom(27);
  time42.save_to_eeprom(30);
  time52.save_to_eeprom(33);
  time62.save_to_eeprom(36);
  time72.save_to_eeprom(39);
  EEPROM.commit();
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup(void)
{

  pinMode(led, OUTPUT);
  pinMode(dispense_trigger, OUTPUT);
  digitalWrite(dispense_trigger, LOW);
  pinMode(was_dispensed, INPUT);
  pinMode(timeout, INPUT);

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
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  digitalWrite(led, 1);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  get_values_from_eeprom();
  timeClient.begin();
  timeClient.setTimeOffset(19800);
    // declare redirect.html
  strcpy(redirect_html, R"rawliteral(<!DOCTYPE html>
<html>
   <head>
   </head>
   <body>
   <script>
   window.location.href='/';
   </script>
   </body>
</html>)rawliteral");



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
        <label for="dosage">Dosage:</label><input type="submit"><input type="button" onclick="location.href='/reset';" value="Reset" />
    </form>


    </body>
</html>
)rawliteral" ,time11.dosage,time11.str,time12.dosage,time12.str,time21.dosage,time21.str,time22.dosage,time22.str,time31.dosage,time31.str,time32.dosage,time32.str,time41.dosage,time41.str,time42.dosage,time42.str,time51.dosage,time51.str,time52.dosage,time52.str,time61.dosage,time61.str,time62.dosage,time62.str,time71.dosage,time71.str,time72.dosage,time72.str);
                request->send(200, "text/html", index_html); });

  // reset all values
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      time11.dispensed = false;
      time21.dispensed = false;
      time31.dispensed = false;
      time41.dispensed = false;
      time51.dispensed = false;
      time61.dispensed = false;
      time71.dispensed = false;
      time12.dispensed = false;
      time22.dispensed = false;
      time32.dispensed = false;
      time42.dispensed = false;
      time52.dispensed = false;
      time62.dispensed = false;
      time72.dispensed = false;
    request->send(200, "text/html", redirect_html); });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    
               
    if (request->hasParam(dosage_1_monday)) {
      time11.dosage = (request->getParam(dosage_1_monday)->value().toInt());
    }
    if (request->hasParam(dosage_1_tuesday)) {
      time21.dosage = (request->getParam(dosage_1_tuesday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_wednesday)) {
      time31.dosage = (request->getParam(dosage_1_wednesday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_thursday)) {
      time41.dosage = (request->getParam(dosage_1_thursday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_friday)) {
      time51.dosage = (request->getParam(dosage_1_friday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_saturday)) {
      time61.dosage = (request->getParam(dosage_1_saturday)->value()).toInt();
    }
    if (request->hasParam(dosage_1_sunday)) {
      time71.dosage = (request->getParam(dosage_1_sunday)->value()).toInt();
    }

    if (request->hasParam(dosage_2_monday)) {
      time12.dosage = (request->getParam(dosage_2_monday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_tuesday)) {
      time22.dosage = (request->getParam(dosage_2_tuesday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_wednesday)) {
      time32.dosage = (request->getParam(dosage_2_wednesday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_thursday)) {
      time42.dosage = (request->getParam(dosage_2_thursday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_friday)) {
      time52.dosage = (request->getParam(dosage_2_friday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_saturday)) {
      time62.dosage = (request->getParam(dosage_2_saturday)->value()).toInt();
    }
    if (request->hasParam(dosage_2_sunday)) {
      time72.dosage = (request->getParam(dosage_2_sunday)->value()).toInt();
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
//     snprintf(redirect_html, 500,  R"rawliteral(<!DOCTYPE html>
//<html>
//   <head>
//   </head>
//   <body>
//   <script>
//   window.location.href='/';
//   </script>
//   </body>
//</html>)rawliteral");



    request->send(200, "text/html", redirect_html); });

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
