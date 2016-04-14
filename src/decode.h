#ifndef _TD_PROTO_DECODE_H_
#define _TD_PROTO_DECODE_H_

#include "Values.h"

namespace td_proto {

#define CHECK_RETURN_BUFFER_VAILD(value) if (!buffer.isVaild()) { \
	return value; \
}

#define CHECK_BREAK_BUFFER_VAILD() if (!buffer.isVaild()) { \
	break; \
}

	Values decode_field(ByteBuffer& buffer, Config& config);

	Values decode_number(ByteBuffer& buffer, u16 pattern) {
		switch (pattern)
		{
		case TYPE_U8:
			return Values(buffer.read<u8>());
		case TYPE_I8:
			return Values(buffer.read<i8>());
		case TYPE_U16:
			return Values(buffer.read<u16>());
		case TYPE_I16:
			return Values(buffer.read<i16>());
		case TYPE_U32:
			return Values(buffer.read<u32>());
		case TYPE_I32:
			return Values(buffer.read<i32>());
		case TYPE_FLOAT: {
			float value = (float)(buffer.read<i32>() / 1000.0);
			return Values(value);
		}
		default:
			std::terminate();
			break;
		}
		return Values();
	}
	
	Field read_field(ByteBuffer& buffer) {
		auto index = decode_number(buffer, TYPE_U16)._u16;
		auto pattern = decode_number(buffer, TYPE_U16)._u16;
		return Field(index, get_name_by_type(pattern));
	}

	Values decode_str_raw(ByteBuffer& buffer, u16 pattern) {
		switch (pattern)
		{
		case TYPE_STR: {
			u16 len = decode_number(buffer, TYPE_U16)._u16;
			if( len == 0 ) {
				return Values(new std::string());
			}
			
			u8* value = new u8[len];
			buffer.read(value, len);
			std::string* str = new std::string((const char*)value, len);
			delete[] value;
			return Values(str);
		}
		case TYPE_RAW: {
			u16 len = decode_number(buffer, TYPE_U16)._u16;
			if (len == 0) {
				return Values(new std::vector<char>());
			}

			auto vecs = new std::vector<char>(len);
			u8* value = new u8[len];
			buffer.read(value, len);
			memcpy(&vecs[0], value, len);
			return Values(vecs);
		}
		default:
			std::terminate();
			break;
		}
		return Values();
	}

	Values decode_map(ByteBuffer& buffer, Config& config) {
		auto map = new std::map<std::string, Values>();
		while (true) {
			auto field = read_field(buffer);
			if (field.is_nil_type()) {
				return Values(map);
			}
			CHECK_RETURN_BUFFER_VAILD(Values(map));
			auto sub_value = decode_field(buffer, config);
			auto name = config.get_field_index_name(field.index);
			if ( name.size() == 0 ) {
				continue;
			}
			map->insert(std::make_pair(name, sub_value));
		}
		return Values();
	}

	Values decode_arrays(ByteBuffer& buffer, Config& config, u16 pattern, u16 sub_type) {
		auto value = new std::vector<Values>();
		while (true) {
			auto sub_value = decode_field(buffer, config);
			if (sub_value.sub_type == TYPE_NIL) {
				break;
			}
			if (sub_value.sub_type != sub_type) {
				buffer.setVaild(false);
				break;
			}
			CHECK_BREAK_BUFFER_VAILD();
			value->push_back(std::move(sub_value));
		}
		return Values(value, (u8)pattern);
	}

	Values decode_by_field(ByteBuffer& buffer, Config& config, Field& field) {
		auto t = get_type_by_name(field.pattern.c_str());
		switch (t)
		{
		case TYPE_U8:
		case TYPE_I8:
		case TYPE_U16:
		case TYPE_I16:
		case TYPE_U32:
		case TYPE_I32:
		case TYPE_FLOAT:
			return decode_number(buffer, t);
		case TYPE_STR:
		case TYPE_RAW:
			return decode_str_raw(buffer, t);
		case TYPE_MAP:
			return decode_map(buffer, config);
		case TYPE_AU8:
		case TYPE_AI8:
		case TYPE_AU16:
		case TYPE_AI16:
		case TYPE_AU32:
		case TYPE_AI32:
		case TYPE_AFLOAT:
		case TYPE_ASTR:
		case TYPE_ARAW:
		case TYPE_AMAP:
			return decode_arrays(buffer, config, t, t + TYPE_U8 - TYPE_AU8);
		default:
			std::terminate();
		}
	}

	Values decode_field(ByteBuffer& buffer, Config& config) {
		auto field = read_field(buffer);
		if (field.is_nil_type()) {
			return Values();
		}
		return decode_by_field(buffer, config, field);
	}

	std::tuple<std::string, std::vector<Values>> decode_proto(ByteBuffer& buffer, Config& config) {
		auto name_value = decode_str_raw(buffer, TYPE_STR);
		std::vector<Values> value;
		while (true)
		{
			auto sub_value = decode_field(buffer, config);
			if (sub_value.sub_type == TYPE_NIL) {
				break;
			}
			CHECK_BREAK_BUFFER_VAILD();
			value.push_back(std::move(sub_value));
		}
		auto result = std::make_pair(*name_value._str, std::move(value));
		auto proto = config.get_proto_by_name(*name_value._str);
		if (proto == NULL) {
			buffer.setVaild(false);
			return std::move(result);
		}
		if (proto->args.size() != value.size()) {
			buffer.setVaild(false);
			return std::move(result);
		}
		return std::move(result);

	}

}
#endif