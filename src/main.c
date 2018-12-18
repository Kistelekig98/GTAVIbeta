/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <stm32f4xx.h>

#include "display.h"
#include "vcp.h"
#include "Common.h"
#include "drawing.h"
#include "touch.h"
#include "gyroscope.h"
#include "CarFunctions.h"

#include "UartFifo.h"
#include "MessageHandler.h"
#include "CommHandler.h"

#include "WindowManager.h"
#include "MenuWindow.h"

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

static void SystemClock_Config(void);
char buff[20];
uint32_t frameTime;

int main(void) {
// ======================= INICIALIZÁLÁSOK ====================================================================
	gyro_t gyroVal;

	SystemClock_Config();
	HAL_Init();
	DRV_InitLed(LED3);
	DRV_InitLed(LED4);
	DRV_InitDrawTimer();
	DRV_Display_Init();
	DRV_Display_Clear();

//	UartFifoInit();

	InitWindowManager();	//kell

	DRV_VCP_Init();
	DRV_TS_Init();
	DRV_Gyro_Init();
	DRV_Gyro_Reset();
	DRV_Gyro_Calibration();

	DRV_VCP_WriteString("TemaLabor Hello.\r\n");

	SetActiveWindow(GetMenuWindow());	//kell

	uint8_t fifobyte;
	pMessage_t pMessage;
	uint8_t generateSignal=1;

// ======================= A LÉNYEG ======================================================================
	bool start = false;						// true-ra indul majd a játék
	bool alive = true;						// ha ütközünk, false-ra vált

	// ------------------- FŐCÍM -------------------------------------------------------------------------
		Pixel TitlePx;													// Pozíció
		TitlePx.x = 120;
		TitlePx.y = 250;

		char* TitleText = "GTA VI open beta";							// Szöveg

		DRV_Display_WriteStringAt(TitlePx, TitleText, ALIGN_Center);	// Kiíratás

	// ------------------- START GOMB --------------------------------------------------------------------
		Pixel StartButtonPx;				// Körvonal bal alsó pontja
		StartButtonPx.x = 90;
		StartButtonPx.y = 82;

		uint16_t StartButtonWidth = 60;		// Körvonal magassága, szélessége
		uint16_t StartButtonHeight = 40;

		DRV_DrawRectangle(StartButtonPx, StartButtonHeight, StartButtonWidth, black);	// Körvonal kirajzolása

		Pixel StartTextPx;					// Start felirat középpontja
		StartTextPx.x = 120;
		StartTextPx.y = 103;

		char* StartText = "Start";

		DRV_Display_WriteStringAt(StartTextPx, StartText, ALIGN_Center);				// Start felirat kiírása

	// ------------------ AUTÓK KONFIGURÁLÁSA -----------------------------------------------------------
		Car MyCar;							// Saját autó és paraméterei
		MyCar.CarPos.x = 120;
		MyCar.CarPos.y = 45;
		MyCar.color = 0xFF9F87E4;

		uint8_t CarWidth = 30;				// Egy általános autó méretei
		uint8_t CarHeight = 60;

		uint8_t CycleCounter = 0;			// A főciklus ciklusait számoljuk ezzel
		uint8_t CarDelay = 8;				// A főciklus minden ennyiedik futásakor küldünk be új autót
		uint8_t VerticalSpeed = 15;			// A szembe jövő autók sebessége

	// ------------------ AUTÓK TÁROLÁSA ----------------------------------------------------------------
		Car CarArray[20];

		for(int i = 0; i < 20; i++) {
			CarArray[i].CarPos.x = 0;		// Kezdetban egy "ellenfél" autó sincsen
			CarArray[i].CarPos.y = 0;		// Nem létező autó = minden adata 0
			CarArray[i].color = 0;
		}

	// ------------------ START GOMB MEGNYOMÁSÁNAK VIZSGÁLATA -------------------------------------------
	uint16_t touchX;						// Itt lett megérintve a képernyő
	uint16_t touchY;

	while(!start) {
		if(DRV_TS_IsTouchDetected()){
			touchX = DRV_TS_GetX();
			touchY = DRV_TS_GetY();
											// Érintés a gombon belűl -> Start
			if( (touchX >= StartButtonPx.x) && (touchX <= StartButtonPx.x + StartButtonWidth) &&
				(touchY >= StartButtonPx.y) && (touchY <= StartButtonPx.y + StartButtonHeight) ) {
				start = true;
			}
		}
	}

	// ------------------ EGYÉB BEÁLLÍTÁSOK ------------------------------------------------------------
	srand(HAL_GetTick());					// rand() seedelése

	uint8_t score = 0;						// Pontszám = kikerült autók száma
	char scoreString[10];					// Ugyanez karaktertömbben

	Pixel ActualScorePx;
	ActualScorePx.x = 230;
	ActualScorePx.y = 300;


// ====================== LOOP - MAGA A JÁTÉK ==========================================================
	while(alive) {

	// ------------------ SAJÁT AUTÓ IRÁNYÍTÁSA --------------------------------------------------------
		if(DRV_TS_IsTouchDetected()){

			touchX = DRV_TS_GetX();
			touchY = DRV_TS_GetY();

			if(touchX > MyCar.CarPos.x && MyCar.CarPos.x < 225) {		// érintés az autótól balra
				MyCar.CarPos.x += 10;
			}
			if(touchX <= MyCar.CarPos.x && MyCar.CarPos.x > 15) {		// érintés az autótól jobbra
				MyCar.CarPos.x -= 10;
			}
		}



		DRV_Display_Clear();

		if(MyCar.CarPos.x > 225) {										// esetleges túlcsúszások megelőzése
			MyCar.CarPos.x = 225;
		}
		if(MyCar.CarPos.x < 15) {
			MyCar.CarPos.x = 15;
		}

	// ------------------ JÁTÉKMENET --------------------------------------------------------------------
		if (CycleCounter == CarDelay || CycleCounter == 0) {			// Új autó beszúrása
			CycleCounter = 0;

			ShiftCarArray(&CarArray);									// Tömb shiftelése
			AddCar(&CarArray);											// Beszúrás a tömb elejére
		}

		sprintf(scoreString, "%d", score);								// int -> char*
		DrawScore(&scoreString, ActualScorePx);							// Pontszám kiírás

		DrawCars(&CarArray, CarWidth, CarHeight);						// Autók kirajzolása

		MoveCars(&CarArray, VerticalSpeed);								// Autók mozgatása

		if(CheckCollision(&CarArray, &MyCar, CarWidth, CarHeight)) {	// Ütközés vizsgálata
			alive = false;
		}

		DRV_DrawCar(MyCar.CarPos, CarHeight, CarWidth, 0xFF9F87E4);		// Saját autó kirajzolása

		DRV_Display_SwitchBuffer();

		score = DeleteCar(&CarArray, score);							// A képernyőből kiment autók törlése
																		// + pontszám frissítése
		HAL_Delay(30);
		CycleCounter++;
	}

// ====================== GAME OVER KEZELÉSE ===========================================================

	DRV_Display_Clear();

	// ------------------ GAME OVER FELIRAT ------------------------------------------------------------
	char* GameOverMsg = "Game Over";

	Pixel GameOverPx;
	GameOverPx.x = 120;
	GameOverPx.y = 250;

	DRV_Display_WriteStringAt(GameOverPx, GameOverMsg, ALIGN_Center);

	// ------------------ PONTSZÁMKIJELZÉS ------------------------------------------------------------
	Pixel FinalScoreTextPx;
	FinalScoreTextPx.x = 80;
	FinalScoreTextPx.y = 180;

	char* FinalScoreText = "Score:";

	DRV_Display_WriteStringAt(FinalScoreTextPx, FinalScoreText, ALIGN_Left);

	Pixel FinalScorePx;
	FinalScorePx.x = 160;
	FinalScorePx.y = 180;

	DrawScore(&scoreString, FinalScorePx);

	DRV_Display_SwitchBuffer();

// ===================== KONYEC FILMA =================================================================
/*
		if(DRV_TS_IsTouchDetected())

			HandleTouch(DRV_TS_GetX(), DRV_TS_GetY());

		//gyroVal = DRV_Gyro_GetXYZ();

		while(UartFifoPop(&fifobyte)){
			pMessage=CommByteReceived(fifobyte);
			if (pMessage!=NULL){
				HandleMessage(pMessage);
				InitMessage(pMessage);
				generateSignal=0;
			}
		}


		if (generateSignal){
			float dftTestSignal = 1000 * sin(2 * M_PI * 1 * HAL_GetTick()/1000.0f) + 1000 * sin(2 * M_PI * 8 * HAL_GetTick()/1000.0f);
			DataPacket d;
			d.Data = dftTestSignal; //gyroVal.omega_z;
			HandleSpectrum(dftTestSignal);
			gyroVal=DRV_Gyro_GetXYZ();
			HandleChart(gyroVal.omega_z);
			HandleCompass(gyroVal.omega_z);
			HandleHistogram(gyroVal.omega_z);
		}

		//DataPacket d;
		//d.Data = dftTestSignal; //gyroVal.omega_z;
		//HandleNewData(&d);
*/
		//HAL_Delay(100);
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//  Mi kiegészítésünk
/*
		BSP_LCD_SelectLayer(1);
		BSP_LCD_Clear();
		BSP_LCD_SetLayerVisible(1, DISABLE);
*/
/*
		BSP_LCD_SelectLayer(0);
		DRV_Display_Clear();
		*/
//		BSP_LCD_SetFont(&Font16);	//itt nemlehet
/*
		for(int i=0;i<150;i++){
			DRV_Display_DrawPixel(i, 10*sqrt(i), 0xFFFF);
		}
*/

	// ------------ [ Az efölött lévő dolgok eredetileg a while(1)-ben voltak ] -----------------------

}

// ================ MAIN() VÉGE =======================================================================
//---------------------------------------------------------
/*//periodikusan ez újra
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (HandleDraw())
		DRV_Display_SwitchBuffer();
}
*/

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Activate the Over-Drive mode */
  HAL_PWREx_EnableOverDrive();

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	DRV_ErrorLoop();

}
#endif
