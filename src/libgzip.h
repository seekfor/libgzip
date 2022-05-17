#ifndef __LIBGZIP_H__
#define __LIBGZIP_H__

typedef struct
{
	int index;
	int width;
	int height;
	int bpp;
	int interlace;
	int colortype;

	int period;
	int* rgb;
}libphoto_output_t;

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#include "bits.h"
#include "huffman.h"
#include "lz77.h"
#include "gzip.h"
#include "png.h"




#endif
