#include <App.hpp>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <BoardConfig.hpp>
#include "OtaHandler.hpp"

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 tft;

void InitializeOTA()
{
  MDNS.begin( OTA_HOSTNAME );
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA
  .onStart([]()
  {
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.printf("\n\n[%d] OTA - Start updating %s\n", xPortGetCoreID(), 
      (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem" );

    tft.fillScreen( ILI9341_BLUE );
    digitalWrite(TFT_BL, true);
    tft.setTextColor( ILI9341_YELLOW );
    tft.setTextSize(2);
    tft.setCursor( 10, 10 );
    tft.println( "OTA start updating" );

    tft.setCursor( 10, 138 );
    tft.println( "Progress" );

    tft.drawFastHLine( 0, 30, 240, ILI9341_WHITE);
  } )

  .onEnd([]()
  {
    tft.setTextColor( ILI9341_ORANGE );
    tft.setCursor( 10, 280 );
    tft.print("*** RESTART ***");
    Serial.printf("\n[%d] OTA - Upload finished.\n", xPortGetCoreID() );
    Serial.println("\n\n*** restarting system ***\n\n");
  } )

  .onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("[%d] OTA - Progress: %u%%\r", xPortGetCoreID(), (progress / (total / 100)));
    tft.fillRect( 0, 158, (progress / (total/240)), 4, ILI9341_GREENYELLOW );
  } )

  .onError([](ota_error_t error)
  {
    tft.setTextColor( ILI9341_RED );
    tft.setCursor( 10, 250 );
    tft.print("ERROR!");
    tft.setCursor( 10, 280 );

    Serial.printf("Error[%u]: ", error);

    if (error == OTA_AUTH_ERROR)
    {
      tft.print( "Auth Failed" );
      Serial.println("Auth Failed");
    }
    else
    if (error == OTA_BEGIN_ERROR)
    {
      tft.print( "Begin Failed" );
      Serial.println("Begin Failed");
    }
    else
    if (error == OTA_CONNECT_ERROR)
    {
      tft.print( "Connect Failed" );
      Serial.println("Connect Failed");
    }
    else
    if (error == OTA_RECEIVE_ERROR)
    {
      tft.print( "Receive Failed" );
      Serial.println("Receive Failed");
    }
    else
    if (error == OTA_END_ERROR)
    {
      tft.print( "End Failed" );
      Serial.println("End Failed");
    }
  } );

  ArduinoOTA.begin();
  MDNS.addServiceTxt("_arduino", "_tcp", "fw_name", APP_NAME );
  MDNS.addServiceTxt("_arduino", "_tcp", "fw_version", APP_VERSION );

  Serial.println("Update Over-The-Air : initialized");
}
