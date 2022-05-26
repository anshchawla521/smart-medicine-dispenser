#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
Servo myServo1; 
Servo myServo2;

const int buzzer = 12;
const int trigPin = 7;
const int echoPin = 6;
const int dispenseStatusPin = A0;
const int dispensedStatus = A1;
const int notDispensed = A2;


int distance = 0;
int t = 0;


void setup() {
  Serial.begin(9600);
  myServo1.attach(5);
  myServo2.attach(4);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(dispenseStatusPin, INPUT_PULLUP);
  pinMode(dispensedStatus, OUTPUT);
  pinMode(notDispensed, OUTPUT);
  digitalWrite(dispensedStatus , LOW);
  digitalWrite(notDispensed , LOW);
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.home();
  myServo1.write(90);
  myServo2.write(0);
  lcd.setCursor(0,0);
  lcd.print("Hello");
  
  delay(3000);
}

  
void dispense()
{
  lcd.clear();
  lcd.home();
  lcd.print("Medicine ");
  lcd.setCursor(0,1);
  lcd.print("dispensing...");
  myServo1.write(90);
  myServo2.write(0);
  delay(1000);
  myServo2.write(90);
  delay(1000);
  myServo2.write(0);
  delay(1000);
  myServo1.write(0);
  delay(1000);
  lcd.clear();
  lcd.home();
  lcd.print("Medicine ");
  lcd.setCursor(0,1);
  lcd.print("dispensed!");
  myServo1.write(90);
  delay(2000);
  lcd.clear();
  lcd.home();
  lcd.print("Have a good day!");
  
}
int measure_distance()
{
  long duration;
  int distance = 0;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);       // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);     // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);          // Calculating the distance
  distance= duration*0.034/2;
  Serial.println(distance);
  delay(15);
  return distance>80? 80:distance;
 
}

void loop() {

  

  if (digitalRead(dispenseStatusPin))
  {
    //dispense medicine
      if (t >= 20)
  {
    digitalWrite(notDispensed , HIGH);
  }
    distance = measure_distance();

    if (distance <30 )
    {
      delay(1000);
      noTone(buzzer);
      dispense();
      digitalWrite(dispensedStatus,HIGH);
      delay(5000);//wait for 5 seconds after dispensing.
      //noTone(buzzer);
      t = 0;
    }
    else if (distance >30 && distance <60)
    { //distance greater than 30
      myServo1.write(90);
      myServo2.write(0);
      tone(buzzer , 1000);
      lcd.clear();
      lcd.home();
      lcd.print("Bring your hand");
      lcd.setCursor(0,1);
      lcd.print("closer-->");
      delay(1000);
      t++;
     }
     else if (distance >60)
     {
      
      lcd.clear();
      lcd.home();
      lcd.setCursor(0,0);
      lcd.print("TIME FOR YOUR");
      lcd.setCursor(0,1);
      lcd.print("MEDICINE");
      tone(buzzer ,2000);
      delay(1000);
      t++;
     }
  }
  else{
    //not asked to dispense
    t = 0;
    lcd.clear();
    lcd.home();
    lcd.print("SMART MEDICINE");
    lcd.setCursor(0,1);
    lcd.print("DISPENSER");
    noTone(buzzer);
    
    digitalWrite(dispensedStatus , LOW);
    digitalWrite(notDispensed , LOW);
    delay(2000); // check every 2 second for dispense.
    
  }


}
