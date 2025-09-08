#include "main.h"
#include "MPU6050.h"
#include "math.h"
extern I2C_HandleTypeDef hi2c1;


void MPU6050_Init(void) {
    // 唤醒设备
    uint8_t data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 100);

    // 设置陀螺仪量程 ±500 °/s
    data = 0x08;  // 500dps
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 100);

    // 设置加速度计量程 ±4g
    data = 0x08;  // 4g
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 100);
}
void MPU6050_ReadRaw(MPU6050_t *p) {
    uint8_t buf[14];
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, buf, 14, 100);

    // 解析加速度计数据
    p->accel_x = (int16_t)((buf[0] << 8) | buf[1]);   // aX
    p->accel_y = (int16_t)((buf[2] << 8) | buf[3]);   // aY
    p->accel_z = (int16_t)((buf[4] << 8) | buf[5]);   // aZ

    // 解析陀螺仪数据
    p->gyro_x = (int16_t)((buf[8] << 8) | buf[9]);    // gX
    p->gyro_y = (int16_t)((buf[10] << 8) | buf[11]);  // gY
    p->gyro_z = (int16_t)((buf[12] << 8) | buf[13]);  // gZ
}
void ComputeEulerAngles(MPU6050_t *p) {
    static uint32_t prev_time = 0;
    uint32_t current_time = HAL_GetTick();
    p->dt = (current_time - prev_time) / 1000.0f;  // 转为秒
    prev_time = current_time;


    // 转换为实际物理值（加速度单位：g，陀螺仪单位：°/s）
    float aX = p->accel_x / 8192.0f;  // ±4g 量程，灵敏度 8192 LSB/g
    float aY = p->accel_y / 8192.0f;
    float aZ = p->accel_z / 8192.0f;

    float gX = (p->gyro_x - p->gyro_xbis) / 65.5f;    // ±500dps 量程，灵敏度 65.5 LSB/(°/s)
    float gY = (p->gyro_y - p->gyro_ybis)/ 65.5f;
    float gZ = (p->gyro_z - p->gyro_zbis)/ 65.5f;

    // 加速度计计算的静态角度
    float accel_roll = atan2f(aY, aZ) * RAD_TO_DEG;
    float accel_pitch = atan2f(-aX, sqrtf(aY*aY + aZ*aZ)) * RAD_TO_DEG;

    // 互补滤波融合角度
    p->roll = ALPHA * (p->roll + gX * p->dt) + (1 - ALPHA) * accel_roll;
    p->pitch = ALPHA * (p->pitch + gY * p->dt) + (1 - ALPHA) * accel_pitch;
}
void CalculateBias(MPU6050_t*p) {
    int32_t gx_sum = 0, gy_sum = 0, gz_sum = 0;
    for (int i = 0;i<100;i++) {
        MPU6050_ReadRaw(p);
        gx_sum += p->gyro_x;
        gy_sum += p->gyro_y;
        gz_sum += p->gyro_z;
        HAL_Delay(10);
    }
    p->gyro_xbis = gx_sum / 100.0f;
    p->gyro_ybis = gy_sum / 100.0f;
    p->gyro_zbis = gz_sum / 100.0f;

}