#ifndef __PRINT_UTILS_H__
#define __PRINT_UTILS_H__

#include <Arduino.h>

void printName(String name, String symbol);
void printPrice(double price, bool show_eur);
void printChange(double percent_change_1h);
void printTime(char *last_updated);
void printPagination();
void printError(String error);
void printTransition();

#endif