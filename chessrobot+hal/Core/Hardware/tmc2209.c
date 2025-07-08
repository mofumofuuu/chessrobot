#include "tim.h"
#include "gpio.h"
//5480 90°
//大臂电机，方向正向上转
#define EN3 GPIOA,GPIO_PIN_9
#define step3 GPIOA,GPIO_PIN_8//tim1ch1
#define dir3 GPIOC,GPIO_PIN_9
//旋转电机，方向正向左转
#define EN2 GPIOC,GPIO_PIN_7
#define step2 GPIOC,GPIO_PIN_6//tim8ch1
#define dir2 GPIOD,GPIO_PIN_15
//小臂电机，方向正向上转
#define EN1 GPIOD,GPIO_PIN_13
#define step1 GPIOD,GPIO_PIN_12//tim4ch1
#define dir1 GPIOD,GPIO_PIN_11

uint8_t motor1_flag=0; //电机1 0就绪 1运行
uint8_t motor2_flag=0; //电机2 0就绪 1运行
uint8_t motor3_flag=0; //电机3 0就绪 1运行

//用于定步
int motor1_step=0;
int motor2_step=0;
int motor3_step=0;

//用于计数是否加速与当前挡位
uint8_t motor1_accenum=0,motor1_hz=0;
uint8_t motor2_accenum=0,motor2_hz=0;
uint8_t motor3_accenum=0,motor3_hz=0;

//记到此数加速
#define accel_num 10
//psc为168主频为10e6
//从100hz到4800hz慢慢加速
uint16_t arr_set[48]={10000,5000,3333,2500,2000,1667,1429,1250,1111,1000,909,833,769,714,667,625,588,556,526,500,476,455,435,417,400,385,370,357,345,333,323,313,303,294,286,278,270,263,256,250,244,238,233,227,222,217,213,208};
void tmc2209_init(void) { 
    //开启电机使能引脚
    HAL_GPIO_WritePin(EN3,GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN1,GPIO_PIN_SET);

//     __HAL_TIM_SET_AUTORELOAD(&htim8,625-1);
//     __HAL_TIM_SET_AUTORELOAD(&htim1,625-1);
//     __HAL_TIM_SET_AUTORELOAD(&htim4,625-1);

//     __HAL_TIM_SET_COMPARE (&htim4, TIM_CHANNEL_1, 313-1);
//     __HAL_TIM_SET_COMPARE (&htim1, TIM_CHANNEL_1, 313-1);
//     __HAL_TIM_SET_COMPARE (&htim8, TIM_CHANNEL_1, 313-1);
}

void motor3_setangle(float angle) { 
    if(angle>0) {
        HAL_GPIO_WritePin(dir3,GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(dir3,GPIO_PIN_RESET);
        angle=-angle;
    }
    motor3_step=angle*60.8888f;
    //开启定时中断
    HAL_TIM_Base_Start_IT(&htim1);
    //使能pwm输出
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
}
void motor2_setangle(float angle) { 
    if(angle>0) {
        HAL_GPIO_WritePin(dir2,GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(dir2,GPIO_PIN_RESET);
        angle=-angle;
    }
    motor2_step=angle*60.8888f;
    //开启定时中断
    HAL_TIM_Base_Start_IT(&htim8);
    //使能pwm输出
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
}
void motor1_setangle(float angle) { 
    if(angle>0) {
        HAL_GPIO_WritePin(dir1,GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(dir1,GPIO_PIN_RESET);
        angle=-angle;
    }
    motor1_step=angle*60.8888f;
    //开启定时中断
    HAL_TIM_Base_Start_IT(&htim4);
    //使能pwm输出
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
}




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    //大臂电机电机3
    if(htim->Instance == TIM1){//如果是tim1触发中断
        motor3_flag=1;
        if(motor3_hz<=31){//48个加速度挡位
            if(motor3_accenum==0){//设置arr与ccr
                __HAL_TIM_SET_AUTORELOAD(&htim1,arr_set[motor3_hz]-1);                  //设置arr
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,arr_set[motor3_hz]/2-1);    //设置ccr
            }
            motor3_accenum++;//记录过几个脉冲
            if(motor3_accenum==accel_num){//但记录到该加速时
                motor3_accenum=0;//清零计数
                motor3_hz++;//加速
            }
        }
        motor3_step--;
        if(motor3_step<=0){
            motor3_flag=0;
            //关闭定时中断
            HAL_TIM_Base_Stop_IT(&htim1);
            //关闭pwm输出
            HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
            motor3_accenum=0;//清零计数
            motor3_hz=0;//加速挡位归零等待下一次调用
        }
    }

    //旋转电机电机2
    if(htim->Instance == TIM8){//如果是tim8触发中断
        motor2_flag=1;
        if(motor2_hz<=31){//48个加速度挡位
            if(motor2_accenum==0){//设置arr与ccr
                __HAL_TIM_SET_AUTORELOAD(&htim8,arr_set[motor2_hz]-1);                  //设置arr
                __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1,arr_set[motor2_hz]/2-1);    //设置ccr
            }
            motor2_accenum++;//记录过几个脉冲
            if(motor2_accenum==accel_num){//但记录到该加速时
                motor2_accenum=0;//清零计数
                motor2_hz++;//加速
            }
        }
        motor2_step--;
        if(motor2_step<=0){
            motor2_flag=0;
            //关闭定时中断
            HAL_TIM_Base_Stop_IT(&htim8);
            //关闭pwm输出
            HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_1);
            motor2_accenum=0;//清零计数
            motor2_hz=0;//加速挡位归零等待下一次调用
        }
    }
    if(htim->Instance == TIM4){//如果是tim4触发中断
        motor1_flag=1;
        if(motor1_hz<=31){//48个加速度挡位
            if(motor1_accenum==0){//设置arr与ccr
                __HAL_TIM_SET_AUTORELOAD(&htim4,arr_set[motor1_hz]-1);                  //设置arr
                __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,arr_set[motor1_hz]/2-1);    //设置ccr
            }
            motor1_accenum++;//记录过几个脉冲
            if(motor1_accenum==accel_num){//但记录到该加速时
                motor1_accenum=0;//清零计数
                motor1_hz++;//加速
            }
        }
        motor1_step--;
        if(motor1_step<=0){
            motor1_flag=0;
            //关闭定时中断
            HAL_TIM_Base_Stop_IT(&htim4);
            //关闭pwm输出
            HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
            motor1_accenum=0;//清零计数
            motor1_hz=0;//加速挡位归零等待下一次调用
        }
    }
}

/***************************************测试函数**************************************************/
void tmc2209_test(void) { 
 //电机一启用方向正
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
  //电机二启用方向正
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
  //电机三启用方向正
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);

  __HAL_TIM_SET_COMPARE (&htim4, TIM_CHANNEL_1, 5000-1);
  __HAL_TIM_SET_COMPARE (&htim1, TIM_CHANNEL_1, 5000-1);
  __HAL_TIM_SET_COMPARE (&htim8, TIM_CHANNEL_1, 5000-1);

//   HAL_TIM_Base_Start_IT(&htim1);
//   HAL_TIM_Base_Start_IT(&htim4);
//   HAL_TIM_Base_Start_IT(&htim8);

  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
}

void motor2_test(){
    HAL_GPIO_WritePin(dir2,GPIO_PIN_SET);
    //开启定时中断
    HAL_TIM_Base_Start_IT(&htim8);
    //使能pwm输出
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
}
