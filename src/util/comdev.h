/*
 * comdev.h
 *
 *  Created on: 2012-9-26
 *      Author: zhangbo
 */

#ifndef COMDEV_H_
#define COMDEV_H_

#include "types.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void comdev_select_com(char com);

/**
 * 配置当前的串口设备通讯参数
 */
BOOLEAN comdev_setup(unsigned char com, int baudRate, int parity, int byteSize, int stopBits, void (*recv)(void*, size_t));

/**
 * 当前串口设备是否可写入数据。
 */
BOOLEAN comdev_can_write();

/**
 * 激活当前串口设备
 * state=1 如果当前串口已经配置过，则读取线程开始读取串口设备。
 * state=0 如果当前设备状态为激活状态，则停止读取串口设备的线程。
 */
BOOLEAN comdev_set_enable(BOOLEAN state);

/**
 * 当前串口设备是否允许写入。
 */
void comdev_set_output(BOOLEAN state);

/**
 * 从串口读取一行返回，还是读取到数据直接返回。
 */
void comdev_set_line_mode(BOOLEAN state);

/**
 * 从串口读取的数据是直接返回，还是前加0x80后加0x81返回。
 */
void comdev_set_direct_mode(BOOLEAN state);

/**
 * 向串口设备写数据，data为写入数据的开始地址，len为写入的字节数。
 */
size_t comdev_write(void* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* COMDEV_H_ */
