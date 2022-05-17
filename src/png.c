#include "libgzip.h"

typedef struct
{
	FILE* fp;
	libphoto_output_t info;
	unsigned int pal[256 * 3];
	int numpal;
	int* rgb;
	char* fdat;
	char* odat;
	char* idat;
	size_t dsize;
	char* block;
	size_t msize;
}libpng_t;

static int png_get_linesize(libpng_t* png)
{
	int linesize;
	switch(png->info.colortype)
	{
		case 0x00:/*Gray image,bpp should be 1/2/4/8*/
			linesize = png->info.width * png->info.bpp / 8;
			break;
		default:
		case 0x02:/*true color,bpp should be 8/16*/
			linesize = 3 * png->info.width * png->info.bpp / 8;
			break;
		case 0x03:/*Indexed color,bpp should be 1/2/4/8*/
			linesize = png->info.width * png->info.bpp / 8;
			break;
		case 0x04:/*Gray image with alpha,bpp should be 8/16*/
			linesize = 2 * png->info.width * png->info.bpp / 8;
			break;
		case 0x06:/*true color with alpha,bpp should be 8/16*/
			linesize = 4 * png->info.width * png->info.bpp / 8;
			break;
	}
	return linesize;
}

static int png_decode_ihdr(libpng_t* png,unsigned char* buf)
{
	int linesize;
	libphoto_output_t* info = &png->info;
	info->width = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	info->height = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
	info->bpp = buf[8];
	info->colortype = buf[9];
	info->interlace = buf[12];
	if(!png->rgb)
	{
		linesize = png_get_linesize(png);
		png->rgb = (int*)malloc(linesize * info->height * sizeof(int));
		png->odat = (char*)malloc((linesize + 1) * info->height);
		png->idat = (char*)malloc(linesize * info->height);
		png->fdat = (char*)malloc(linesize * info->height);
	}
	info->rgb = png->rgb;
	printf("width = %d,height = %d,bpp = %d,interlace = %d,color type = %d\r\n",info->width,info->height,info->bpp,info->interlace,buf[9]);
	return 0;
}

static int png_decode_plte(libpng_t* png,unsigned char* buf)
{
	unsigned int* pal = png->pal;
	int i;
	png->numpal = (png->info.bpp > 8)?256:(2 << png->info.bpp);
	for(i = 0; i < png->numpal; i ++)
	{
		pal[i] = (buf[2] << 16) | (buf[1] << 8) | buf[0];
		buf += 3;
	}
	return 0;
}

static int png_decode_idat(libpng_t* png,unsigned char* buf,int size)
{
	memcpy(png->idat + png->dsize,buf,size);
	png->dsize += size;
	return png->dsize;
}

static int png_filter_picture(libpng_t* png)
{
	unsigned char* fdat = (unsigned char*)png->fdat;
	int num;
	int i;
	int j;
	int k;
	int x;
	unsigned int pa,pb,pc,pr;
	unsigned int p;
	unsigned int a,b,c;
	unsigned char type;
	char* src = (char*)png->odat;
	int linesize = png_get_linesize(png);
	int w;
	int h = png->info.height;
	switch(png->info.colortype)
	{
	case 2:
		num = 3;
		break;
	case 4:
		num = 2;
		break;
	case 6:
		num = 4;
		break;
	default:
		num = 1;
		break;

	}
	w = linesize / num;
	
	for(i = 0; i < h; i ++)
	{
		type = *src ++;
		for(j = 0; j < w; j ++)
		{
			for(k = 0;k < num; k ++,fdat ++,src ++)
			{
				x = *(src + 0);
				a = (j == 0)?0x00:*(fdat - num);
				b = (i == 0)?0x00:*(fdat - linesize);
				c = (i == 0)?0x00:((j == 0)?0x00:*(fdat - linesize - num));
				switch(type)
				{
				default:
					return -1;
				case 0:/*None*/
					*fdat = x;
					break;
				case 1:/*Sub*/
					*fdat = x + a;
					break;
				case 2:/*Up*/
					*fdat = x + b;
					break;
				case 3:/*Average*/
					*fdat = x + ((a + b) >> 1);
					break;
				case 4:/*Paeth*/
					p = a + b - c;
					pa = abs(p - a);
					pb = abs(p - b);
					pc = abs(p - c);
					if((pa <= pb) && (pa <= pc) )
					{
						pr = a;
					}
					else if(pb <= pc)
					{
						pr = b;
					}
					else
					{
						pr = c;
					}
					*fdat = x + pr;
					break;
				}
			}
		}
	}
	return 0;
}

