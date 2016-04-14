#ifndef _TD_PROTO_VALUES_H_
#define _TD_PROTO_VALUES_H_
#include "TDMacro.h"
#include <string>
#include <vector>
#include <Map>

namespace td_proto {

const u16 TYPE_NIL = 0;
const u16 TYPE_U8 = 1;
const u16 TYPE_I8 = 2;
const u16 TYPE_U16 = 3;
const u16 TYPE_I16 = 4;
const u16 TYPE_U32 = 5;
const u16 TYPE_I32 = 6;
const u16 TYPE_FLOAT = 7;
const u16 TYPE_STR = 8;
const u16 TYPE_RAW = 9;
const u16 TYPE_MAP = 10;
const u16 TYPE_AU8 = 21;
const u16 TYPE_AI8 = 22;
const u16 TYPE_AU16 = 23;
const u16 TYPE_AI16 = 24;
const u16 TYPE_AU32 = 25;
const u16 TYPE_AI32 = 26;
const u16 TYPE_AFLOAT = 27;
const u16 TYPE_ASTR = 28;
const u16 TYPE_ARAW = 29;
const u16 TYPE_AMAP = 30;

const char* STR_TYPE_NIL = "nil";
const char* STR_TYPE_U8 = "u8";
const char* STR_TYPE_I8 = "i8";
const char* STR_TYPE_U16 = "u16";
const char* STR_TYPE_I16 = "i16";
const char* STR_TYPE_U32 = "u32";
const char* STR_TYPE_I32 = "i32";
const char* STR_TYPE_FLOAT = "float";
const char* STR_TYPE_STR = "str";
const char* STR_TYPE_RAW = "raw";
const char* STR_TYPE_MAP = "map";
const char* STR_TYPE_AU8 = "u8[]";
const char* STR_TYPE_AI8 = "i8[]";
const char* STR_TYPE_AU16 = "u16[]";
const char* STR_TYPE_AI16 = "i16[]";
const char* STR_TYPE_AU32 = "u32[]";
const char* STR_TYPE_AI32 = "i32[]";
const char* STR_TYPE_AFLOAT = "float[]";
const char* STR_TYPE_ASTR = "str[]";
const char* STR_TYPE_ARAW = "raw[]";
const char* STR_TYPE_AMAP = "map[]";

class Values
{
public:
	u8 sub_type;
	union {
		u8 _u8;
		i8 _i8;
		u16 _u16;
		i16 _i16;
		u32 _u32;
		i32 _i32;
		float _f;
		std::string* _str;
		std::vector<char>* _raw;
		std::map<std::string, Values>* _map;
		std::vector<Values>* _au8;
		std::vector<Values>* _ai8;
		std::vector<Values>* _au16;
		std::vector<Values>* _ai16;
		std::vector<Values>* _au32;
		std::vector<Values>* _ai32;
		std::vector<Values>* _af;
		std::vector<Values>* _astr;
		std::vector<Values>* _araw;
		std::vector<Values>* _amap;
	};

	explicit Values(u8 _u8) : sub_type(TYPE_U8), _u8(_u8) {
	}
	explicit Values(i8 _i8) : sub_type(TYPE_I8), _i8(_i8) {
	}
	explicit Values(u16 _u16) : sub_type(TYPE_U16), _u16(_u16) {
	}
	explicit Values(i16 _i16) : sub_type(TYPE_I16), _i16(_i16) {
	}
	explicit Values(u32 _u32) : sub_type(TYPE_U32), _u32(_u32) {
	}
	explicit Values(i32 _i32) : sub_type(TYPE_I32), _i32(_i32) {
	}
	explicit Values(float _f) : sub_type(TYPE_FLOAT), _f(_f) {
	}
	explicit Values(std::string* _str) : sub_type(TYPE_STR), _str(_str) {
	}
	explicit Values(std::vector<char>* _raw) : sub_type(TYPE_RAW), _raw(_raw) {
	}
	explicit Values(std::map<std::string, Values>* _map) : sub_type(TYPE_MAP), _map(_map) {
	}
	explicit Values(std::vector<Values>* arrays, u8 sub_type) : sub_type(sub_type) {
		switch (sub_type)
		{
		case TYPE_AU8:
			_au8 = arrays;
			break;
		case TYPE_AI8:
			_ai8 = arrays;
			break;
		case TYPE_AU16:
			_au16 = arrays;
			break;
		case TYPE_AI16:
			_ai16 = arrays;
			break;
		case TYPE_AU32:
			_au32 = arrays;
			break;
		case TYPE_AI32:
			_ai32 = arrays;
			break;
		case TYPE_AFLOAT:
			_af = arrays;
			break;
		case TYPE_ASTR:
			_astr = arrays;
			break;
		case TYPE_ARAW:
			_araw = arrays;
			break;
		case TYPE_AMAP:
			_amap = arrays;
			break;
		default:
			sub_type = TYPE_NIL;
			break;
		}
	}

	//explicit Values(std::vector<Values>* _au8) : sub_type(TYPE_AU8), _au8(_au8) {
	//}
	//explicit Values(std::vector<Values>* _ai8) : sub_type(TYPE_AI8), _ai8(_ai8) {
	//}
	//explicit Values(std::vector<Values>* _au16) : sub_type(TYPE_AU16), _au16(_au16) {
	//}
	//explicit Values(std::vector<Values>* _ai16) : sub_type(TYPE_AI16), _ai16(_ai16) {
	//}
	//explicit Values(std::vector<Values>* _au32) : sub_type(TYPE_AU32), _au32(_au32) {
	//}
	//explicit Values(std::vector<Values>* _ai32) : sub_type(TYPE_AI32), _ai32(_ai32) {
	//}
	//explicit Values(std::vector<Values>* _af) : sub_type(TYPE_AFLOAT), _af(_af) {
	//}
	//explicit Values(std::vector<Values>* _astr) : sub_type(TYPE_ASTR), _astr(_astr) {
	//}
	//explicit Values(std::vector<Values>* _araw) : sub_type(TYPE_ARAW), _araw(_araw) {
	//}
	//explicit Values(std::vector<Values>* _amap) : sub_type(TYPE_AMAP), _amap(_amap) {
	//}
	~Values() {
		switch (this->sub_type)
		{
		case TYPE_STR:
			delete _str;
			break;
		default:
			break;
		}
	}
};

}
#endif