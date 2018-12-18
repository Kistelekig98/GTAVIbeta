#ifndef CARFUNCTIONS_H_
#define CARFUNCTIONS_H_
#endif /* CARFUNCTIONS_H_ */
#include "drawing.h"
#include "display.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct Car {							// Egy általános autó leírása
		Pixel CarPos;							// Pozíció (x, y)
		uint32_t color;							// Szín (0xFFRRGGBB)
	} Car;

double CarDistanceX(Car* c1, Car* c2);			// Két autó horizontális távolsága
double CarDistanceY(Car* c1, Car* c2);			// Két autó vertikális távolsága

void ShiftCarArray(Car* CarArray);				// A tömbben lévő autók jobbra shiftelése

void AddCar(Car* CarArray);						// Új autó beszúrása

uint8_t DeleteCar(Car* CarArray, uint8_t score); // A képernyőből kiment autók törlése + pontszám növelése

void DrawCars(Car* CarArray, uint8_t CarWidth, uint8_t CarHeight);		// Kirajzolja a tömbben lévő autókat

void MoveCars(Car* CarArray, uint8_t speed);	// Adott számú pixellel elmozdítja aaz autókat lefelé

bool CheckCollision(Car* CarArray, Car* MyCar, uint8_t CarWidth, uint8_t CarHeight);	// Ütközések vizsgálata

void DrawScore(char* score, Pixel p);			// Pontszám kiírása
