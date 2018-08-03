#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
    #define DLLExport __declspec(dllexport)
#else
    #define DLLExport 
#endif

typedef unsigned char uint8_t;

#ifdef WIN32
	typedef void(_stdcall *LogCallBack)(char* logText);
	LogCallBack UnityDebugLog;
#endif

#define LUA_BLIB_VERSION "1.1"

#define BLIB_MAX(a,b) a>b?a:b

#define W_HEAD(buf) (buf)->buf + (buf)->write_index
#define R_HEAD(buf) (buf)->buf + (buf)->read_index

#define HEX_LOG(t, d, dl) \
printf("[read]: %s, %dbyte  =====>   %s\n", t, dl, d);  \
for (int i = 0; i < dl; i++)  \
{ \
  printf(">%d\t%#02x\n", *(d + i), *(d + i)); \
} \

typedef struct byte_buffer_t
{
	int id;
	int init_size;
	int read_index;
	int write_index;
	uint8_t * buf;
} byte_buffer_struct;

byte_buffer_struct * g_buf;
byte_buffer_struct* l_buf;

DLLExport void blib_new(lua_State *L, int init_size, int buf_id);

DLLExport void blib_free(lua_State *L, int buf_id);

DLLExport void blib_reset(lua_State *L, int buf_id);

DLLExport int blib_set_channel(lua_State *L, byte_buffer_struct * buf, int buf_id);

DLLExport byte_buffer_struct* blib_get_channel(lua_State *L, int buf_id);

DLLExport int blib_get_channel_rindex(lua_State *L, int buf_id);

DLLExport int blib_get_channel_windex(lua_State *L, int buf_id);

DLLExport void blib_write(byte_buffer_struct* buf, char* data, int data_length, char* title);

void blib_realloc(byte_buffer_struct * buf, int buf_id, int re_size);

DLLExport void blib_write_int(byte_buffer_struct * buf, int value);

DLLExport void blib_write_string(byte_buffer_struct * buf, char* value);

DLLExport void blib_write_short(byte_buffer_struct * buf, short value);

DLLExport void blib_write_float(byte_buffer_struct * buf, float value);

DLLExport void blib_write_double(byte_buffer_struct * buf, double value);

DLLExport void blib_read(byte_buffer_struct * buf, char* data, int data_length, char* title);

DLLExport int blib_read_int(byte_buffer_struct * buf);

DLLExport char* blib_read_string(byte_buffer_struct * buf);

DLLExport short blib_read_short(byte_buffer_struct * buf);

DLLExport double blib_read_float(byte_buffer_struct * buf);

DLLExport double blib_read_double(byte_buffer_struct * buf);

