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

	static std::map<const char*, u16> HASH_STR_INT = {
		{ STR_TYPE_U8, TYPE_U8 },
		{ STR_TYPE_I8, TYPE_I8 },
		{ STR_TYPE_U16, TYPE_U16 },
		{ STR_TYPE_I16, TYPE_I16 },
		{ STR_TYPE_U32, TYPE_U32 },
		{ STR_TYPE_I32, TYPE_I32 },
		{ STR_TYPE_FLOAT, TYPE_FLOAT },
		{ STR_TYPE_STR, TYPE_STR },
		{ STR_TYPE_RAW, TYPE_RAW },
		{ STR_TYPE_MAP, TYPE_MAP },
		{ STR_TYPE_AU8, TYPE_AU8 },
		{ STR_TYPE_AI8, TYPE_AI8 },
		{ STR_TYPE_AU16, TYPE_AU16 },
		{ STR_TYPE_AI16, TYPE_AI16 },
		{ STR_TYPE_AU32, TYPE_AU32 },
		{ STR_TYPE_AI32, TYPE_AI32 },
		{ STR_TYPE_AFLOAT, TYPE_AFLOAT },
		{ STR_TYPE_ASTR, TYPE_ASTR },
		{ STR_TYPE_ARAW, TYPE_ARAW },
		{ STR_TYPE_AMAP, TYPE_AMAP },
	};

	static std::map<u16, const char*> HASH_INT_STR;

	u16 get_type_by_name(const char* name) {
		auto iter = HASH_STR_INT.find(name);
		if (iter == HASH_STR_INT.end()) {
			return 0;
		}
		return iter->second;
	}

	const char* get_name_by_type(u16 index) {
		if (HASH_INT_STR.empty()) {
			for (auto iter : HASH_STR_INT)
			{
				HASH_INT_STR.insert(std::make_pair(iter.second, iter.first));
			}
		}
		auto iter = HASH_INT_STR.find(index);
		if (iter == HASH_INT_STR.end()) {
			return STR_TYPE_NIL;
		}
		return iter->second;
	}

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

		explicit Values() : sub_type(TYPE_NIL) {

		}
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
		~Values() {
			switch (this->sub_type)
			{
			case TYPE_STR:
				delete _str;
				break;
			case TYPE_RAW:
				delete _raw;
				break;
			case TYPE_MAP:
				delete _map;
				break;
			case TYPE_AU8:
				delete _au8;
				break;
			case TYPE_AI8:
				delete _ai8;
				break;
			case TYPE_AU16:
				delete _au16;
				break;
			case TYPE_AU32:
				delete _au32;
				break;
			case TYPE_AI32:
				delete _ai32;
				break;
			case TYPE_AFLOAT:
				delete _af;
				break;
			case TYPE_ASTR:
				delete _astr;
				break;
			case TYPE_ARAW:
				delete _araw;
				break;
			case TYPE_AMAP:
				delete _amap;
				break;
			default:
				break;
			}
		}

		void move(Values& other)
		{
			this->sub_type = other.sub_type;
			switch (other.sub_type)
			{
			case TYPE_U8:
				this->_u8 = other._u8;
				break;
			case TYPE_I8:
				this->_i8 = other._i8;
				break;
			case TYPE_U16:
				this->_u16 = other._u16;
				break;
			case TYPE_I16:
				this->_i16 = other._i16;
				break;
			case TYPE_U32:
				this->_u32 = other._u32;
				break;
			case TYPE_I32:
				this->_i32 = other._i32;
				break;
			case TYPE_FLOAT:
				this->_f = other._f;
				break;
			case TYPE_STR:
				this->_str = other._str;
				break;
			case TYPE_RAW:
				this->_raw = other._raw;
				break;
			case TYPE_MAP:
				this->_map = other._map;
				break;
			case TYPE_AU8:
				this->_au8 = other._au8;
				break;
			case TYPE_AI8:
				this->_ai8 = other._ai8;
				break;
			case TYPE_AU16:
				this->_au16 = other._au16;
				break;
			case TYPE_AI16:
				this->_ai16 = other._ai16;
				break;
			case TYPE_AU32:
				this->_au32 = other._au32;
				break;
			case TYPE_AI32:
				this->_ai32 = other._ai32;
				break;
			case TYPE_AFLOAT:
				this->_af = other._af;
				break;
			case TYPE_ASTR:
				this->_astr = other._astr;
				break;
			case TYPE_ARAW:
				this->_araw = other._araw;
				break;
			case TYPE_AMAP:
				this->_amap = other._amap;
				break;
			default:
				break;
			}
			other.sub_type = TYPE_NIL;
		}

		Values(Values& other) {
			move(other);
		}
		Values(Values&& other)
		{
			move(other);
		}
		Values& operator= (Values& other) {
			move(other);
		}
		Values& operator= (Values&& other) {
			move(other);
		}

		std::vector<Values>* get_array_value() {
			switch (this->sub_type)
			{
			case TYPE_AU8:
				return this->_au8;
				break;
			case TYPE_AI8:
				return this->_ai8;
				break;
			case TYPE_AU16:
				return this->_au16;
				break;
			case TYPE_AI16:
				return this->_ai16;
				break;
			case TYPE_AU32:
				return this->_au32;
				break;
			case TYPE_AI32:
				return this->_ai32;
				break;
			case TYPE_AFLOAT:
				return this->_af;
				break;
			case TYPE_ASTR:
				return this->_astr;
				break;
			case TYPE_ARAW:
				return this->_araw;
				break;
			case TYPE_AMAP:
				return this->_amap;
				break;
			default:
				break;
			}
			return nullptr;
		}
	};

}
#endif