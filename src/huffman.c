#include "libgzip.h"

void huffmanInitLITable(huffman_table_t* t)
{
	int i;
	t->num = 288;
	for(i = 0; i < 144; i ++)
	{
		t->vlc[i].value = i;
		t->vlc[i].numbits = 8;
		t->vlc[i].code = 0x30 + i;
	}
	for(;i < 256; i ++)
	{
		t->vlc[i].value = i;
		t->vlc[i].numbits = 9;
		t->vlc[i].code = 0x190 + i - 144;
	}
	for(; i < 280; i ++)
	{
		t->vlc[i].value = i;
		t->vlc[i].numbits = 7;
		t->vlc[i].code = i - 256;
	}
	for(;i < 288; i ++)
	{
		t->vlc[i].value = i;
		t->vlc[i].numbits = 8;
		t->vlc[i].code = 0xc0 + i - 280;

	}

}

void huffmanInitDISTable(huffman_table_t* t)
{
	int i;
	for(i = 0; i < 32; i ++)
	{
		t->vlc[i].value = i;
		t->vlc[i].numbits = 5;
		t->vlc[i].code = i;
	}
}

int huffmanNormlize(int* cls,int num,huffman_table_t* huffman)
{
	int i,j;
	int idx;
	int code;
	int blcount[16] = {0};
	int nextcode[16] = {0};
	for(i = 1; i < 16; i ++)
	{
		blcount[i] = 0;
		for(j = 0; j < num;j ++)
		{
			if(cls[j] == i)
			{
				blcount[i] ++;
			}
		}
	}
	code = 0;
	blcount[0] = 0;
	for(i = 1; i < 16; i ++)
	{
		code = (code + blcount[i - 1]) << 1;
		nextcode[i] = code;
	}
	huffman->num = num;
	memset(huffman->vlc,0,sizeof(huffman->vlc));
	for(i = 0; i < num;i ++)
	{
		idx = huffman->vlc[i].numbits = cls[i];
		if(idx)
		{
			huffman->vlc[i].value = i;
			huffman->vlc[i].code = nextcode[idx];
			nextcode[idx] ++;
		}
	}
	return 0;
}

huffman_item_t* huffmanRead(bitstream_t* bt,huffman_table_t* t)
{
	int i;
	int code;
	huffman_item_t* item = &t->vlc[0];
	for(i = 0; i < t->num; i ++,item ++)
	{
		if(!item->numbits)
		{
			continue;
		}
		code = bitstreamReadBits(bt,item->numbits,0);
		if(code == item->code)
		{
			bitstreamSkip(bt,item->numbits);
			return item;
		}
	}
	return (huffman_item_t*)0;
}
