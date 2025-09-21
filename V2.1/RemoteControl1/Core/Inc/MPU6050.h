//
// Created by z on 2025/8/31.
//

#ifndef REMOTECONTROL_MPU6050_H
#define REMOTECONTROL_MPU6050_H

#define MPU6050_ADDR         0x68 << 1  // I2C 地址（AD0=0）
#define SMPLRT_DIV_REG       0x19
#define GYRO_CONFIG_REG      0x1B
#define ACCEL_CONFIG_REG     0x1C
#define PWR_MGMT_1_REG       0x6B
#define ACCEL_XOUT_H_REG     0x3B
#define RAD_TO_DEG 57.2957795f  // 弧度转角度
#define ALPHA      0.96f        // 滤波系数
typedef struct {
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    float roll, pitch;
    int16_t accel_xbis, accel_ybis, accel_zbis;
    int16_t gyro_xbis, gyro_ybis, gyro_zbis;
    float dt;  // 采样时间间隔（单位：秒）
}MPU6050_t;
void MPU6050_Init(void);
void MPU6050_ReadRaw(MPU6050_t *p);
void ComputeEulerAngles(MPU6050_t *p);
void CalculateBias(MPU6050_t*p);

#endif //REMOTECONTROL_MPU6050_H