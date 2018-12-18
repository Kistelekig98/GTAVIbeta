#include "drawing.h"
#include "display.h"

void DRV_DrawRectangle(Pixel p, uint16_t height, uint16_t width, Color color){

	for(int i = p.x ; i < p.x + width;i++ ) {

		 DRV_Display_DrawPixel(i, p.y, 0xFFFF);
	}

	for (int i= p.y ;i <= p.y + height; i++ ){
		DRV_Display_DrawPixel(p.x + width, i, 0xFFFF);
	}
#include "drawing.h"
#include "display.h"
    
    void DRV_DrawRectangle(Pixel p, uint16_t height, uint16_t width, Color color){
        
        for(int i = p.x ; i < p.x + width;i++ ) {
            
            DRV_Display_DrawPixel(i, p.y, 0xFFFF);
        }
        
        for (int i= p.y ;i <= p.y + height; i++ ){
            DRV_Display_DrawPixel(p.x + width, i, 0xFFFF);
        }
        
        for(int i= p.x + width; i >= p.x; i-- ){
            DRV_Display_DrawPixel(i, p.y + height, 0xFFFF);
        }
        
        for(int i=p.y + height ;i >=p.y; i--){
            DRV_Display_DrawPixel(p.x, i, 0xFFFF);
        }
        
    }
    
    void DRV_DrawCar(Pixel p, uint16_t height, uint16_t width, uint32_t color) {
        Pixel p2;
        p2.x = (p.x - (width / 2));
        p2.y = (p.y - (height / 2));
        uint16_t x;
        uint16_t y;
        
        for( int i = 0; i <= height; i++) {
            for(int j = 0; j <= width; j++) {
                //csak azokat a pontokat írjuk ki ami a 320x240 tartományba esnek
                //véletlen hibák ellen véd + beúszás csak ezzel lehetséges
                if ((p2.x + j <= 240) && (p2.x + j >= 0) && (p2.y + i <=320) && (p2.y + i >= 0)) {
                    DRV_Display_DrawPixel(p2.x + j, p2.y + i, color);
                }
            }
        }
    }

	for(int i= p.x + width; i >= p.x; i-- ){
		DRV_Display_DrawPixel(i, p.y + height, 0xFFFF);
	}

	for(int i=p.y + height ;i >=p.y; i--){
		DRV_Display_DrawPixel(p.x, i, 0xFFFF);
	}

}

void DRV_DrawCar(Pixel p, uint16_t height, uint16_t width, uint32_t color) {
	Pixel p2;
	p2.x = (p.x - (width / 2));
	p2.y = (p.y - (height / 2));
	uint16_t x;
	uint16_t y;

	for( int i = 0; i <= height; i++) {
		for(int j = 0; j <= width; j++) {
			//csak azokat a pontokat írjuk ki ami a 320x240 tartományba esnek
			//véletlen hibák ellen véd + beúszás csak ezzel lehetséges
			if ((p2.x + j <= 240) && (p2.x + j >= 0) && (p2.y + i <=320) && (p2.y + i >= 0)) {
				DRV_Display_DrawPixel(p2.x + j, p2.y + i, color);
			}
		}
	}
}
