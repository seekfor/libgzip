#include "../src/gzip.h"

#define TESTAPP_ID_GZ	0
#define TESTAPP_ID_PNG	1

#define TESTAPP	TESTAPP_ID_PNG


#if TESTAPP == TESTAPP_ID_PNG
static char bmp[1024 * 1024 * 16] = {0};
	int bmpEncode(int* rgb,int width,int height,char* bmp);
#endif

int main(int argc,char* argv[])
{
#if TESTAPP == TESTAPP_ID_GZ
	int len = gzDecode(argv[1],0,NULL,argv[2]);
	printf("gzip decode size = %d\r\n",len);
#elif TESTAPP == TESTAPP_ID_PNG
	libphoto_output_t info;
	void* png = pngCreate(argv[1]);
	if(0 == pngDecode(png,&info))
	{
		FILE* p;
		int size;
		size = bmpEncode(info.rgb,info.width,info.height,bmp);
		p = fopen(argv[2],"wb");
		fwrite(bmp,1,size,p);
		fclose(p);
		printf("PNG decode success!\r\n");
	}
	pngDestroy(png);
#endif
	return 0;
}

