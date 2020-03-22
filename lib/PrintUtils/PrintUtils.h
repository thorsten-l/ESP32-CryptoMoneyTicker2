#ifndef __PRINT_UTILS_H__
#define __PRINT_UTILS_H__

#include <Arduino.h>

void printName(String name, String symbol);
void printPrice(String price, bool show_eur);
void printChange(String percent_change_1h);
void printTime(String last_updated);
void printPagination();
void printError(String error);
void printTransition();
void printDigits(int digits);

#endif