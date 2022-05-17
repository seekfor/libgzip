#include "libgzip.h"

#define MAX_WIN_SIZE	(32 * 1024)

static unsigned char win[MAX_WIN_SIZE];
static unsigned char* pwin = &win[MAX_WIN_SIZE];
static int wsize = 0;

static int lz77_add_dict(char* buf,int size)
{
	int skip;
	if((wsize + size) > MAX_WIN_SIZE)
	{
		skip = wsize + size - MAX_WIN_SIZE;
		if(skip)
		{
			memcpy(win,pwin + skip,wsize - skip);
		}
		memcpy(win + wsize - skip,buf,size);
		wsize = MAX_WIN_SIZE;
		pwin = win;
	}
	else
	{
		if(wsize)
		{
			memcpy(pwin - size,pwin,wsize);
		}
		memcpy(pwin - size + wsize ,buf,size);
		pwin -= size;
		wsize += size;
	}
	return wsize;
}

static void lz77_get_dict(char* buf,int d,int l)
{
	unsigned char* item;
	item = (win + MAX_WIN_SIZE) - d;
	if(d >= l)
	{
		memcpy(buf,item,l);
	}
	else
	{
		while(l >= d)
		{
			memcpy(buf,item,d);
			l -= d;
			buf += d;
		}
		if(l)
		{
			memcpy(buf,item,l);
		}
	}
}

int lz77Decode(short int* raw,int size,char* out)
{
	short int l;
	short int d;
	char* oldout = out;
	while(size --)
	{
		l = *raw ++;
		d = *raw ++;
		if(l == -1)
		{
			*out = d & 0xff;
			lz77_add_dict(out,1);
			out++;
		}
		else
		{
			lz77_get_dict(out,d,l);
			lz77_add_dict(out,l);
			out += l;
		}
		
	}
	return (size_t)out - (size_t)oldout;
}

int lz77DecodeFile(short int* raw,int size,FILE* out)
{
	short int l;
	short int d;
	int dsize = 0;
	char obuf[512];
	while(size --)
	{
		l = *raw ++;
		d = *raw ++;
		if(l == -1)
		{
			obuf[0] = d & 0xff;
			lz77_add_dict(obuf,1);
			fwrite(obuf,1,1,out);
			dsize ++;
		}	
		else
		{
			lz77_get_dict(obuf,d,l);
			lz77_add_dict(obuf,l);
			fwrite(obuf,1,l,out);
			dsize += l;
		}
		
	}
	return dsize;
}

