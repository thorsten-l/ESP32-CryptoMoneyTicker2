#ifndef __CRYPTO_DATA_HPP__
#define __CRYPTO_DATA_HPP__

#include <Arduino.h>
#include <ArduinoJson.h>

#define NUMBER_OF_RETRIES 3

struct _crypto_info
{
  unsigned long id;
  char name[64];
  char symbol[16];
  char slug[64];
  double price;
  double volume_24h;
  double percent_change_1h;
  double percent_change_24h;
  double percent_change_7d;
  double market_cap;
  char last_updated[64];
};

class CryptoData
{
private:
  DeserializationError readJSON( int retries, String currency);
  DeserializationError getCryptoData(String currency);
  void get( struct _crypto_info *info, JsonObject &obj, String currency );

public:
  bool update(String currency);
  void get( struct _crypto_info *info, String symbol, String currency);
};

extern CryptoData cryptoData;

#endif

