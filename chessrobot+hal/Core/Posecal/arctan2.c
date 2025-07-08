#include <math.h>
#include "stdint.h"
#include "H_Oled.h"

#define M_PI_2 1.5707963f

float fast_atan2_std(float y, float x) {
    return atan2f(y, x);
}

void example2() { 
    float test=fast_atan2_std(0, 250);
    OLED_ShowFloat(3,1,test,4,4);
}
