#include "arcsin.h"
#include "arccos.h"
#include "arctan2.h"
#include "tmc2209.h"
#include "H_Oled.h"
#include "airpump.h"

#include "math.h"

#define MY_PI 3.1415926f
#define MY_PI_2 MY_PI/2

#define long_lenth 135
#define short_lenth 170

extern uint8_t motor1_flag; //电机1 0就绪 1运行
extern uint8_t motor2_flag; //电机2 0就绪 1运行
extern uint8_t motor3_flag; //电机3 0就绪 1运行

//大小臂初始角度(上电前)
// float arm1_angle=-15.45f;相对于y轴
// float arm2_angle=-32.3f;
//上电开始复位后（相对于x轴）
float arm1_angle=90.0f;
//上电开始复位后（相对于y轴）
float arm2_angle=90.0f;
float arm12_angle=0.0f;

//求取大臂与Y正半轴角度
//(x - long_lenth\sin\theta)^2 + (y - long_lenth\cos\theta)^2 = short_lenth^2 \quad
float arm1_anglecal(double x, double y) { 
    float arctan2 = fast_atan2_std(y, x);
    float temp=(x*x+y*y-10675)/(270*sqrt(x*x+y*y));
    float arcsin = arcsin_standard(temp);
    float theta =arcsin-arctan2;
    //OLED_ShowFloat(3,1,theta*180.0f/MY_PI,4,6);
    return theta*180.0f/MY_PI;//弧度转换为角度返回
}
//计算小臂与大臂夹角
float arm2_anglecal(double x, double y) { 
    float temp=(47125-x*x-y*y)/45900;
    float arccos= arccos_standard(temp);
    return arccos*180.0f/MY_PI;//弧度转换为角度返回
}

void angle_setlz(double x, double y){
        //大臂相对于X的角度
        float angle_arm1=90.0f-arm1_anglecal(x,y);

        //180-(90-angle_arm1)-angle_arm2为小臂相对于x正半轴的夹角
        float angle_arm2=arm2_anglecal(x,y)-arm1_anglecal(x,y);

        //将小臂角度设置为angle_arm2
        //设置电机转动预计角度-当前角度
        motor1_setangle(angle_arm2-arm2_angle);
        //更新当前角度
        arm2_angle=angle_arm2;
        OLED_ShowFloat(2,1,arm2_angle,3,5);

        HAL_Delay(500);

        //将大臂角度设置为angle_arm1
        //设置电机转动预计角度-当前角度
        motor3_setangle(angle_arm1-arm1_angle);
        OLED_ShowFloat(4,1,angle_arm1-arm1_angle,3,5);
        //更新当前角度
        arm1_angle=angle_arm1;
        OLED_ShowFloat(1,1,arm1_angle,3,5);
}

void angele_setxyz(double x, double y,double z){
    x=x+64.0f;//加上大小臂电机到中心电机距离
    double lenth=sqrt(x*x+y*y);
    //电磁阀长度
    lenth=lenth-50.0f;
    angle_setlz(lenth-64.0f,z);
    double theta = atan2(y,x);
    //预计角减去当前角
    motor2_setangle(theta*180.0f/MY_PI-arm12_angle);
    OLED_ShowFloat(3,1,theta*180.0f/MY_PI,3,5);
    arm12_angle=theta*180.0f/MY_PI;
}

//0为吸棋 1为放棋
void angele_smothmove(double x, double y,double z,uint8_t flag){
    //移动到对应位置并缓慢下降
    if(z>=-20){
        angele_setxyz(x,y,z);
    }else{
        for(int i=-20;i>=z;i=i-5){
             if(i-z<=5){
                angele_setxyz(x,y,z);
                 while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
                     HAL_Delay(10);
                }
                break;
                }else{
                 angele_setxyz(x,y,i);
                 while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
                     HAL_Delay(10);
                }
             }
        }
    }

    //吸取棋子或放下棋子
    if(flag==0){
        chess_take();
    }else{
        chess_release();
    }

    //从对应位置缓慢升高
    //对应位置低于-20升高再移动，高于直接移动
    if(z<-20){
        for(int i=z;i<=-20;i=i+10){
            if(-20-i<=10){
                angele_setxyz(x,y,-20);
                while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
                    HAL_Delay(10);
                }
                break;
            }else{
                angele_setxyz(x,y,i);
                while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
                    HAL_Delay(10);
                }
            }
        }
    }
    
    angele_setxyz(208,0,80);
    while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
        HAL_Delay(10);
    }

}
// void angele_smothmove(double x, double y,double z,uint8_t flag){
//     if(z>=-20){//大于-20 
//         angele_setxyz(x,y,z);
//     }else{
//         for(int i=-20;i>=z;i=i-10){
//             if(i-z<=10){
//                 angele_setxyz(x,y,z);
//                 while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
//                     HAL_Delay(10);
//                 }
//                 break;
//             }else{
//                 angele_setxyz(x,y,i);
//                 while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
//                     HAL_Delay(10);
//                 }
//             }
//         }
// if(flag==0){
//     chess_take();
// }else{
//     chess_release();
// }
//     for(int i=z;i<=-20;i=i+10){
//         if(-20-i<=10){
//             angele_setxyz(x,y,-20);
//             while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
//                 HAL_Delay(10);
//             }
//             break;
//         }else{
//             angele_setxyz(x,y,i);
//             while(motor1_flag==1||motor2_flag==1||motor3_flag==1){
//                 HAL_Delay(10);
//             }
//         }
//     }
// } 
// }
