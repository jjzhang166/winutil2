#ifndef	_ENCRYPT_H
#define	_ENCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

int encrypt_3des(const char *key1, const char *key2, const char *key3, const unsigned char *in,
		unsigned char *out);

int decrypt_3des(const char *key1, const char *key2, const char *key3, const unsigned char *in,
		unsigned char *out);

int md5_string(const unsigned char *data, unsigned long len, char *strMd5);

#ifdef __cplusplus
}
#endif

#endif	//_ENCRYPT_H
