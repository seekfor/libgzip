#include "libgzip.h"

static int gz_decode_codelen(bitstream_t* bt,huffman_table_t* clen,int num)
{
	int i;
	int idx;
	int values[] = 
	{
		16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15
	};
	int cls[24] = {0};
	for(i = 0; i < num;i ++)
	{
		idx = values[i];
		cls[idx] = bitstreamReadValue(bt,3,1);
	}
	huffmanNormlize(cls,19,clen);
	return 0;
}

static int gz_decode_huffman(bitstream_t* bt,huffman_table_t* hclen,huffman_table_t* huffman,int num)
{
	int i;
	int cls[512] = {0};
	int oldval = 0;
	int numcls = 0;
	int times;
	huffman_item_t* item;
	for(numcls = 0; numcls < num;)
	{
		item = huffmanRead(bt,hclen);
		if(!item)
		{
			return -1;
		}
		switch(item->value)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			oldval = item->value;
			cls[numcls++] = oldval;
			break;
		case 16:
			times = bitstreamReadValue(bt,2,1) + 3;
			for(i = 0; i < times; i ++)
			{
				cls[numcls++] = oldval;
			}
			break;
		case 17:
			times = bitstreamReadValue(bt,3,1) + 3;
			for(i = 0; i < times; i ++)
			{
				cls[numcls++] = 0;
			}
			break;
		case 18:
			times = bitstreamReadValue(bt,7,1) + 11;
			for(i = 0; i < times; i ++)
			{
				cls[numcls++] = 0;
			}
			break;
		}
	}
	if(numcls != num)
	{
		return -1;
	}
	huffmanNormlize(cls,numcls,huffman);
	return 0;
}

static int gz_decode_block(bitstream_t* bt,huffman_table_t* lit,huffman_table_t* dist,short int* raw)
{
	huffman_item_t* item;
	int value;
	int eob = 0;
	int length,distance;
	int size = 0;
	while(!eob)
	{
		item = huffmanRead(bt,lit);
		if(!item)
		{
			return -1;
		}
		length = 0;
		value = item->value;
		switch(value)
		{
		default:
			*raw++ = (short int)0xffff;
			*raw++ = (short int)value;
			size ++;
			continue;
		case 256:
			eob = 1;
			continue;
		case 257:
		case 258:
		case 259:
		case 260:
		case 261:
		case 262:
		case 263:
		case 264:
			length = value - 254;
			break;
		case 265:
		case 266:
		case 267:
		case 268:
			length = 11 + (value - 265) * 2 + bitstreamReadValue(bt,1,1);
			break;
		case 269:
		case 270:
		case 271:
		case 272:
			length = 19 + (value - 269) * 4 + bitstreamReadValue(bt,2,1);
			break;
		case 273:
		case 274:
		case 275:
		case 276:
			length = 35 + (value - 273) * 8 + bitstreamReadValue(bt,3,1);
			break;
		case 277:
		case 278:
		case 279:
		case 280:
			length = 67 + (value - 277) * 16 + bitstreamReadValue(bt,4,1);
			break;
		case 281:
		case 282:
		case 283:
		case 284:
			length = 131 + (value - 281) * 32 + bitstreamReadValue(bt,5,1);
			break;
		case 285:
			length = 258;
			break;
		}

		item = huffmanRead(bt,dist);
		if(!item)
		{
			return size;
		}
		value = item->value;
		switch(value)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			distance = 1 + value;
			break;
		case 4:
		case 5:
			distance = 5 + (value - 4) * 2 + bitstreamReadValue(bt,1,1);
			break;
		case 6:
		case 7:
			distance = 9 + (value - 6) * 4 + bitstreamReadValue(bt,2,1);
			break;
		case 8:
		case 9:
			distance = 17 + (value - 8) * 8 + bitstreamReadValue(bt,3,1);
			break;
		case 10:
		case 11:
			distance = 33 + (value - 10) * 16 + bitstreamReadValue(bt,4,1);
			break;
		case 12:
		case 13:
			distance = 65 + (value - 12) * 32 + bitstreamReadValue(bt,5,1);
			break;
		case 14:
		case 15:
			distance = 129 + (value - 14) * 64 + bitstreamReadValue(bt,6,1);
			break;
		case 16:
		case 17:
			distance = 257 + (value - 16) * 128 + bitstreamReadValue(bt,7,1);
			break;
		case 18:
		case 19:
			distance = 513 + (value - 18) * 256 + bitstreamReadValue(bt,8,1);
			break;
		case 20:
		case 21:
			distance = 1025 + (value - 20) * 512 + bitstreamReadValue(bt,9,1);
			break;
		case 22:
		case 23:
			distance = 2049 + (value - 22) * 1024 + bitstreamReadValue(bt,10,1);
			break;
		case 24:
		case 25:
			distance = 4097 + (value - 24) * 2048 + bitstreamReadValue(bt,11,1);
			break;
		case 26:
		case 27:
			distance = 8193 + (value - 26) * 4096 + bitstreamReadValue(bt,12,1);
			break;
		case 28:
		case 29:
			distance = 16385 + (value - 28) * 8192 + bitstreamReadValue(bt,13,1);
			break;
		default:
			return size;
		}
		*raw++ = (short int)length;
		*raw++ = (short int)distance;
		size ++;
	};
	return size;
}


