#include "arcsin.h"
#include "arccos.h"
#include "arctan2.h"
#include "tmc2209.h"
#include "H_Oled.h"

#include "math.h"

#define MY_PI 3.1415926f
#define MY_PI_2 MY_PI/2

//大小臂初始角度
float arm1_angle=22.861f;
float arm2_angle=90.0f;

//判断是否为合法范围
uint8_t scopecal(double x, double y) { 
    uint8_t flag = 0;
    if(x*x+(y-140)*(y-140) >= 25600) flag++;
    if(x*x+y*y>=3918.76) flag++;
    if(x*x+(y+160)*(y+160)>=19600) flag++;
    if(x*x+y*y<=74982.87) flag++;
    if(y>=-119.52) flag++;
    if(flag==5) return 1;
    else return 0;
}

//求取大臂与Y正半轴角度
//(x - 140\sin\theta)^2 + (y - 140\cos\theta)^2 = 160^2 \quad
float arm1_anglecal(double x, double y) { 
    float arctan2 = fast_atan2_std(y, x);
    float temp=(x*x+y*y-6000)/(280*sqrt(x*x+y*y));
    float arcsin = arcsin_standard(temp);
    float theta =arcsin-arctan2;
    //OLED_ShowFloat(3,1,theta*180.0f/MY_PI,4,6);
    return theta*180.0f/MY_PI;//弧度转换为角度返回
}

//计算小臂与大臂夹角
float arm2_anglecal(double x, double y) { 
    float temp=(45200-x*x-y*y)/44800;
    float arccos= arccos_standard(temp);
    return arccos*180.0f/MY_PI;//弧度转换为角度返回
}

//大臂初始角为22.861°小臂初始角为90°
void angle_set(double x, double y){
    if(scopecal(x,y)==1){
        //22.861°-90°
        float angle_arm1=arm1_anglecal(x,y);

        //180-(90-angle_arm1)-angle_arm2为小臂相对于x正半轴的夹角
        float angle_arm2=180.0f-(90.0f-angle_arm1)-arm2_anglecal(x,y);

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
}
