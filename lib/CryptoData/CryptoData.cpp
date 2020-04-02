#include <App.hpp>
#include <HTTPClient.h>
#include <CryptoData.hpp>
#include <WiFi.h>

CryptoData cryptoData;

static StaticJsonDocument<4096> coinDataDocument;
static WiFiClientSecure clientSecure;
static HTTPClient https;
static DeserializationError error;

DeserializationError CryptoData::readJSON( int retries, String currency )
{
  error = DeserializationError::IncompleteInput;

    if (https.begin(clientSecure, "https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?symbol=BTC,XMR,ETH,LTC,XRP&convert=" + currency )) {

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
            error = deserializeJson(coinDataDocument, https.getString());
            Serial.printf("[JSON] %s\n", error.c_str());

          if ( error == DeserializationError::Ok )
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

  if (error != DeserializationError::Ok && retries >= 1)
  {
    Serial.printf("ERROR: Parsing weather data. (%s, retries=%d)\n", error.c_str(), retries--);
    delay(5000);
    error = readJSON( retries, currency );
  }

  return error;
}


DeserializationError CryptoData::getCryptoData( String currency )
{
  DeserializationError err = readJSON(
      NUMBER_OF_RETRIES, currency );
  return err;
}


bool CryptoData::update(String currency)
{
  bool updateError = false;

  DeserializationError err = getCryptoData(currency);

  if (err != DeserializationError::Ok)
  {
    Serial.print("ERROR: Reading current crypto data: ");
    Serial.println(err.c_str());
    updateError = true;
  }
  return updateError;
}

void CryptoData::get( struct _crypto_info *info, String symbol, String currency )
{
  JsonObject obj = coinDataDocument.as<JsonObject>();
  info->id = obj["data"][symbol]["id"].as<int>();
  info->price = obj["data"][symbol]["quote"][currency]["price"].as<double>();
  info->percent_change_1h = obj["data"][symbol]["quote"][currency]["percent_change_1h"].as<double>();
  strcpy( info->name, obj["data"][symbol]["name"].as<String>().c_str());
  strcpy( info->symbol, symbol.c_str());
  strcpy( info->last_updated, obj["data"][symbol]["quote"][currency]["last_updated"].as<String>().c_str());
}