static int png_convert_picture(libpng_t* png)
{
	unsigned int* rgb = (unsigned int*)png->info.rgb;
	int i;
	int j;
	if(png->info.bpp == 1)
	{
	}
	else if(png->info.bpp == 2)
	{
	}
	else if(png->info.bpp == 4)
	{

	}
	else if(png->info.bpp == 8)
	{
		unsigned char* src = (unsigned char*)png->fdat;
		switch(png->info.colortype)
		{
		case 0x00:/*Gray image*/
			break;
		case 0x01:
		case 0x05:/*no these cases*/
			break;
		case 0x02:/*RGB color*/
			for(i = 0; i < png->info.height; i ++)
			{
				for(j = 0; j < png->info.width; j ++,src += 3)
				{
					*rgb ++ = 0xff000000 | (src[2] << 16) | (src[1] << 8) | src[0];
				}
			}
			break;
		case 0x03:/*Indexed color*/
			break;
		case 0x04:/*Gray image with alpha*/
			break;
		case 0x06:/*RGBA color*/
			for(i = 0; i < png->info.height; i ++)
			{
				for(j = 0; j < png->info.width; j ++,src += 4)
				{
					*rgb++ = (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
				}
			}
			break;
		}
	}
	else if(png->info.bpp == 16)
	{
	}
	return 0;
}

static int png_decode_picture(libpng_t* png)
{
	/*
		MARK:the first two bytes are zlib header,and ignored it now!
		<CMF> <FLG>
		CMF:always it is 0x78
			D0 ~ D3,compression method,only 8 is avail
			D4 ~ D7,compression info
		FLG:
			D0 ~ D4,FCHECK,check bits for CMF & FLG
			D5,FDICT,preset dictionary
			D6 ~ D7,compression level
	*/
	int len = gzipDecode((unsigned char*)png->idat + 2,png->dsize - 2,NULL,(char*)png->odat);
	png_filter_picture(png);
	return png_convert_picture(png);
}

static int png_decode_block(libpng_t* png)
{
	unsigned char mem[8] = {0};
	char* buf;
	size_t size;
	if(4 != fread(mem,1,4,png->fp))
	{
		return 1;
	}
	size = ((mem[0] << 24) | (mem[1] << 16) | (mem[2] << 8) | mem[3]);
	size += 8;
	if(!png->block || (size > png->msize))
	{
		if(png->block)
		{
			free(png->block);
		}
		if(size < 65536)
		{
			png->block = (char*)malloc(65536);
			png->msize = 65536;
		}
		else
		{
			png->msize = size;
			png->block = (char*)malloc(size);
		}
		if(!png->block)
		{
			return 3;
		}
	}
	if(size != fread(png->block,1,size,png->fp))
	{
		return 4;
	}
	buf = png->block;
	if(!memcmp(buf,"IHDR",4))
	{
		png_decode_ihdr(png,(unsigned char*)buf + 4);
	}
	else if(!memcmp(buf,"PLTE",4))
	{
		png_decode_plte(png,(unsigned char*)buf + 4);
	}
	else if(!memcmp(buf,"IDAT",4))
	{
		png_decode_idat(png,(unsigned char*)buf + 4,size - 8);
	}
	else if(!memcmp(buf,"IEND",4))
	{
		return png_decode_picture(png);
	}
	else
	{
	}
	return -1;
}

void* pngCreate(char* filename)
{
	FILE* fp;
	char buf[8];
	libpng_t* png;
	fp = fopen(filename,"rb");
	if(!fp)
	{
		return NULL;
	}
	fread(buf,1,8,fp);
	if(memcmp(buf,"\x89\x50\x4e\x47\x0d\x0a\x1a\x0a",8))
	{
		fclose(fp);
		return NULL;
	}
	png = (libpng_t*)malloc(sizeof(libpng_t));
	if(!png)
	{
		return NULL;
	}
	memset(png,0,sizeof(libpng_t));
	png->fp = fp;

	return png;
}

int pngDecode(void* hdl,libphoto_output_t* attr)
{
	int ret;
	libpng_t* png = (libpng_t*)hdl;
	do
	{
		ret = png_decode_block(png);
		if(ret == 0)
		{
			if(attr)
			{
				*attr = png->info;
			}
			break;
		}
	}while(ret < 0);
	return ret;
}

int pngDestroy(void* hdl)
{
	libpng_t* png = (libpng_t*)hdl;
	fclose(png->fp);
	free(png->block);
	free(png->idat);
	free(png->odat);
	free(png->fdat);
	free(png->rgb);
	free(png);
	return 0;
}

