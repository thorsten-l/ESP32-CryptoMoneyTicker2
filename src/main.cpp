#include <Arduino.h>
#include <App.hpp>
#include <BoardConfig.hpp>
#include <Bitmaps.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <OtaHandler.hpp>

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

byte mac[6];
struct tm timeinfo;

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

  configTzTime(TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

  if (getLocalTime(&timeinfo, 10000)) // wait up to 10sec to sync
  {
    Serial.println(&timeinfo, PSTR("Time set            : %A %d %B %Y %H:%M:%S"));
    Serial.print( PSTR("Timezone            : "));
    Serial.println(getenv("TZ"));
  }
  else
  {
    Serial.println(PSTR("Time not set"));
  }

  InitializeOTA();
}

/*
  Issuer: C=SE, O=AddTrust AB, OU=AddTrust External TTP Network, CN=AddTrust External CA Root
  Validity
    Not Before: May 30 10:48:38 2000 GMT
    Not After : May 30 10:48:38 2020 GMT <-- i'm waiting for the new root cert
  s:/C=GB/ST=Greater Manchester/L=Salford/O=COMODO CA Limited/CN=COMODO ECC Certification Authority
  i:/C=SE/O=AddTrust AB/OU=AddTrust External TTP Network/CN=AddTrust External CA Root

static const char* root_ca=
"-----BEGIN CERTIFICATE-----\n"
"MIID0DCCArigAwIBAgIQQ1ICP/qokB8Tn+P05cFETjANBgkqhkiG9w0BAQwFADBv\n"
"MQswCQYDVQQGEwJTRTEUMBIGA1UEChMLQWRkVHJ1c3QgQUIxJjAkBgNVBAsTHUFk\n"
"ZFRydXN0IEV4dGVybmFsIFRUUCBOZXR3b3JrMSIwIAYDVQQDExlBZGRUcnVzdCBF\n"
"eHRlcm5hbCBDQSBSb290MB4XDTAwMDUzMDEwNDgzOFoXDTIwMDUzMDEwNDgzOFow\n"
"gYUxCzAJBgNVBAYTAkdCMRswGQYDVQQIExJHcmVhdGVyIE1hbmNoZXN0ZXIxEDAO\n"
"BgNVBAcTB1NhbGZvcmQxGjAYBgNVBAoTEUNPTU9ETyBDQSBMaW1pdGVkMSswKQYD\n"
"VQQDEyJDT01PRE8gRUNDIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MHYwEAYHKoZI\n"
"zj0CAQYFK4EEACIDYgAEA0d7L3XJghWF+3XkkRbUq2KZ9T5SCwbOQQB/l+EKJDwd\n"
"AQTuPdKNCZcM4HXk+vt3iir1A2BLNosWIxatCXH0SvQoULT+iBxuP2wvLwlZW6Vb\n"
"CzOZ4sM9iflqLO+y0wbpo4H+MIH7MB8GA1UdIwQYMBaAFK29mHo0tCb3+sQmVO8D\n"
"veAky1QaMB0GA1UdDgQWBBR1cacZSBm8nZ3qQUfflMRId5nTeTAOBgNVHQ8BAf8E\n"
"BAMCAYYwDwYDVR0TAQH/BAUwAwEB/zARBgNVHSAECjAIMAYGBFUdIAAwSQYDVR0f\n"
"BEIwQDA+oDygOoY4aHR0cDovL2NybC50cnVzdC1wcm92aWRlci5jb20vQWRkVHJ1\n"
"c3RFeHRlcm5hbENBUm9vdC5jcmwwOgYIKwYBBQUHAQEELjAsMCoGCCsGAQUFBzAB\n"
"hh5odHRwOi8vb2NzcC50cnVzdC1wcm92aWRlci5jb20wDQYJKoZIhvcNAQEMBQAD\n"
"ggEBAB3H+i5AtlwFSw+8VTYBWOBTBT1k+6zZpTi4pyE7r5VbvkjI00PUIWxB7Qkt\n"
"nHMAcZyuIXN+/46NuY5YkI78jG12yAA6nyCmLX3MF/3NmJYyCRrJZfwE67SaCnjl\n"
"lztSjxLCdJcBns/hbWjYk7mcJPuWJ0gBnOqUP3CYQbNzUTcp6PYBerknuCRR2RFo\n"
"1KaFpzanpZa6gPim/a5thCCuNXZzQg+HCezF3OeTAyIal+6ailFhp5cmHunudVEI\n"
"kAWvL54TnJM/ev/m6+loeYyv4Lb67psSE/5FjNJ80zXrIRKT/mZ1JioVhCb3ZsnL\n"
"jbsJQdQYr7GzEPUQyp2aDrV1aug=\n"
"-----END CERTIFICATE-----\n";
*/

bool once = true;

void loop()
{
  if ( once )
  {
    Serial.println( "run once" );

    WiFiClientSecure *client = new WiFiClientSecure;
    if(client) 
    {
      // client->setCACert(root_ca);

      HTTPClient https;

      if (https.begin(*client, "https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?symbol=BTC,XMR,ETH&convert=EUR")) {  // HTTPS

        Serial.print("[HTTPS] GET...\n");

        https.addHeader( "Accept", "application/json", true, true );
        https.addHeader( "X-CMC_PRO_API_KEY", COINMARKETCAP_API_KEY, false, true );

        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else 
        {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }

    once = false;
  }

  ArduinoOTA.handle();
  delay(20);
}
