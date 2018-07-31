#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "blib.h"

#define LUA_BLIB_VERSION "1.1"

#define max(a,b) a>b?a:b

// static const void *key = 0;

DLLExport void blib_new(lua_State *L, int init_size, int buf_id)
{
	byte_buffer_struct* buf = (byte_buffer_struct*)malloc(sizeof(byte_buffer_struct));
	buf->id = buf_id;
	buf->init_size = init_size;
	buf->read_index = 0;
	buf->write_index = 0;
	buf->buf = (uint8_t*)calloc(init_size, sizeof(uint8_t));

	blib_set_channel(L, buf, buf_id);
}

DLLExport void blib_free(lua_State *L, int buf_id)
{
	byte_buffer_struct* buf = blib_get_channel(L, buf_id);
	if(buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
}

DLLExport void blib_reset(lua_State *L, int buf_id)
{
	byte_buffer_struct* buf = blib_get_channel(L, buf_id);
	if(buf != NULL)
	{
		buf->read_index = 0;
		buf->write_index = 0;
		memset(buf->buf, '*', strlen(buf->buf));
	}
}

DLLExport int blib_set_channel(lua_State *L, byte_buffer_struct * buf, int buf_id)
{
	/*lua_pushlightuserdata(L, (void *)&key);*/
	lua_pushinteger(L, buf_id);
	//save buf ptr
	lua_pushlightuserdata(L, buf);
	lua_settable(L, LUA_REGISTRYINDEX);
	return 0;
}

DLLExport byte_buffer_struct* blib_get_channel(lua_State *L, int buf_id)
{
	/*lua_pushlightuserdata(L, (void *)&key);*/
	lua_pushinteger(L, buf_id);
	lua_gettable(L, LUA_REGISTRYINDEX);
	byte_buffer_struct * buf = lua_touserdata(L, -1);
	if(buf == NULL)
	{
		UnityDebugLog("get_from_channel is NULL");
	}
	return buf;
}

DLLExport int blib_get_channel_rindex(lua_State *L, int buf_id)
{
	int r_index = 0;
	byte_buffer_struct* buf = blib_get_channel(L, buf_id);
	if(buf != NULL)
	{
		r_index = buf->read_index;
	}
	return r_index;
}

DLLExport int blib_get_channel_windex(lua_State *L, int buf_id)
{
	int w_index = 0;
	byte_buffer_struct* buf = blib_get_channel(L, buf_id);
	if(buf != NULL)
	{
		w_index = buf->write_index;
	}
	return w_index;
}

DLLExport void blib_write_int(byte_buffer_struct * buf, int value)
{
	int data_length = sizeof(int);
	blib_write(buf, &value, data_length, "int");
}

DLLExport void blib_write_string(byte_buffer_struct * buf, char* value)
{
	int str_len = strlen(value);
	str_len += 1;// '\0'
	blib_write_int(buf, str_len);
	blib_write(buf, value, str_len, "string");
}

DLLExport void blib_write_short(byte_buffer_struct * buf, short value)
{
	int data_length = sizeof(short);
	blib_write(buf, &value, data_length, "short");
}

DLLExport void blib_write_float(byte_buffer_struct * buf, float value)
{
	int data_length = sizeof(float);
	blib_write(buf, &value, data_length, "float");
}

DLLExport void blib_write_double(byte_buffer_struct * buf, double value)
{
	int data_length = sizeof(double);
	blib_write(buf, &value, data_length, "double");
}

DLLExport void blib_write_bool(byte_buffer_struct * buf, char bool_value)
{
	int data_length = 1;
	blib_write(buf, &bool_value, data_length, "bool");
}

void blib_realloc(byte_buffer_struct * buf, int re_size)
{
	if(buf != NULL)
	{
		UnityDebugLog("realloc~~~\n");
		char int_v[4];
		itoa(buf->init_size, int_v, 4);
		// memcpy(int_v, &buf->init_size, 4);
		UnityDebugLog(int_v);
		buf->init_size += re_size;

		char int_v1[4];
		itoa(buf->init_size, int_v1, 4);
		// memcpy(int_v1, &buf->init_size, 4);
		UnityDebugLog(int_v1);

		UnityDebugLog("realloc~~~ end\n");
		buf->buf = realloc(buf->buf, buf->init_size);
	}
}

DLLExport void blib_write(byte_buffer_struct* buf, char* data, int data_length, char* title)
{
	HEX_LOG(title, data, data_length)
	if(buf->init_size - buf->write_index < data_length)
	{
		blib_realloc(buf, buf->id, max(data_length, 1024));
	}
	memcpy(W_HEAD(buf), data, data_length);
	buf->write_index += data_length;
}

DLLExport int blib_read_int(byte_buffer_struct * buf)
{
	int data_length = sizeof(int);
	int value = 0;
	blib_read(buf, &value, data_length, "int");
	return value;
}

DLLExport char* blib_read_string(byte_buffer_struct * buf)
{
	int data_length = blib_read_int(buf);
	char* data = malloc(data_length);
	blib_read(buf, data, data_length, "string");
	return data;
}

DLLExport short blib_read_short(byte_buffer_struct * buf)
{
	int data_length = sizeof(short);
	short value = 0;
	blib_read(buf, &value, data_length, "short");
	return value;
}

DLLExport double blib_read_float(byte_buffer_struct * buf)
{
	int data_length = sizeof(float);
	float value = 0;
	blib_read(buf, &value, data_length, "float");
	return value;
}

DLLExport double blib_read_double(byte_buffer_struct * buf)
{
	int data_length = sizeof(double);
	double value = 0;
	blib_read(buf, &value, data_length, "double");
	return value;
}

DLLExport void blib_read(byte_buffer_struct * buf, char* data, int data_length, char* title)
{
	memcpy(data, R_HEAD(buf), data_length);
	buf->read_index += data_length;
	//HEX_LOG(title, data, data_length);
}

// 注册日志回调函数
DLLExport void set_display_log(LogCallBack DisplayLog)
{
	UnityDebugLog = DisplayLog;
}

//------------------------------------------ lua api --------------------------------------------
// static int new_buf(lua_State *L)
// {
// 	int init_size = lua_tointeger(L, 1);
// 	byte_buffer_struct *buf = (byte_buffer_struct*) lua_newuserdata(L, sizeof(byte_buffer_struct));
// 	buf->init_size = init_size;
// 	buf->read_index = 0;
// 	buf->write_index = 0;
// 	buf->buf = (uint8_t*)calloc(init_size, sizeof(uint8_t));
// 	return 1;
// }

static int set_channel(lua_State *L)
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib, int]
	int buf_id = (int)lua_tointeger(L, 2);
	blib_set_channel(L, buf, buf_id);
	return 0;
}

