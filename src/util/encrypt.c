#include <stdio.h>
#include <string.h>
#include "des.h"
#include "pub.h"
#include "encrypt.h"
#include "../util/md5.h"
#include <malloc.h>

char* ch64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char *base64Encode(unsigned char *src, int srclen) {
	int n, buflen, i, j;
	int pading = 0;
	unsigned char *buf;
	static unsigned char *dst;

	buf = src;
	buflen = n = srclen;
	if (n % 3 != 0) /* pad with '=' by using a temp buffer */
	{
		pading = 1;
		buflen = n + 3 - n % 3;
		buf = malloc(buflen + 1);
		memset(buf, 0, buflen + 1);
		memcpy(buf, src, n);
		for (i = 0; i < 3 - n % 3; i++)
			buf[n + i] = '=';
	}
	dst = malloc(buflen * 4 / 3 + 1);
	memset(dst, 0, buflen * 4 / 3 + 1);
	for (i = 0, j = 0; i < buflen; i += 3, j += 4) {
		dst[j] = (buf[i] & 0xFC) >> 2;
		dst[j + 1] = ((buf[i] & 0x03) << 4) + ((buf[i + 1] & 0xF0) >> 4);
		dst[j + 2] = ((buf[i + 1] & 0x0F) << 2) + ((buf[i + 2] & 0xC0) >> 6);
		dst[j + 3] = buf[i + 2] & 0x3F;
	}
	for (i = 0; i < buflen * 4 / 3; i++) /* map 6 bit value to base64 ASCII character */
		dst[i] = ch64[dst[i]];
	if (pading)
		free(buf);
	return dst;
}

unsigned char *base64Decode(char *src) {
	int n, i, j;
	char *p;
	static unsigned char *dst;

	n = strlen(src);
	for (i = 0; i < n; i++) /* map base64 ASCII character to 6 bit value */
	{
		p = strchr(ch64, src[i]);
		if (!p)
			break;
		src[i] = p - ch64;
	}
	dst = malloc(n * 3 / 4 + 1);
	memset(dst, 0, n * 3 / 4 + 1);
	for (i = 0, j = 0; i < n; i += 4, j += 3) {
		dst[j] = (src[i] << 2) + ((src[i + 1] & 0x30) >> 4);
		dst[j + 1] = ((src[i + 1] & 0x0F) << 4) + ((src[i + 2] & 0x3C) >> 2);
		dst[j + 2] = ((src[i + 2] & 0x03) << 6) + src[i + 3];
	}
	return dst;
}

void tranStrTo2Bit(char *str, char *out2bit) {
	//把字符串16进制数转换成字符流
	char data[9];
	char tmp[3];
	int i = 0;
	memset(data, 0x00, 9);
	for (i = 0; i < 8; i++) {
		tmp[0] = *(str + i * 2);
		tmp[1] = *(str + i * 2 + 1);
		tmp[2] = 0;
		out2bit[i] = strtol(tmp, NULL, 16);
	}
}

int encrypt_3des(const unsigned char *key1, const unsigned char *key2, const unsigned char *key3, const unsigned char *in, size_t inLen,
		char *out) {
	int i = 0;
	unsigned char input[9]; //每次加密8个字节
	const unsigned char * ptr = NULL;
	unsigned char output[9];
	char strChar[128];
	ptr = in;

	while (ptr < (in + inLen)) {
		memset(input, 0x00, 9);
		memcpy(input, ptr, 8);
		memset(output, 0x00, 9);
		des(input, key1, output, DES_ENCRYPT);
		des(output, key2, input, DES_DECRYPT);
		des(input, key3, output, DES_ENCRYPT);
		for (i = 0; i < 8; i++) {
			sprintf(strChar, "%02x", (unsigned char) output[i]); //把二进制密文转换成字符串
			strcat(out, strChar);
		}

		ptr = ptr + 8;
	}
	return 0;
}

int decrypt_3des(const unsigned char *key1, const unsigned char *key2, const unsigned char *key3, const unsigned char *in, size_t inLen,
		char *out) {
	int i = 0;
	unsigned char input[9]; //每次加密8个字节
	const unsigned char * ptr = NULL;
	unsigned char output[9];
	char strChar[128];
	ptr = in;

	while (ptr < (in + inLen)) {
		memset(input, 0x00, 9);
		memcpy(input, ptr, 8);
		memset(output, 0x00, 9);
		des(input, key3, output, DES_DECRYPT);
		des(output, key2, input, DES_ENCRYPT);
		des(input, key1, output, DES_DECRYPT);
		for (i = 0; i < 8; i++) {
			sprintf(strChar, "%02x", (unsigned char) output[i]); //把二进制密文转换成字符串
			strcat(out, strChar);
		}
		ptr = ptr + 8;
	}
	return 0;
}

int md5_string(const unsigned char *data, unsigned long len,  char *md5) {
	MD5_CTX context;
	int i = 0;
	unsigned char digest[16];
	char strChar[128];
	char * ptr = NULL;

	MD5Init(&context);
	MD5Update(&context, data, len);
	MD5Final(digest, &context);
	ptr = strChar;
	for (i = 0; i < 16; i++) {
		sprintf(ptr, "%02X", (unsigned char) digest[i]);
		ptr = ptr + 2;
	};
	sprintf(md5, "%s", strChar);
	return 0;
}

