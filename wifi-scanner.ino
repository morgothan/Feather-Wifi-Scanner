#include "ESP8266WiFi.h"
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const int  buttonPin = 2;
const char* host = "www.heyshutup.com";

int buttonPushCounter = 0;
int buttonState = 0;
int lastButtonState = 0;



void setup() {
  Serial.begin(115200);

  pinMode(0, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  //  Serial.println("Setup done");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);

  display.clearDisplay();


  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Setup Complete!");
  display.display();

}

void loop() {
  display.setCursor(0, 0);
  //  Serial.println("scan start");
  display.clearDisplay();
  display.println("Scan Starting!");
  display.display();


  int n = WiFi.scanNetworks();
  //  Serial.println("scan done");
  display.println("Scan Finished!");
  display.display();
  if (n == 0)
  {
    //    Serial.println("no networks found");
    display.println("No Networks Found.");
    display.display();
  }
  else
  {
    //    Serial.print(n);
    //    Serial.println(" networks found");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(n);
    display.println(" Networks Found!");
    display.println("Looking for Open WiFi");
    display.display();
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      display.setCursor(0, 16);
      display.setTextColor(BLACK);
      display.print("Testing: ");
      display.print(i - 1);
      display.setCursor(0, 16);
      display.setTextColor(WHITE);
      display.print("Testing: ");
      display.print(i);
      display.display();
      /*      Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" NONE":"*");*/
      if (WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        char ssid[50];
        WiFi.SSID(i).toCharArray(ssid, 50);
        WiFi.begin(ssid);
        int count = 0;
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          count++;
          //           Serial.print(".");
          if (count % 4 == 0)
          {
            display.setCursor(75, 16);
            display.setTextColor(BLACK);
            display.print("/");
            display.setCursor(75, 16);
            display.setTextColor(WHITE);
            display.print("-");
          }
          else if (count % 4 == 1)
          {
            display.setCursor(75, 16);
            display.setTextColor(BLACK);
            display.print("-");
            display.setCursor(75, 16);
            display.setTextColor(WHITE);
            display.print("\\");
          }
          else if (count % 4 == 2)
          {
            display.setCursor(75, 16);
            display.setTextColor(BLACK);
            display.print("\\");
            display.setCursor(75, 16);
            display.setTextColor(WHITE);
            display.print("|");
          }
          else
          {
            display.setCursor(75, 16);
            display.setTextColor(BLACK);
            display.print("|");
            display.setCursor(75, 16);
            display.setTextColor(WHITE);
            display.print("/");
          }
          display.display();
          if (count >= 100) {
            //           Serial.println("Failed to Connect");
            break;
          }
        }
        if (WiFi.status() == WL_CONNECTED) {
          /*           Serial.println("WiFi connected");
                     Serial.println("IP address: ");
                     Serial.println(WiFi.localIP());*/
          display.clearDisplay();
          display.setCursor(0, 0);
          display.print(WiFi.SSID(i));
          display.print(" ");
          display.println(WiFi.RSSI(i));
          display.print("IP: ");
          display.println(WiFi.localIP());
          display.display();
          // Use WiFiClient class to create TCP connections
          WiFiClient client;
          const int httpPort = 80;
          if (!client.connect(host, httpPort)) {
            //                Serial.println("connection failed");
            display.setCursor(0, 25);
            display.println("Limited Access");
            return;
          }

          // We now create a URI for the request
          String url = "/live";
          /*              Serial.print("Requesting URL: ");
                        Serial.println(url);*/
          // This will send the request to the server
          client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Connection: close\r\n\r\n");
          delay(500);

          // Read all the lines of the reply from server and see if we have access
          while (client.available()) {
            String line = client.readStringUntil('\n');
            //              Serial.print(line);
            if (line.substring(0, 4) == "live") {
              //                Serial.print("******* OPEN WIFI FOUND ********");
              display.setCursor(0, 25);
              display.println("Open Access!!!!");
              display.display();
              delay(500);
            }
          }
          WiFi.disconnect();
          while (WiFi.status() == WL_CONNECTED) {
            delay(500);
            //            Serial.print(".");
          }
          //          Serial.println("WiFi disconnected");
        }

      }
      delay(10);
    }
  }
  //Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
}
