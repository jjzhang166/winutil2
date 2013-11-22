/*
 * comdev.cpp
 *
 *  Created on: 2012-9-26
 *      Author: zhangbo
 */

#include "comdev.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <dos.h>
#include <io.h>
#include <conio.h>
#include "types.h"
#include "current_com.h"
#include "log.h"

#define COM_READ_BUFFER_SIZE 1024

static BOOLEAN setup = 0;
static BOOLEAN com_enable = 0;
static BOOLEAN output = 0;
static BOOLEAN direct_mode = 1;
static BOOLEAN line_mode = 0;
BOOLEAN reading = 0;
static void (*comdev_ex_send)(void*, size_t);
static void comdev_read_proc_start();

BOOLEAN comdev_setup(unsigned char com, int baudRate, int parity, int byteSize, int stopBits, void (*recv)(void*, size_t)) {
	COMPARAM p;
	p.bEvtChar = 0x0d;
	p.fBinary = 1;

	p.bComId = com;
	comdev_ex_send = recv;

	switch (baudRate) {
	case 0:
		p.dwBaudRate = 9600;
		break;
	case 1:
		p.dwBaudRate = 4800;
		break;
	case 2:
		p.dwBaudRate = 2400;
		break;
	case 3:
		p.dwBaudRate = 1200;
		break;
	case 4:
		p.dwBaudRate = 900;
		break;
	case 5:
		p.dwBaudRate = 600;
		break;
	case 6:
		p.dwBaudRate = 300;
		break;
	case 7:
		p.dwBaudRate = 150;
		break;
	case 8:
		p.dwBaudRate = 75;
		break;
	case 9:
		p.dwBaudRate = 800;
		break;
	default:
		p.dwBaudRate = 9600;
		break;
	}

	switch (parity) {
	case 0: //none
		p.bParity = 0;
		break;
	case 1: //1
		p.bParity = 1;
		break;
	case 2:
		p.bParity = 2;
		break;
	default:
		p.bParity = 0;
		break;
	}

	switch (byteSize) {
	case 0:
		p.bByteSize = 8;
		break;
	case 1:
		p.bByteSize = 7;
		break;
	default:
		p.bByteSize = 8;
		break;
	}

	switch (stopBits) {
	case 0:
		p.bStopBits = 0; //1
		break;
	case 1:
		p.bStopBits = 2; //2
		break;
	default:
		p.bStopBits = 1; //1.5
		break;
	}

	current_com_setup(&p);
	setup = 1;
	if (com_enable && setup) {
		current_com_open();
		comdev_read_proc_start();
	}
	return TRUE;
}

static void comdev_send(unsigned char* data, unsigned long len) {
	if (!direct_mode) {
		int i = 0, j = 0;
		char* pout = malloc(len * 3);
		for (i = 0; i < len; i++) {
			pout[j++] = 0x80;
			pout[j++] = data[i];
			pout[j++] = 0x81;
		}
		comdev_ex_send(pout, len * 3);
		free(pout);
	} else {
		comdev_ex_send((char*) data, len);
	}
}

static unsigned char inBuffer[COM_READ_BUFFER_SIZE];
static unsigned long inLen = 0;
static unsigned char tempBuffer[COM_READ_BUFFER_SIZE * 10];
static long tempLen = 0;
static unsigned long WINAPI comdev_read_thread(void* device) {
	log_write_str(COM_INFO, "com_read_thread start");
	reading = 1;
	while (reading) {
		Sleep(5);
		if (!com_enable) {
			continue;
		}
		inLen = current_com_read(inBuffer, COM_READ_BUFFER_SIZE);
		if (inLen == 0) {
			continue;
		}
		if (line_mode) {
			int last = 0, i = 0;

			for (i = 0; i < inLen; i++) {
				if (inBuffer[i] == 0x0D) {
					memcpy(tempBuffer + tempLen, inBuffer + last, i + 1 - last);
					tempLen += i + 1 - last;
					last = i + 1;
					comdev_send(tempBuffer, tempLen);
					tempLen = 0;
				}
			}
			memcpy(tempBuffer + tempLen, inBuffer + last, i - last);
			tempLen += i - last;
		} else {
			comdev_send(inBuffer, inLen);
		}
	}
	log_write_str(COM_INFO, "com_read_thread ended");

	return 0;
}

static void comdev_read_proc_start() {
	if (reading == 0) {
		CreateThread(NULL, 0, comdev_read_thread, NULL, 0, 0);
		reading = 1;
	}
}

static void comdev_read_proc_stop() {
	reading = 0;
	Sleep(10);
}

BOOLEAN comdev_set_enable(BOOLEAN state) {
	com_enable = state;
	if (com_enable && setup) {
		if (current_com_open() == -1) {
			com_enable = 0;
			return FALSE;
		}
		comdev_read_proc_start();
	} else {
		comdev_read_proc_stop();
		current_com_close();
		setup = 0;
	}
	return TRUE;
}

void comdev_set_output(BOOLEAN state) {
	output = state;
	if (state) {
		log_write_str(COM_INFO, "com out 1");
	} else {
		log_write_str(COM_INFO, "com out 0");
	}
}

BOOLEAN comdev_can_write() {
	return output && com_enable;
}

void comdev_set_line_mode(BOOLEAN state) {
	line_mode = state;
	if (state) {
		log_write_str(COM_INFO, "line mode 1");
	} else {
		log_write_str(COM_INFO, "line mode 0");
	}
}

void comdev_set_direct_mode(BOOLEAN state) {
	direct_mode = state;
	if (state) {
		log_write_str(COM_INFO, "direct mode 1");
	} else {
		log_write_str(COM_INFO, "direct mode 0");
	}
}

size_t comdev_write(void* data, size_t len) {
	return current_com_write(data, len);
}

