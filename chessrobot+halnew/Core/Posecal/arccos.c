#include <math.h>
#include "stdint.h"

// 计算arccos并返回弧度值
float arccos_standard(float x) {
    // 输入范围检查 (-1 ≤ x ≤ 1)
    if (x > 1.0) x = 1.0;
    if (x < -1.0) x = -1.0;
    
    // 调用标准库函数
    return acos(x);
}
