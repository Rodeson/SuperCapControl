#ifndef _RGBLED_H
#define _RGBLED_H

#include "main.h"
#include "tim.h"

void RGB_LED_Init(void);
void RGB_B_Light(void);
void RGB_R_Light(void);
void RGB_G_Light(void);
void RGB_ALL_Unlight(void);
void RGB_LED_BeforeWork_Check(void);

#endif
