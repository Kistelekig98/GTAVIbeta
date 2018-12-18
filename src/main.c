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

int main(void)
{
	gyro_t gyroVal;

	SystemClock_Config();
	HAL_Init();
	DRV_InitLed(LED3);
	DRV_InitLed(LED4);
	DRV_InitDrawTimer();
	DRV_Display_Init();
	DRV_Display_Clear();

//	UartFifoInit();

	InitWindowManager();		//kell

	DRV_VCP_Init();
	DRV_TS_Init();
	DRV_Gyro_Init();
	DRV_Gyro_Reset();
	DRV_Gyro_Calibration();

	DRV_VCP_WriteString("TemaLabor Hello.\r\n");

	SetActiveWindow(GetMenuWindow());		//kell

	uint8_t fifobyte;
	pMessage_t pMessage;
	uint8_t generateSignal=1;

//		Tabula Rasa
//---------------------------------------------------------------------
	BSP_LCD_SelectLayer(1);
	BSP_LCD_Clear();
	BSP_LCD_SetLayerVisible(1, DISABLE);
	BSP_LCD_SetLayerVisible(0, ENABLE);

	BSP_LCD_SelectLayer(0);
	DRV_Display_Clear();
//---------------------------------------------------------------------
//##################################################################
//##################################################################
	bool start = false;

		Pixel p;				// Főcím középpontja
		p.x = 120;
		p.y = 250;

		Pixel p2;				//Start körvonala bal alsó p ja
		p2.x = 90;
		p2.y = 82;

		uint16_t sWidth = 60;	// Start körvonal magassága, szélessége
		uint16_t sHeight = 40;

		Pixel p3;				//Start felirat közép pontja
		p3.x = 120;
		p3.y = 103;

		Pixel MyCarPos;			// Saját autó pozíciója
		MyCarPos.x = 120;
		MyCarPos.y = 45;

		uint16_t CarWidth = 30;
		uint16_t CarHeight = 60;

		typedef struct Car {	// egy  autó
			Pixel CarPos;
			uint32_t color;
		} Car;
//******************************

		char* str = "Hello";
		char* str2 = "Start";

		DRV_Display_WriteStringAt(p, str, ALIGN_Center);
		DRV_DrawRectangle(p2, sHeight, sWidth, black);
		DRV_Display_WriteStringAt(p3, str2, ALIGN_Center);


	//**************************************


		uint16_t touchX;
		uint16_t touchY;

	while(!start) {
		if(DRV_TS_IsTouchDetected()){
			touchX = DRV_TS_GetX();
			touchY = DRV_TS_GetY();

			if( (touchX >= p2.x) && (touchX <= p2.x + sWidth) && (touchY >= p2.y) && (touchY <= p2.y + sHeight) ) {
				start = true;
			}
		}
	}

	srand(HAL_GetTick());

	Car Car1;
	Car1.CarPos.x = 100;
	Car1.CarPos.y = 350;
	Car1.color = 0xFF00FF00;

// ============================== LOOP =======================================================
	while(1) {

		if(DRV_TS_IsTouchDetected()){

			touchX = DRV_TS_GetX();
			touchY = DRV_TS_GetY();

			if(touchX > MyCarPos.x && MyCarPos.x < 225) {
				MyCarPos.x += 10;
			}
			if(touchX <= MyCarPos.x && MyCarPos.x > 15) {
				MyCarPos.x -= 10;
			}
		}

		DRV_Display_Clear();
		if(MyCarPos.x > 225){MyCarPos.x = 225;}
		if(MyCarPos.x < 15){MyCarPos.x = 15;}



		Car1.CarPos.y = Car1.CarPos.y -25;
		if(Car1.CarPos.y < -30) {
			Car1.CarPos.y = 350;

			Car1.CarPos.x = rand() %220;
		}

		DRV_DrawCar(MyCarPos, CarHeight, CarWidth, 0xFF9F87E4);
		DRV_DrawCar(Car1.CarPos, CarHeight, CarWidth, Car1.color);
		HAL_Delay(50);


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

	}
}
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
