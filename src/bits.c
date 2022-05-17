#include "libgzip.h"

unsigned int bitstreamSwap32(unsigned int val,int bits)
{
	unsigned int ret = 0;
	int i;
	for(i = 0; i < bits; i ++)
	{
		if(val & (1 << i))
		{
			ret |= 1 << (bits - 1 - i);
		}
	}
	return ret;
}

unsigned char bitstreamSwap8(unsigned char val,int bits)
{
	unsigned int ret = 0;
	int i;
	for(i = 0; i < bits; i ++)
	{
		if(val & (1 << i))
		{
			ret |= 1 << (bits - 1 - i);
		}
	}
	return ret;
}

void bitstreamInit(bitstream_t* bt,FILE* fp,unsigned char* buf,int size)
{
	memset(bt,0,sizeof(bitstream_t));
	if(fp)
	{
		buf = bt->pool;
		bt->fp = fp;
		size = fread(bt->pool,1,4096,fp);
	}
	bt->buf = buf + 4;
	bt->bufval = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	bt->size = size - 4;
	bt->rd = 0;
}

int bitstreamReadValue(bitstream_t* bt,int bits,int peek)
{
	int len;
	unsigned int ret;
	ret = (bt->bufval >> bt->rd) & ((1 << bits) - 1);
	if(peek)
	{
		if(bt->fp)
		{
			if(bt->size < 64)
			{
				memcpy(bt->pool,bt->buf,bt->size);
				len = fread(bt->pool + bt->size,1,4096 - bt->size,bt->fp);
				if(len > 0)
				{
					bt->size += len;
				}
				bt->buf = bt->pool;
			}
		}
		bt->rd += bits;
		while(bt->rd >= 8)
		{
			if(bt->size > 0)
			{
				bt->bufval >>= 8;
				bt->bufval |= (bt->buf[0] << 24);
				bt->buf ++;
				bt->rd -= 8;
				bt->size --;
			}
			else
			{
				break;
			}
		}
	}
	return ret;
}

int bitstreamReadBits(bitstream_t* bt,int bits,int peek)
{
	unsigned int ret = bitstreamReadValue(bt,bits,peek);
	return bitstreamSwap32(ret,bits);
}

void bitstreamSkip(bitstream_t* bt,int bits)
{
	bitstreamReadValue(bt,bits,1);
}

void bitstreamAlign(bitstream_t* bt)
{
	if(bt->rd & 0x07)
	{
		bitstreamSkip(bt,8 - (bt->rd & 0x07));
	}
}

int bitstreamMore(bitstream_t* bt)
{
	return bt->size || (bt->rd < 8);
}
