#include <M5Stack.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
WiFiMulti wifiMulti;
const char* ssid     = "xxxxx";
const char* password = "xxxxx";
uint8_t buff_pic[30000];
uint16_t buff_len=0;

unsigned long previousMillis = 0;
const long interval = 5000;

unsigned char ZoomMode=15;
String MapType = "roadmap";


void setup() 
{
   USE_SERIAL.begin(115200);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    M5.begin();
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
    M5.Lcd.setBrightness(255);
    M5.update();
    header("Initial System",TFT_NAVY );
    M5.Lcd.setFreeFont(&FreeSansBold9pt7b);  
    WiFi.begin(ssid, password);
    lcd_println("");
    lcd_println("");
    lcd_println("");
    lcd_println("Connect AP");
     while (WiFi.status() != WL_CONNECTED) 
     {
        delay(500);
        Serial.print(".");
        lcd_print(".");
    } 
    Serial.println("");
    Serial.println("");
    Serial.println("WiFi connected");
    lcd_println("");
    lcd_println("WiFi connected");
    Serial.println("IP address: ");
    lcd_print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd_println(WiFi.localIP().toString());
    delay(5000);
    lcd_println("Connect Server");

}

void loop() 
{
  Get_GoogleMAP("13.840888","100.542095",15,"roadmap");
  delay(1000);
}
String  GennerateGet(String latitude,String longitude,unsigned char zoom,String maptype)
{
  String data = "http://maps.googleapis.com/maps/api/staticmap?";
  data += "center="+latitude+","+longitude;
  data += "&format=jpg-baseline";
  data += "&zoom="+String(zoom);
  data += "&size=320x240";
  data += "&maptype="+maptype;
  data += "&markers=color:red%7Clabel:S%7C"+latitude+","+longitude;
  return(data);
}

void Get_GoogleMAP(String latitude,String longitude,unsigned char ZoomMode,String MapType)
{
  
  if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        USE_SERIAL.print("[HTTP] begin...\n");
        http.begin(GennerateGet(latitude,longitude,ZoomMode,MapType));
       
        USE_SERIAL.print("[HTTP] GET...\n");
        int httpCode = http.GET();
        if(httpCode > 0) {
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
             if(httpCode == HTTP_CODE_OK) {
                int len = http.getSize();
                uint8_t buff[128] = { 0 };
                WiFiClient * stream = http.getStreamPtr();
                buff_len=0;
                while(http.connected() && (len > 0 || len == -1)) {
                    size_t size = stream->available();

                    if(size) {
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        for(int i=0;i<c;i++)
                       {
                           buff_pic[buff_len] = buff[i];
                           buff_len++;
                       }
                        if(len > 0) {
                            len -= c;
                        }
                    }
                    delay(1);
                }
                 M5.Lcd.drawJpg(buff_pic, buff_len);
                USE_SERIAL.println();
                USE_SERIAL.print("[HTTP] connection closed or file end.\n");

            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
}




void header(const char *string, uint16_t color)
{
  M5.Lcd.fillScreen(color);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_MAGENTA, TFT_BLUE);
  M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLUE);
  M5.Lcd.setTextDatum(TC_DATUM);
  M5.Lcd.drawString(string, 160, 2, 4); // Font 4 for fast drawing with background
}
void lcd_print(String str)
{
  M5.Lcd.print(" ");
  M5.Lcd.print(str);
}
void lcd_println(String str)
{
  M5.Lcd.print(" ");
  M5.Lcd.println(str);
}

