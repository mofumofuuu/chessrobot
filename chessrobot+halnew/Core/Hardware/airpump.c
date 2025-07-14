#include "gpio.h"

//上电关闭继电器
//电磁阀上电打开
//气泵上电吸气
//做反向处理
//继电器2pd8为电磁阀 继电器1 pd10为气泵 

//电磁阀通电时导通，断电时关闭
void chess_init(void)
{
    //关闭气泵
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
}
void chess_take(void)
{
    //打开气泵
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
}

void chess_release(void)
{
    //关闭气泵
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
}
