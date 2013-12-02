#ifndef	_ENCRYPT_H
#define	_ENCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

int encrypt_3des(const unsigned char *key1, const unsigned char *key2,
		const unsigned char *key3, const unsigned char *in, size_t inLen, char *out);

int decrypt_3des(const unsigned char *key1, const unsigned char *key2,
		const unsigned char *key3, const unsigned char *in, size_t inLen, char *out);

int md5_string(const unsigned char *data, unsigned long len, char *md5);

#ifdef __cplusplus
}
#endif

#endif	//_ENCRYPT_H
