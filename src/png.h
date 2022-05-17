#ifndef __PNG_H__
#define __PNG_H__

#ifdef __cplusplus
extern "C"
{
#endif

	int pngDestroy(void* hdl);
	int pngDecode(void* hdl,libphoto_output_t* attr);
	void* pngCreate(char* filename);

#ifdef __cplusplus
}
#endif




#endif