static int gz_decode_with_file(char* ifile,short int* raw,char* ofile)
{
	int len;
	int nlen;
	unsigned char flag;
	bitstream_t bt;
	int btype,bfinal = 0;
	huffman_table_t hclen;
	huffman_table_t hlit;
	huffman_table_t hdist;
	int numhlit,numhclen,numhdist;
	unsigned char buf[64];
	int size;
	int dsize = 0;
	int block = 0;
	int nofile = 0;
	int noalloc = 1;
	FILE* out = NULL;
	FILE* in = fopen(ifile,"rb");
	if(!in)
	{
		return 0;
	}
	size = fread(buf,1,10,in);

	if(memcmp(buf,"\x1f\x8b\x08",3))
	{
		fclose(in);
		return 0;
	}

	if(ofile)
	{
		out = fopen(ofile,"wb");
		if(!out)
		{
			nofile = 1;
		}
	}
	if(!raw)
	{
		raw = (short int*)malloc(256 * 1024);
		if(!raw)
		{
			fclose(in);
			if(out)
			{
				fclose(out);
			}
			return 0;
		}
		noalloc = 0;
	}

	flag = buf[3];
	
	if(flag & 0x04)
	{
		fread(buf,1,2,in);
		len = (buf[1] << 8) | buf[0];
		fseek(in,len,SEEK_CUR);
	}
	if(flag & 0x08)
	{
		char oname[256] = {0};
		char* name = oname;
		while(!feof(in))
		{
			fread(name,1,1,in);
			if(name[0] == 0x00)
			{
				break;
			}
			name++;
		}
		if(!strcmp(oname,""))
		{
			sprintf(oname,"%s.unzip",ifile);
		}
		if(!ofile)
		{
			out = fopen(oname,"wb");
			if(!out)
			{
				fclose(in);
				free(raw);
				return 0;
			}
		}
	}
	if(flag & 0x10)
	{
		while(!feof(in))
		{
			fread(buf,1,1,in);
			if(buf[0] == 0x00)
			{
				break;
			}
		}
	}
	if(flag & 0x02)
	{
		fseek(in,2,SEEK_CUR);
	}
	bitstreamInit(&bt,in,NULL,0);
	while(!bfinal && bitstreamMore(&bt))
	{
		block ++;
		bfinal = bitstreamReadValue(&bt,1,1);
		btype = bitstreamReadValue(&bt,2,1);
		switch(btype)
		{
		case 0x00:/*no compression*/
			bitstreamAlign(&bt);
			len = bitstreamReadValue(&bt,16,1);
			nlen = bitstreamReadValue(&bt,16,1);
			if((len + nlen) != 0xffff)
			{
				bfinal = 1;
				continue;
			}
			while(len --)
			{
				buf[0] = bitstreamReadValue(&bt,8,1);
				if(out)
				{
					fwrite(buf,1,1,out);
				}
				else
				{
					memcpy(ofile + dsize,buf,1);
				}
				dsize ++;
			}
			continue;
		case 0x01:/*FIXED huffman table*/
			huffmanInitLITable(&hlit);
			huffmanInitDISTable(&hdist);
			size = gz_decode_block(&bt,&hlit,&hdist,raw);
			break;
		case 0x02:/*DYNAMIC huffman table*/
			numhlit = bitstreamReadValue(&bt,5,1) + 257;
			numhdist = bitstreamReadValue(&bt,5,1) + 1;
			numhclen = bitstreamReadValue(&bt,4,1) + 4;
			gz_decode_codelen(&bt,&hclen,numhclen);
			gz_decode_huffman(&bt,&hclen,&hlit,numhlit);
			gz_decode_huffman(&bt,&hclen,&hdist,numhdist);
			size = gz_decode_block(&bt,&hlit,&hdist,raw);
			break;
		default:
			bfinal = 1;
			continue;
		}
		if(size > 0)
		{
			if(out)
			{
				len = lz77DecodeFile(raw,size,out);
			}
			else
			{
				len = lz77Decode(raw,size,ofile + dsize);
			}
			dsize += len;
		}
	};
	fclose(in);
	if(out)
	{
		fclose(out);
	}
	if(!noalloc)
	{
		free(raw);
	}
	return dsize;
}

