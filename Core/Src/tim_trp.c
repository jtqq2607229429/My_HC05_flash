//
// Created by JIANG on 2021/8/2.
//

#include "tim_trp.h"
#include "tim.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == htim14.Instance) {

    }
    HAL_TIM_Base_Start_IT(htim);
}
