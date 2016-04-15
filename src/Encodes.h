#ifndef _TD_PROTO_ENCODE_H_
#define _TD_PROTO_ENCODE_H_
#include "Buffer.h"
#include "Values.h"
namespace td_proto {

	bool encode_field(Buffer& buffer, Config& config, Values& value);

	bool encode_number(Buffer& buffer, Values& value) {
		switch (value.sub_type)
		{
		case TYPE_U8:
			buffer.write<u8>(value._u8);
			break;
		case TYPE_I8:
			buffer.write<i8>(value._i8);
			break;
		case TYPE_U16:
			buffer.write<u16>(value._u16);
			break;
		case TYPE_I16:
			buffer.write<i16>(value._i16);
			break;
		case TYPE_U32:
			buffer.write<u32>(value._u32);
			break;
		case TYPE_I32:
			buffer.write<i32>(value._i32);
			break;
		case TYPE_FLOAT: {
			auto v = (i32)(value._f * 1000);
			buffer.write<i32>(v);
		}
			break;
		default:
			std::terminate();
		}
		return true;
	}


	bool write_str_field(Buffer& buffer, const char* pattern) {
		encode_number(buffer, Values((u16)0));
		encode_number(buffer, Values((u16)get_type_by_name(pattern)));
		return true;
	}

	bool write_field(Buffer& buffer, Field* field) {
		if (field == nullptr) {
			return false;
		}
		encode_number(buffer, Values((u16)field->index));
		encode_number(buffer, Values((u16)get_type_by_name(field->pattern.c_str())));
		return true;
	}

	bool encode_str_raw(Buffer& buffer, Values& value) {
		switch (value.sub_type)
		{
		case TYPE_STR:
			encode_number(buffer, Values((u16)value._str->size()));
			buffer.append(value._str->c_str(), value._str->size());
			break;
		case TYPE_RAW:
			encode_number(buffer, Values((u16)value._raw->size()));
			buffer.append(&value._raw[0], value._raw->size());
			break;
		default:
			std::terminate();
		}
		return true;
	}

	bool encode_map(Buffer& buffer, Config& config, Values& value) {
		switch (value.sub_type) {
		case TYPE_MAP: {
			for (auto& iter : *value._map) {
				auto name = iter.first;
				if (write_field(buffer, config.get_field_by_name(name))) {
					encode_field(buffer, config, iter.second);
				}
			}
			write_str_field(buffer, STR_TYPE_NIL);
		}
			break;
		default:
			std::terminate();
		}
		return true;
	}

	bool encode_field(Buffer& buffer, Config& config, Values& value) {
		write_str_field(buffer, get_name_by_type(value.sub_type));
		switch (value.sub_type)
		{
		case TYPE_U8:
		case TYPE_I8:
		case TYPE_U16:
		case TYPE_I16:
		case TYPE_U32:
		case TYPE_I32:
		case TYPE_FLOAT:
			encode_number(buffer, value);
			break;
		case TYPE_STR:
		case TYPE_RAW:
			encode_str_raw(buffer, value);
			break;
		case TYPE_MAP:
			encode_map(buffer, config, value);
			break;
		case TYPE_AU8:
		case TYPE_AI8:
		case TYPE_AU16:
		case TYPE_AI16:
		case TYPE_AU32:
		case TYPE_AI32:
		case TYPE_AFLOAT:
		case TYPE_ASTR:
		case TYPE_ARAW:
		case TYPE_AMAP: {
			auto must_type = value.sub_type + TYPE_U8 - TYPE_AU8;
			auto arrays = value.get_array_value();
			if (arrays) {
				for (auto& iter : *arrays) {
					if (iter.sub_type != must_type) {
						buffer.setVaild(false);
						return false;
					}
					encode_field(buffer, config, iter);
				}
			}
			write_str_field(buffer, STR_TYPE_NIL);
		}

			break;
		default:
			break;
		}
		return true;
	}

	bool encode_field(Buffer& buffer, Config& config, std::string& name, std::vector<Values>& infos) {
		auto proto = config.get_proto_by_name(name);
		if (!proto) {
			return false;
		}
		if (proto->args.size() != infos.size()) {
			return false;
		}
		encode_str_raw(buffer, Values(new std::string(name)));
		for (auto& iter : infos)
		{
			encode_field(buffer, config, iter);
		}
		write_str_field(buffer, STR_TYPE_NIL);
		return buffer.isVaild();
	}

}

#endif