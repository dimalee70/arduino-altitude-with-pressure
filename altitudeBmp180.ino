#include <SoftwareSerial.h>
#include <SFE_BMP180.h>
#include <Wire.h>


// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;
SoftwareSerial SIM800(8, 9); // 8 - RX Arduino (TX SIM800L), 9 - TX Arduino (RX SIM800L)
String _buffer;

String url = "http://178.238.78.124:9005/gsm";
String number = "87781888185";
int _timeout;
double T;
int Temp;
String imei = "";
double a;

double bspressure;
double baseline = 0; // baseline pressure

void setup()
{


  pinMode(13, OUTPUT);
    Serial.begin(9600);               // Скорость обмена данными с компьютером
  _buffer.reserve(50);
  Serial.println("Start!");
//  lcd.init();                            // Инициализация lcd             
//  lcd.backlight();                       // Включаем подсветку
//  // Курсор находится в начале 1 строки
//  lcd.print("   Hello, world!");            // Выводим текст


  SIM800.begin(9600);               // Скорость обмена данными с модемом
  SIM800.println("AT+CGSN");
  delay(1000);
  if(SIM800.available())
  {
       imei = SIM800.readStringUntil('\n');    
       imei = SIM800.readStringUntil(char(13));   
   }

  delay(1000);
  SIM800.println("AT+SAPBR=3,1,\"APN\",\"internet\"\n");
  delay(1000);
  SIM800.println("AT+SAPBR=1,1\n");
  delay(1000);
  Serial.println("REBOOT");

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 init success");

  for(int i = 0; i < 10; i++)
  {

    baseline += getPressure();
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW); 
    delay(250);
    
    }
    digitalWrite(13, HIGH);
  baseline /= 10;
  
  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");  
}

void loop()
{

   sendPressure(imei);
  delay(1000);
}
void sendPressure(String str)
{
  double pres = getPressure();
  a = pressure.altitude(pres,baseline);
  SIM800.println("AT+HTTPINIT\n");
  delay(2000);
  //String i = "860719027633449";
  //str.replace(,"");
  String urlSend = "AT+HTTPPARA=\"URL\",\"" + url + "?imei=" + str + "&temperature=" + Temp +"&pressure=" + a +"\"";
  Serial.println(urlSend);
  SIM800.println(urlSend);
//  lcd.setCursor(0, 0);
//  String lcdText = "Difference " + String(a);
//  lcd.print(lcdText);  
//  lcd.setCursor(0, 1);                   // Устанавливаем курсор в начало 2 строки
//  lcd.print("        metres"); 
  
  delay(2000);
  SIM800.println("AT+HTTPACTION=0\n");
  delay(2000);
  SIM800.println("AT+HTTPTERM\n");
  delay(2000);
  }

double getPressure()
{
  char status;
  double T,P,p0,a;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}

