#include <PrintUtils.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TimeLib.h>

extern Adafruit_ILI9341 tft;
extern int coin;
extern int ILI9341_COLOR;
extern String oldPrice[5];

void printName(String name, String symbol) {

  tft.setTextSize(3);
  tft.setCursor(65, 10);
  tft.println(name);

  tft.setTextSize(2);
  tft.setCursor(65, 33);
  tft.print(symbol);

  tft.drawLine(65, 54, 240, 54, ILI9341_WHITE);
}

void printPrice(String price, bool show_eur) {

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 75);
  tft.print("Price:");

  if (price != oldPrice[coin]) {
    if (price > oldPrice[coin]) {

      ILI9341_COLOR = ILI9341_GREENYELLOW;
    } else {

      ILI9341_COLOR = ILI9341_RED;
    }
  }

  tft.setTextSize(3);
  tft.setTextColor(ILI9341_COLOR);
  tft.setCursor(40, 110);
  if (show_eur == false) {
    tft.print("$");
  }
  tft.print(price);

  if (show_eur == true) {
    tft.setTextSize(2);
    tft.print("Euro");
  }

  tft.println();
}

void printChange(String percent_change_1h) {

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 150);
  tft.print("Change(1h):");

  if (percent_change_1h >= "0") {

    tft.setTextColor(ILI9341_GREENYELLOW);

  } else {

    tft.setTextColor(ILI9341_RED);
  }

  tft.setTextSize(3);
  tft.setCursor(40, 180);
  tft.print(percent_change_1h);
  tft.print("%");
}

void printTime(String last_updated) {

  long int timeData = last_updated.toInt();
  time_t t = timeData;
  // time_t Z = 1515675789;

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 220);
  tft.print("Last Updated:");

  tft.setTextSize(3);
  tft.setCursor(40, 250);
  /*
  printDigits(day(t));
  tft.print("/");
  printDigits(month(t));
  tft.print("/");
  tft.print(year(t));
  tft.print(" ");
  */
  printDigits(hour(t) + 1); // +1 for the French time
  tft.print(":");
  printDigits(minute(t));
  // tft.print(":");
  // printDigits(second(t));
}

void printPagination() {

  tft.drawCircle(98, 300, 4, ILI9341_WHITE);
  tft.drawCircle(108, 300, 4, ILI9341_WHITE);
  tft.drawCircle(118, 300, 4, ILI9341_WHITE);
  tft.drawCircle(128, 300, 4, ILI9341_WHITE);
  tft.drawCircle(138, 300, 4, ILI9341_WHITE);
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

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and
  // leading 0
  if (digits < 10)
    tft.print('0');
  tft.print(digits);
}
