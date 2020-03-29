#include <App.hpp>
#include <HTTPClient.h>
#include <CryptoData.hpp>
#include <WiFi.h>

CryptoData cryptoData;
DynamicJsonDocument coinDataDocument(4096);

DeserializationError CryptoData::readJSON( int retries )
{
  HTTPClient http;
  DeserializationError err = DeserializationError::IncompleteInput;

  WiFiClientSecure *client = new WiFiClientSecure;

  if(client) 
  {
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
  
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            err = deserializeJson(coinDataDocument, https.getString());
            Serial.printf("[JSON] %s\n", err.c_str());

          if ( err == DeserializationError::Ok )
          {
            serializeJsonPretty( coinDataDocument, Serial );
            Serial.println();
          }

          Serial.print("[HTTPS] connection closed or file end.\n");
        }
      } 
      else 
      {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
  
      https.end();
    } 
    else 
    {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  if (err != DeserializationError::Ok && retries >= 1)
  {
    Serial.printf("ERROR: Parsing weather data. (%s, retries=%d)\n", err.c_str(), retries--);
    delay(5000);
    err = readJSON( retries );
  }

  return err;
}


DeserializationError CryptoData::getCryptoData()
{
  Serial.println("CryptoData::getCryptoData");
  DeserializationError err = readJSON(
      NUMBER_OF_RETRIES );
  return err;
}


bool CryptoData::update()
{
  bool updateError = false;

  DeserializationError err = getCryptoData();

  if (err != DeserializationError::Ok)
  {
    Serial.print("ERROR: Reading current weather data: ");
    Serial.println(err.c_str());
    updateError = true;
  }

  return updateError;
}

void CryptoData::get( struct _crypto_info *info, String symbol )
{
  JsonObject obj = coinDataDocument.as<JsonObject>();
  info->id = obj["data"][symbol]["id"].as<int>();
  info->price = obj["data"][symbol]["quote"]["EUR"]["price"].as<double>();
  info->percent_change_1h = obj["data"][symbol]["quote"]["EUR"]["percent_change_1h"].as<double>();
  strcpy( info->name, obj["data"][symbol]["name"].as<String>().c_str());
  strcpy( info->symbol, symbol.c_str());
  strcpy( info->last_updated, obj["data"][symbol]["quote"]["EUR"]["last_updated"].as<String>().c_str());
}

