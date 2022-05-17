#ifndef __BITS_H__
#define __BITS_H__

typedef struct
{
	FILE* fp;
	unsigned char pool[4096];
	unsigned char* buf;
	unsigned int bufval;
	int size;
	int rd;
}bitstream_t;


#ifdef __cplusplus
extern "C"
{
#endif

	void bitstreamInit(bitstream_t* bt,FILE* fp,unsigned char* buf,int size);
	int bitstreamReadValue(bitstream_t* bt,int bits,int peek);
	int bitstreamReadBits(bitstream_t* bt,int bits,int peek);
	void bitstreamSkip(bitstream_t* bt,int bits);
	void bitstreamAlign(bitstream_t* bt);
	int bitstreamMore(bitstream_t* bt);
	unsigned int bitstreamSwap32(unsigned int val,int bits);
	unsigned char bitstreamSwap8(unsigned char val,int bits);

#ifdef __cplusplus
}
#endif

#endif
