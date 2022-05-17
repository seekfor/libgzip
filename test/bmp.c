
#define SETDWORD(ptr,item)\
	{\
		*ptr++=(unsigned int)(item & 0xff);\
		*ptr++=(unsigned int)(item >> 8) & 0xff;\
		*ptr++=(unsigned int)(item >> 16) & 0xff;\
		*ptr++=(unsigned int)(item >> 24) & 0xff;\
	}
#define SETWORD(ptr,item)\
	{\
		*ptr++=(item & 0xff);\
		*ptr++=(item >> 8) & 0xff;\
	}

static int bmp_header(char* images,int bpp,int width,int height,int topdown)
{
	char* ptr = images;
	unsigned int i;
	*ptr++ ='B';
	*ptr++ ='M';
	ptr += 4;
	SETDWORD(ptr,0);
	i = 54 + (bpp <= 8?((1u << bpp)<<2):0);
	SETDWORD(ptr,i);
	SETDWORD(ptr,0x28);
	SETDWORD(ptr,width);
	if(topdown)
	{
		SETDWORD(ptr,-height);
	}
	else
	{
		SETDWORD(ptr,height);
	}
	SETWORD(ptr,1);
	SETWORD(ptr,bpp);
	SETDWORD(ptr,0);
	SETDWORD(ptr,0);
	SETDWORD(ptr,3780);
	SETDWORD(ptr,3780);
	SETDWORD(ptr,0);
	SETDWORD(ptr,0);
	return 14 + 0x28;
}

static int bmp_rgb24_encode(int* rgb,int width,int height,char* bmp)
{
	int i,j,k = 0;
	int line;
	int* ptr = rgb;
	int color;
	unsigned char skip;
	line = width * 3;
	skip = line & 0x03;
	if(skip) 
	{
		skip = 4 - skip;
	}
	for(i = 0;i < height;i++)
	{
		for(j = 0;j < width;j++)
		{
			color = *ptr++;
			bmp[k++] = ((color >> 16) & 0xff);
			bmp[k++] = ((color >> 8) & 0xff);
			bmp[k++] = (color & 0xff);
		}
		for(j = 0;j < skip;j++)
		{
			bmp[k++] = 0x00;
		}
	}
	return k;
}

static void bmp_update(char* bmp,int len)
{
	char* ptr = &bmp[2];
	int size;
	SETDWORD(ptr,len);
	ptr = &bmp[34];
	size = len - 0x28 -0x12;
	SETDWORD(ptr,size);
}


int bmpEncode(int* rgb,int width,int height,char* bmp)
{
	int len;
	int skip;
	int oft;
	len = width * 3;
	skip = (len & 0x03)?(4 - (len & 0x03)) * height:0;

	oft = bmp_header(bmp,24,width,height,1);
	len = oft + bmp_rgb24_encode(rgb,width,height,bmp + oft);
	bmp_update(bmp,len);
	return len;
}
