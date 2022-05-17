#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

typedef struct
{
	int numbits;
	int value;
	int code;
}huffman_item_t;


typedef struct
{
	int num;
	huffman_item_t vlc[512];
}huffman_table_t;

#ifdef __cplusplus
extern "C"
{
#endif

	void huffmanInitLITable(huffman_table_t* t);
	void huffmanInitDISTable(huffman_table_t* t);
	int huffmanNormlize(int* cls,int num,huffman_table_t* huffman);
	huffman_item_t* huffmanRead(bitstream_t* bt,huffman_table_t* t);

#ifdef __cplusplus
}
#endif

#endif
