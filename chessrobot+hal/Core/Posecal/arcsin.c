#include <stdint.h>
#include "H_Oled.h"
#include <math.h>


float arcsin_standard(float x) {
    // 输入范围检查
    if (x > 1.0f) x = 1.0f;
    if (x < -1.0f) x = -1.0f;
    
    return asin(x);
}

float Q_rsqrt(float number) {
    union {
        float f;
        uint32_t i;
    } conv;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    conv.f = number;
    conv.i = 0x5f3759df - (conv.i >> 1); // 神奇数字，牛顿迭代初始猜测
    y = conv.f;
    y = y * (threehalfs - (x2 * y * y)); // 牛顿迭代一次
    return y;
}

// 优化的arcsin计算，使用前五项泰勒级数展开
float arcsin_approx(float x) {
    // 输入范围检查
    if (x < -1.0f || x > 1.0f) return 0.0f;
    if (x == 1.0f) return 1.57079632679f; // π/2
    if (x == -1.0f) return -1.57079632679f; // -π/2

    // 计算x的各次幂
    float x2 = x * x;
    float x3 = x * x2;
    float x5 = x3 * x2;
    float x7 = x5 * x2;
    float x9 = x7 * x2;
    float x11 = x9 * x2;
    float x13 = x11 * x2;
    float x15 = x13 * x2;

    // 预计算系数（保留10位有效数字）
    const float c1 = 1.0f;
    const float c2 = 0.1666666667f;      // 1/6
    const float c3 = 0.075f;              // 3/40
    const float c4 = 0.0446428571f;       // 5/112
    const float c5 = 0.0303819444f;       // 35/1152
    const float c6 = 0.0223721591f;       // 63/2816
    const float c7 = 0.0173527644f;       // 231/13312
    const float c8 = 0.0139648438f;       // 429/30720

    // 逐项计算并累加
    float term1 = c1 * x;
    float term2 = c2 * x3;
    float term3 = c3 * x5;
    float term4 = c4 * x7;
    float term5 = c5 * x9;
    float term6 = c6 * x11;
    float term7 = c7 * x13;
    float term8 = c8 * x15;

    return term1 + term2 + term3 + term4 + term5 + term6 + term7 + term8;
}

// 使用示例
void example1() {
    float last=arcsin_standard(0.8071f);
    OLED_ShowFloat(4,1,last,2,8);
}