/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6XeeayPwp"
#define BLYNK_TEMPLATE_NAME "GianPhoiThongMinh"
#define BLYNK_AUTH_TOKEN "mkoDEjaeElshnN01lPlyjZqju_Lys0Rp"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define IR1_PIN 5
#define IR2_PIN 18
#define RAIN_SS_PIN 19
#define RELAY_PIN 17
#define LIGHT_SS_PIN 16
#define DHTPIN 4      // Đặt chân GPIO kết nối với cảm biến DHT11
#define DHTTYPE DHT11 // Loại cảm biến DHT11
#define IN1_L298_PIN 23
#define IN2_L298_PIN 15
#define ENA_L298_PIN 12

// setting PWM properties
const int freq = 20000; // frequency control Dc motor 20kHz- 100kHz
const int ledChannel1 = 0;
const int resolution = 10;

int menu_state = 0;
int stt_sayDo;
int stt_noiPhoi;


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Hang_2.4G";
char pass[] = "0948315735";

DHT dht(DHTPIN, DHTTYPE);
void MotorOut();
void MotorIn();
void AutoMode();
void ManuMode();
void SayDo();
void ThoiTiet();
bool CheckRaining();
bool CheckSunrise();

void setup()
{
  Serial.begin(115200);
  // Setup pin
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(RAIN_SS_PIN, INPUT);
  pinMode(LIGHT_SS_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(IN1_L298_PIN, OUTPUT);
  pinMode(IN2_L298_PIN, OUTPUT);
  pinMode(ENA_L298_PIN, OUTPUT);
  ////
  // configure LED PWM functionalitites
  ledcSetup(ledChannel1, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ENA_L298_PIN, ledChannel1);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  // You can also specify server:
  // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
  
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  if (menu_state == 0)
  {
    ManuMode();
    
    
  }
  else if (menu_state == 1)
  {
    AutoMode();
   
    
  }
  ThoiTiet();
}

bool CheckRaining()
{
  if (digitalRead(RAIN_SS_PIN) == 0) // neu mua
    return true;
  return false;
}

bool CheckSunrise()
{
  if (digitalRead(LIGHT_SS_PIN) == 0) // neu sang
    return true;
  return false;
}

bool CheckInDoor()
{
  if (digitalRead(IR1_PIN) == 0) // kiem tra xem da trong nha chua
  {
    return true;
  }
  return false;
}

bool CheckOutDoor()
{
  if (digitalRead(IR2_PIN) == 0) // kiem tra xem da ngoai troi chua
  {
    return true;
  }
  return false;
}

void AutoMode()
{
  Blynk.virtualWrite(V5, LOW);
  Blynk.virtualWrite(V4, HIGH);
  if (CheckRaining() == false && CheckSunrise() == true) // khong mua, co anh sang nang, dang o trong nha thi cho ra ngoai
  {
    MotorOut();
    Blynk.virtualWrite(V6, HIGH);
  }
  else if (CheckRaining() == true || CheckSunrise() == false ) // co mua, hoac troi toi va dang o ngoai troi thi cho vao nha
  {
    MotorIn();
    Blynk.virtualWrite(V6, LOW);
  }
  SayDo();
}

void ManuMode()
{
  Blynk.virtualWrite(V5, HIGH);
  Blynk.virtualWrite(V4, LOW);
  if (stt_noiPhoi == 1 )
  {
    if (CheckOutDoor() == true)
    {
      Blynk.virtualWrite(V6, HIGH);
    }
      
       MotorOut();
  }
  else if (stt_noiPhoi == 0 )
  {
    if (CheckInDoor()== false)
    {
      Blynk.virtualWrite(V6, LOW);
    }
    MotorIn();
  }
  SayDo();
}

void SayDo()
{
  if (stt_sayDo == 1 && CheckInDoor()) // che do thu cong
  {
    digitalWrite(RELAY_PIN, HIGH); // bat
  }
  else if (stt_sayDo == 0 || CheckInDoor() == false)
  {
    digitalWrite(RELAY_PIN, LOW);
  }
}

void ThoiTiet(){
   unsigned long long preTimer1;
   unsigned long long currTimer1 = millis();

   if (currTimer1 - preTimer1 >= 1000){
    int hum = dht.readHumidity();
    int temp = dht.readTemperature();
    Blynk.virtualWrite(V7, temp);
    Blynk.virtualWrite(V8, hum);
    preTimer1 = currTimer1;
   }  
   if (CheckRaining())
   {
    Blynk.virtualWrite(V9, HIGH);
   }else Blynk.virtualWrite(V9, LOW);
   

    
}

void MotorIn()
{ // cho quan ao vao nha
  digitalWrite(IN1_L298_PIN, HIGH);
  digitalWrite(IN2_L298_PIN, LOW);

  if (CheckInDoor() == true)
  {
    ledcWrite(ledChannel1,100);
    Serial.println(digitalRead(IR1_PIN));
  }
  else
  {
    ledcWrite(ledChannel1,700);
    Serial.println("Dang chay vao");
  }
}

void MotorOut()
{ // cho quan ao ra ngoai troi
  digitalWrite(IN1_L298_PIN, LOW);
  digitalWrite(IN2_L298_PIN, HIGH);
  if (CheckOutDoor() == true)
  {
    ledcWrite(ledChannel1,100);
    Serial.println(digitalRead(IR2_PIN));
    
  }
  else
  {
    ledcWrite(ledChannel1, 700);
    Serial.println("Dang chay ra");
  }
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V6);
}

BLYNK_WRITE(V0)
{ // manu button
  int stt = param.asInt();
  if (stt)
  {
    menu_state = 0;
  }
}
BLYNK_WRITE(V1)
{ // auto button
  int stt = param.asInt();
  if (stt)
  {
    menu_state = 1;
  }
}
BLYNK_WRITE(V2)
{ // noi phoi
  int stt = param.asInt();
  if (stt)
  {
    stt_noiPhoi = 1;
  }
  else
    stt_noiPhoi = 0;
}
BLYNK_WRITE(V3)
{ // che do say
  int stt = param.asInt();
  if (stt)
  {
    stt_sayDo = 1;
  }
  else
    stt_sayDo = 0;
}