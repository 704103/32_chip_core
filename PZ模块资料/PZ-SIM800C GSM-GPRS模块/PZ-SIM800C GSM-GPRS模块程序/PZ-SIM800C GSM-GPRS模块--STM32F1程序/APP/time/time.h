#ifndef _time_H
#define _time_H

#include "system.h"

void TIM4_Init(u16 per,u16 psc);

void TIM7_SetARR(u16 period);
void TIM7_Int_Init(u16 arr,u16 psc);

#endif
