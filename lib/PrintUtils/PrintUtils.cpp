#include <PrintUtils.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 tft;
extern int coinIndex;
extern double oldPrice[5];

char timeBuffer[32];

void printName(String name, String symbol) 
{
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(65, 10);
  tft.println(name);
  tft.setTextSize(2);
  tft.setCursor(65, 33);
  tft.print(symbol);
  tft.drawLine(65, 54, 240, 54, ILI9341_WHITE);
}

void printPrice( double price, String currency ) 
{
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(4, 72);
  tft.print("Price");

  if (price != oldPrice[coinIndex]) 
  {
    if (price > oldPrice[coinIndex]) 
    {
      tft.setTextColor(ILI9341_GREENYELLOW);
    } 
    else 
    {
      tft.setTextColor(ILI9341_RED);
    }
  }

  tft.setTextSize(3);
  tft.setCursor(16, 72+24);
  
  tft.printf("%0.3f", price );
  tft.setTextSize(2);
  tft.print(currency);

  tft.println();
}

void printChange(double percent_change_1h) 
{
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(4, 140);
  tft.print("Change(1h)");
  tft.setTextColor( (percent_change_1h >= 0.0 ) ? ILI9341_GREENYELLOW : ILI9341_RED );
  tft.setTextSize(3);
  tft.setCursor(16, 140+24);
  tft.printf( "%0.3f%%", percent_change_1h );
}

void printTime(char *last_updated) 
{
  strcpy( timeBuffer, last_updated);
  timeBuffer[10] = ' ';
  timeBuffer[19] = 0;
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 208);
  tft.print("Last Updated (UTC)");

  tft.setTextSize(3);
  tft.setCursor(20, 208+24);
  tft.print(&timeBuffer[11]);
}

void printPagination() {
  // tft.drawCircle(98, 300, 4, ILI9341_WHITE);
  tft.drawCircle(108, 300, 4, ILI9341_WHITE);
  tft.drawCircle(118, 300, 4, ILI9341_WHITE);
  tft.drawCircle(128, 300, 4, ILI9341_WHITE);
  // tft.drawCircle(138, 300, 4, ILI9341_WHITE);
}

void printError(String error) {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(65, 22);
  tft.println(error);
}

void printTransition() {
  tft.fillScreen(ILI9341_BLACK);
  yield();
}