static int get_channel(lua_State *L)
{
	int buf_id = (int)lua_tointeger(L, 1);
	byte_buffer_struct *buf = blib_get_channel(L, buf_id);
	lua_pushlightuserdata(L, buf);
	return 1;
}

static int write_int(lua_State *L) 
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib, int]
	int value = lua_tointeger(L, 2); // [blib, int]
	blib_write_int(buf, value);
	return 0;
}

static int write_string(lua_State *L) 
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib, string]
	size_t len = 0;
	char* value = lua_tolstring(L, 2, &len);
	blib_write_string(buf, value);
	return 0;
}

static int write_short(lua_State *L) 
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib, int]
	short value = (short)lua_tointeger(L, 2); // [blib, short]
	blib_write_short(buf, value);
	return 0;
}

static int write_float(lua_State *L)
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib, int]
	float value = (float)lua_tonumber(L, 2); // [blib, float]
	blib_write_float(buf, value);
	return 0;
}

static int write_double(lua_State *L)
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib, int]
	double value = (double)lua_tonumber(L, 2); // [blib, double]
	blib_write_double(buf, value);
	return 0;
}

static int read_int(lua_State *L) 
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib]
	int value = blib_read_int(buf); // [blib]
	lua_pushinteger(L, value); // [blib, int]
	return 1;
}

static int read_string(lua_State *L) 
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib]
	const char* value = blib_read_string(buf); // [blib]
	lua_pushstring(L, value); // [blib, string]
	return 1;
}

static int read_short(lua_State *L) 
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib]
	short value = blib_read_short(buf); // [blib]
	lua_pushinteger(L, value); // [blib, short]
	return 1;
}

static int read_float(lua_State *L)
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib]
	float value = blib_read_float(buf); // [blib]
	lua_pushnumber(L, value); // [blib, float]
	return 1;
}

static int read_double(lua_State *L)
{
	byte_buffer_struct *buf = (byte_buffer_struct *)lua_touserdata(L, 1); // [blib]
	double value = blib_read_double(buf); // [blib]
	lua_pushnumber(L, value); // [blib, double]
	return 1;
}

static const struct luaL_Reg methods[] = {
	// {"new_buf", new_buf },
	{"set_channel", set_channel },
	{"get_channel", get_channel },
	{"write_int", write_int},
	{"write_string", write_string},
	{"write_short", write_short},
	{"write_float", write_float},
	{"write_double", write_double },
	{"read_int", read_int},
	{"read_string", read_string},
	{"read_short", read_short},
	{"read_float", read_float},
	{"read_double", read_double},
	{NULL, NULL},
};

DLLExport int luaopen_blib(lua_State * L)
{
	luaL_newlib(L, methods); // [blib]
	lua_setglobal(L, "blib");// []
	return 1;
}

