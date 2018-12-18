#include "CarFunctions.h"

double CarDistanceX(Car* c1, Car* c2) {
	return abs(c1->CarPos.x - c2->CarPos.x);
}
double CarDistanceY(Car* c1, Car* c2) {
	return abs(c1->CarPos.y - c2->CarPos.y);
}

void ShiftCarArray(Car* CarArray) {
	uint8_t n = 0;										// A tömb elemszáma = aktuálisan ennyi autó játszik

	for(int i = 0; CarArray[i].CarPos.x != 0; i++) {	// Leszámlálás
		n = i + 1;
	}

	for(int i = n; i != 0; i--) {						// Shiftelés jobbra
		CarArray[i] = CarArray[i-1];
	}
}

void AddCar(Car* CarArray) {
	CarArray[0].CarPos.x = 15 + rand() % 210;			// új autó mindig a tömb 0. eleme
	CarArray[0].CarPos.y = 350;
	CarArray[0].color = 0xFFFF0000;
}

uint8_t DeleteCar(Car* CarArray, uint8_t score) {
	for(int i = 0; CarArray[i].CarPos.x != 0; i++) {
		if(CarArray[i].CarPos.y <= -30) {
			CarArray[i].CarPos.x = 0;
			CarArray[i].CarPos.y = 0;
			CarArray[i].color = 0;

			score++;
		}
	}
	return score;
}

void DrawCars(Car* CarArray, uint8_t CarWidth, uint8_t CarHeight) {
	for(int i = 0; CarArray[i].CarPos.x != 0; i++) {
		DRV_DrawCar(CarArray[i].CarPos, CarHeight, CarWidth, CarArray[i].color);
	}
}

void MoveCars(Car* CarArray, uint8_t speed) {
	for(int i = 0; CarArray[i].CarPos.x != 0; i++) {
		CarArray[i].CarPos.y = CarArray[i].CarPos.y - speed;
	}
}

bool CheckCollision(Car* CarArray, Car* MyCar, uint8_t CarWidth, uint8_t CarHeight) {
	for(int i = 0; CarArray[i].CarPos.x != 0; i++) {
		if (CarDistanceX(MyCar, &CarArray[i]) < CarWidth && CarDistanceY(MyCar, &CarArray[i]) < CarHeight) {
			return true;
		}
	}
	return false;
}

void DrawScore(char* score, Pixel p) {
	DRV_Display_WriteStringAt(p, score, ALIGN_Right);
}
