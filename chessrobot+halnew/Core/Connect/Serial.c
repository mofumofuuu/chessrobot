#include "usart.h"

uint8_t dataRcvd;
uint8_t dataSend;

//0表示放,1表示取
uint8_t putflag;
//0黑1白
uint8_t color;
uint8_t chess_x,chess_y;

//主函数状态
extern uint8_t status;

/**
 * @brief 初始化串口
 */
void Serial_Init(void){
    //开启接受中断
	HAL_UART_Receive_IT(&huart2,&dataRcvd,1);
}

/**
 * @brief 发送一个字节
 * @param data 要发送的字节
 */
void Serial_Send(uint8_t data){
    HAL_UART_Transmit_IT(&huart2,&data,1);
}

void Serial_Send_Buffer(uint8_t *buffer,uint16_t size){ 
    HAL_UART_Transmit_IT(&huart2,buffer,size);
}

void Serial_Send_Str(char *str){
    HAL_UART_Transmit_IT(&huart2,(uint8_t *)str,strlen(str));
}

void Serial_Send_Hexbuffer(uint8_t data){
    Serial_Send(0xff);

    Serial_Send(data);

    Serial_Send(0xfe);
}

/**
 * @brief 接收到数据中断回调函数
 * @param huart 串口句柄
 */
uint8_t datanum=0;//接收到第几位数据
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart==&huart2){
        switch(datanum){
            case 0://包头
                if(dataRcvd==0xff) datanum=1;
                break;
            case 1://拿取
                putflag=dataRcvd;
                datanum++;
                break;
            case 2://颜色
                color=dataRcvd;
                datanum++;
                break;
            case 3: 
                chess_x=dataRcvd;
                datanum++;
                break;
            case 4: 
                chess_y=dataRcvd;
                datanum++;
                break;
            case 5://包尾
                if(dataRcvd==0xfe) datanum=0;
                //接收完所有数据主函数开始执行
                status=1;
        }
        
		HAL_UART_Receive_IT(&huart2,&dataRcvd,1);
	}
}

