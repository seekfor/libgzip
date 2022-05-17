#include "libgzip.h"



int main(int argc,char* argv[])
{
	int len = gzipDecode(argv[1],0,NULL,argv[2]);
	printf("gzip decode size = %d\r\n",len);
	return 0;
}

