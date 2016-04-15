#ifndef _TD_PROTO_CONFIG_H_
#define _TD_PROTO_CONFIG_H_

#include "TDMacro.h"
#include <string>
#include <vector>
#include <map>

namespace td_proto {
	class Field {
	public:
		u16 index;
		std::string pattern;
		Field() : index(TYPE_NIL), pattern("") {

		}
		Field(u16 index, std::string pattern)
			: index(index), pattern(pattern) {

		}

		Field(const Field& other) {
			this->index = other.index;
			this->pattern = other.pattern;
		}

		Field new_nil() {
			return Field(0, "nil");
		}

		Field new_pattern(std::string pattern){
			return Field(0, pattern);
		}

		bool is_nil_type() {
			return this->index == 0 && "nil" == this->pattern;
		}
	};

	class Proto {
	public:
		std::string msg_type;
		std::vector<std::string> args;

		Proto(std::string msg_type, std::vector<std::string> args)
			: msg_type(msg_type), args(args) {

		}

		Proto(const Proto& other) {
			this->msg_type = msg_type;
			this->args = args;
		}

	};

	class Config
	{
	public:
		std::map<std::string, Field*> field;
		std::map<std::string, Proto*> proto;
		std::map<u16, std::string> index_field;
		std::map<std::string, std::string> msg_proto;

		Config() {

		}

		Config(std::map<std::string, Field> field, std::map<std::string, Proto> proto) {
			for (auto iter : field) {
				this->field.insert(std::make_pair(iter.first, new Field(iter.second)));
				this->index_field.insert(std::make_pair(iter.second.index, iter.first));
			}
			for (auto iter : proto) {
				this->proto.insert(std::make_pair(iter.first, new Proto(iter.second)));
				this->msg_proto.insert(std::make_pair(iter.first, iter.second.msg_type));
			}
		}

		Field* get_field_by_name(std::string& name) {
			auto iter = this->field.find(name);
			if (iter == this->field.end()) {
				return nullptr;
			}
			return iter->second;
		}

		Field* get_field_by_index(u16& index) {
			auto name = this->get_field_index_name(index);
			return get_field_by_name(name);
		}

		std::string get_field_index_name(u16& index) {
			auto iter = this->index_field.find(index);
			if (iter == this->index_field.end()) {
				return "";
			}
			return iter->second;
		}

		std::string get_proto_msg_type(std::string& name) {
			auto iter = this->msg_proto.find(name);
			if (iter == this->msg_proto.end()) {
				return "";
			}
			return iter->second;
		}

		Proto* get_proto_by_name(std::string& name) {
			auto iter = this->proto.find(name);
			if (iter == this->proto.end()) {
				return nullptr;
			}
			return iter->second;
		}
	};

}
#endif