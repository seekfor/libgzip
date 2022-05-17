#ifndef __LZ77_H__
#define __LZ77_H__


#ifdef __cplusplus
extern "C"
{
#endif

	int lz77Decode(short int* raw,int size,char* out);
	int lz77DecodeFile(short int* raw,int size,FILE* out);

#ifdef __cplusplus
}
#endif

#endif
