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

#include <CryptoData.hpp>
#include <PrintUtils.h>
#include <Button.hpp>

byte mac[6];
struct tm timeinfo;
struct _crypto_info info;

int coinIndex = 0;
#define NUMBER_OF_COINS 5
String coinSymbols[] = { "BTC", "ETH", "LTC", "XMR", "XRP" };
const unsigned char* coinIcons[] = { bitcoin, ethereum, litecoin, monero, ripple };
int coinColors[] = { ILI9341_YELLOW, ILI9341_WHITE, ILI9341_LIGHTGREY, ILI9341_ORANGE, ILI9341_NAVY };
double oldPrice[NUMBER_OF_COINS];

Adafruit_ILI9341 tft =
    Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);


static int currencyIndex = 0;
#define NUMBER_OF_CURRENCIES 4
static String convertToCurrency[] = { "EUR", "USD", "CHF", "GBP" };
static time_t progressTimestamp;
static bool doUpdateCryptoData = true;

static Button buttonL(BUTTON_L);
static Button buttonM(BUTTON_M);
static Button buttonR(BUTTON_R);

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
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(1, 10);
  tft.setTextSize(2);
  tft.printf( "Hostname:\n  %s\n",  WiFi.getHostname());
  tft.printf( "\nIP-Address:\n  %s\n",  WiFi.localIP().toString().c_str());
  tft.printf( "\nGateway:\n  %s\n",  WiFi.gatewayIP().toString().c_str());
  tft.printf( "\nMask:\n  %s\n",  WiFi.subnetMask().toString().c_str());
  tft.printf( "\nDNS:\n  %s\n",  WiFi.dnsIP().toString().c_str());
}

void storeOldPrice()
{
  struct _crypto_info info;
  for( int i=0; i<NUMBER_OF_COINS; i++ )
  {
    cryptoData.get( &info, coinSymbols[i], convertToCurrency[currencyIndex] );
    Serial.printf( "storing old price for %s = %0.3f\n", coinSymbols[i].c_str(), info.price );
    oldPrice[i] = info.price;
  }
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
  progressTimestamp = 0;
  cryptoData.update(convertToCurrency[currencyIndex]);
  storeOldPrice();
}

time_t lastButtonPressedTimestamp = 0;

void loop()
{
  time_t currentMillis = millis();
  getLocalTime( &timeinfo );

  if ( currentMillis - lastButtonPressedTimestamp >= 333 )
  {
    if ( buttonL.isPressed() )
    {
      Serial.println( "left button is pressed" );
      lastButtonPressedTimestamp = currentMillis;
      coinIndex = ( coinIndex + (NUMBER_OF_COINS-2)) % NUMBER_OF_COINS;
      progressTimestamp = currentMillis - 15000;
    }
  
    if ( buttonM.isPressed() )
    {
      progressTimestamp = currentMillis - 10000;
      Serial.println( "middle button is pressed" );
      printTransition();
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(3);
      tft.setCursor(0,0);
      tft.println( "\n  Changing");
      tft.println( "  Currency");
      tft.print( "  to ");
      currencyIndex = ( currencyIndex + 1 ) % NUMBER_OF_CURRENCIES;
      tft.println( convertToCurrency[currencyIndex]);
      cryptoData.update(convertToCurrency[currencyIndex]);
      storeOldPrice();
      lastButtonPressedTimestamp = currentMillis;
      coinIndex = ( coinIndex - 1 ) % NUMBER_OF_COINS;
    }
  
    if ( buttonR.isPressed() )
    {
      Serial.println( "right button is pressed" );
      lastButtonPressedTimestamp = currentMillis;
      progressTimestamp = currentMillis - 15000;
    }
  }

  if ( doUpdateCryptoData && ( timeinfo.tm_min % 5 ) == 0 )
  {
    Serial.println(&timeinfo, PSTR("Time set            : %A %d %B %Y %H:%M:%S"));
    storeOldPrice();
    cryptoData.update(convertToCurrency[currencyIndex]);
    doUpdateCryptoData = false;
  }

  if ( doUpdateCryptoData == false && ( timeinfo.tm_min % 5 ) > 0 )
  {
    doUpdateCryptoData = true;
  }

  if(( currentMillis - progressTimestamp ) >= 15000 )
  {
    Serial.printf( "Showing %s (free heap: %d)\n", coinSymbols[coinIndex].c_str(), ESP.getFreeHeap());
    cryptoData.get( &info, coinSymbols[coinIndex], convertToCurrency[currencyIndex] );

    printTransition();
    tft.drawBitmap(5, 5, coinIcons[coinIndex], 45, 45, coinColors[coinIndex] );
    printName(info.name, info.symbol );
    printPrice( info.price, convertToCurrency[currencyIndex] );
    printChange(info.percent_change_1h);
    printTime(info.last_updated);
    printPagination();

    /*printError(error);*/
    
    tft.fillCircle(98 + (coinIndex*10), 300, 4, ILI9341_WHITE);

    coinIndex = ( coinIndex + 1 ) % NUMBER_OF_COINS;
    progressTimestamp = currentMillis;
  }

  ArduinoOTA.handle();
  delay(20);
}
