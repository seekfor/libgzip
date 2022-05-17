#ifndef __GZIP_H__
#define __GZIP_H__

#ifdef __cplusplus
extern "C"
{
#endif

	int gzipDecode(unsigned char* buf,int size,short int* raw,char* out);

#ifdef __cplusplus
}
#endif

#endif