int gzipDecode(unsigned char* buf,int size,short int* raw,char* out)
{
	int len;
	int nlen;
	unsigned char flag;
	bitstream_t bt;
	int btype,bfinal = 0;
	int blksize;
	huffman_table_t hclen;
	huffman_table_t hlit;
	huffman_table_t hdist;
	int numhlit,numhclen,numhdist;
	int noalloc = 1;
	FILE* wr = NULL;

	if(!size)
	{
		return gz_decode_with_file((char*)buf,raw,out);
	}
	if(out)
	{
		wr = fopen(out,"wb");
	}

	if(!memcmp(buf,"\x1f\x8b\x08",3))
	{
		flag = buf[3];
		buf += 10;
		size -= 10;
		
		if(flag & 0x04)
		{
			len = (buf[1] << 8) | buf[0];
			buf += 2 + len;
			size -= 2 + len;
		}
		if(flag & 0x08)
		{
			char oname[256] = {0};
			strcpy(oname,(char*)buf);
			len = strlen(oname) + 1;
			if(!out)
			{
				wr = fopen(oname,"wb");
			}
			buf += len;
			size -= len;
		}
		if(flag & 0x10)
		{
			while(*buf != 0x00)
			{
				size --;
				buf ++;
			}
			buf ++;
			size --;
		}
		if(flag & 0x02)
		{
			buf += 2;
			size -= 2;
		}
	}

	if(!raw)
	{
		raw = (short int*)malloc(256 * 1024);
		if(!raw)
		{
			return 0;
		}
		noalloc = 0;
	}

	bitstreamInit(&bt,NULL,buf,size);
	size = 0;
	while(!bfinal && bitstreamMore(&bt))
	{
		blksize = 0;
		bfinal = bitstreamReadValue(&bt,1,1);
		btype = bitstreamReadValue(&bt,2,1);
		switch(btype)
		{
		case 0x00:/*no compression*/
			bitstreamAlign(&bt);
			len = bitstreamReadValue(&bt,16,1);
			nlen = bitstreamReadValue(&bt,16,1);
			if((len + nlen) != 0xffff)
			{
				bfinal = 1;
				continue;
			}
			while(len --)
			{
				unsigned char ch;
				if(wr == NULL)
				{
					*out++ = bitstreamReadValue(&bt,8,1);
					size ++;
				}
				else
				{
					ch = bitstreamReadValue(&bt,8,1);
					fwrite(&ch,1,1,wr);	
				}
				size ++;
			}
			continue;
		case 0x01:/*FIXED huffman table*/
			huffmanInitLITable(&hlit);
			huffmanInitDISTable(&hdist);
			blksize = gz_decode_block(&bt,&hlit,&hdist,raw);
			break;
		case 0x02:/*DYNAMIC huffman table*/
			numhlit = bitstreamReadValue(&bt,5,1) + 257;
			numhdist = bitstreamReadValue(&bt,5,1) + 1;
			numhclen = bitstreamReadValue(&bt,4,1) + 4;
			gz_decode_codelen(&bt,&hclen,numhclen);
			gz_decode_huffman(&bt,&hclen,&hlit,numhlit);
			gz_decode_huffman(&bt,&hclen,&hdist,numhdist);
			blksize = gz_decode_block(&bt,&hlit,&hdist,raw);
			break;
		default:
			bfinal = 1;
			continue;
		}
		if(blksize > 0)
		{
			if(wr == NULL)
			{
				len = lz77Decode(raw,blksize,out);
				out += len;
			}
			else
			{
				len = lz77DecodeFile(raw,blksize,wr);
			}
			size += len;
		}
	};
	if(!noalloc)
	{
		free(raw);
	}
	return size;
}
