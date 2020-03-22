#include <Arduino.h>
#include <App.hpp>
#include <BoardConfig.hpp>
#include <Bitmaps.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <OtaHandler.hpp>

byte mac[6];

Adafruit_ILI9341 tft =
    Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void connectWiFi()
{
  tft.println("Connecting to: ");
  tft.println(" ");
  tft.println(WIFI_SSID);
  digitalWrite(TFT_BL, true);

  Serial.print(PSTR("\nConnecting to WiFi network " WIFI_SSID " "));

  WiFi.begin();
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(OTA_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
    tft.drawBitmap(70, 50, wifi_1, 100, 100, ILI9341_WHITE);
    delay(100);
    tft.drawBitmap(70, 50, wifi_2, 100, 100, ILI9341_WHITE);
    delay(100);
    tft.drawBitmap(70, 50, wifi_3, 100, 100, ILI9341_WHITE);
    delay(100);
    tft.fillRect(70, 50, 100, 100, ILI9341_BLACK);
  }

  Serial.println(PSTR(" connected.\n"));

  WiFi.macAddress(mac);
  Serial.printf(PSTR("WiFi MAC Address    : %02X:%02X:%02X:%02X:%02X:%02X\n"), mac[0],
                mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.printf(PSTR("WiFi Hostname       : %s\n"), WiFi.getHostname());
  Serial.print(PSTR("WiFi IP-Address     : "));
  Serial.println(WiFi.localIP());
  Serial.print(PSTR("WiFi Gateway-IP     : "));
  Serial.println(WiFi.gatewayIP());
  Serial.print(PSTR("WiFi Subnetmask     : "));
  Serial.println(WiFi.subnetMask());
  Serial.print(PSTR("WiFi DNS Server     : "));
  Serial.println(WiFi.dnsIP());
  Serial.println();

  digitalWrite(TFT_BL, false);
  tft.fillScreen(ILI9341_BLACK);
}

void setup()
{
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  pinMode(BUTTON_L, INPUT_PULLUP);
  pinMode(BUTTON_M, INPUT_PULLUP);
  pinMode(BUTTON_R, INPUT_PULLUP);

  delay(3000); // wait for serial monitor
  Serial.println(PSTR("\n\n\n" APP_NAME " - Version " APP_VERSION
                      " by Dr. Thorsten Ludewig"));
  Serial.println(PSTR("Build date: " __DATE__ " " __TIME__ "\n"));

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextWrap(true);
  tft.setCursor(0, 170);
  tft.setTextSize(2);

  connectWiFi();
  InitializeOTA();
}

void loop()
{
  ArduinoOTA.handle();
  delay(20);
}
