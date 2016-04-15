#include "Values.h"
#include "Config.h"
#include "Buffer.h"
#include "Decodes.h"
#include "Encodes.h"
#include <iostream>
#include <string>

int test(td_proto::Values value){
	std::cout << "xxxxxxxxxxx" << std::endl;
	return 0;
}

void test_head_field(td_proto::Buffer& buffer, u16 index, u16 t) {
	u8 data[2] = { 0, 0 };
	buffer.read(data, 2);
	assert(buffer.isVaild());
	auto val = td_proto::ByteGetValue<u16>((char *)data);
	assert(val == index);

	buffer.read(data, 2);
	assert(buffer.isVaild());
	auto val_t = td_proto::ByteGetValue<u16>((char *)data);
	assert(val_t == t);

}

void test_enocde_u8() {
	auto config = td_proto::Config();
	auto buffer = td_proto::Buffer();
	auto value = td_proto::Values((u8)1);
	td_proto::encode_field(buffer, config, value);
	td_proto::encode_field(buffer, config, value);

	test_head_field(buffer, 0, td_proto::TYPE_U8);
	u8 data[1] = { 0 };
	buffer.read(data, 1);
	assert(data[0] == 1);

	auto ret = td_proto::decode_field(buffer, config);
	assert(buffer.isVaild() && ret.sub_type == td_proto::TYPE_U8);
	assert(ret._u8 == 1);
}

void test_encode_u16() {
	auto config = td_proto::Config();
	auto buffer = td_proto::Buffer();
	auto value = td_proto::Values((u16)0x1234);
	td_proto::encode_field(buffer, config, value);
	td_proto::encode_field(buffer, config, value);
	test_head_field(buffer, 0, td_proto::TYPE_U16);
	u8 data[2] = { 0, 0 };
	buffer.read(data, 2);
	assert(buffer.isVaild());
	assert(data[0] == 0x34);
	assert(data[1] == 0x12);

	auto ret = td_proto::decode_field(buffer, config);
	assert(buffer.isVaild() && ret.sub_type == td_proto::TYPE_U16);
	assert(ret._u16 == 0x1234);
}

void test_encode_u32() {
	auto config = td_proto::Config();
	auto buffer = td_proto::Buffer();
	auto value = td_proto::Values((u32)0x12345678);
	td_proto::encode_field(buffer, config, value);
	td_proto::encode_field(buffer, config, value);
	test_head_field(buffer, 0, td_proto::TYPE_U32);
	u8 data[4] = { 0, 0, 0, 0 };
	buffer.read(data, 4);
	assert(buffer.isVaild());
	assert(data[0] == 0x78);
	assert(data[1] == 0x56);
	assert(data[2] == 0x34);
	assert(data[3] == 0x12);

	auto ret = td_proto::decode_field(buffer, config);
	assert(buffer.isVaild() && ret.sub_type == td_proto::TYPE_U32);
	assert(ret._u32 == 0x12345678);
}

void test_encode_float() {
	auto config = td_proto::Config();
	auto buffer = td_proto::Buffer();
	auto value = td_proto::Values(12345.123f);
	td_proto::encode_field(buffer, config, value);
	td_proto::encode_field(buffer, config, value);
	test_head_field(buffer, 0, td_proto::TYPE_FLOAT);
	u8 data[4] = { 0, 0, 0, 0 };
	buffer.read(data, 4);
	assert(buffer.isVaild());
	auto val = td_proto::ByteGetValue<u32>((char *)data);
	auto val_f = val / 1000.0;
	assert(std::fabs(val_f - 12345.123) < 0.001);

	auto ret = td_proto::decode_field(buffer, config);
	assert(buffer.isVaild() && ret.sub_type == td_proto::TYPE_FLOAT);
	assert(std::fabs(ret._f - 12345.123) < 0.001);
}

void test_encode_str() {
	auto config = td_proto::Config();
	auto buffer = td_proto::Buffer();
	const char* name = "I'm a chinese people";
	auto char_len = strlen(name);

	auto value = td_proto::Values(new std::string(name));
	td_proto::encode_field(buffer, config, value);
	td_proto::encode_field(buffer, config, value);
	test_head_field(buffer, 0, td_proto::TYPE_STR);

	auto len = td_proto::decode_number(buffer, td_proto::TYPE_U16);
	assert(len._u16 == char_len);
	u8* data = new u8[char_len + 1];
	buffer.read(data, char_len);
	data[char_len] = '\0';
	assert(strcmp(name, (char*)data) == 0);

	auto ret = td_proto::decode_field(buffer, config);
	assert(buffer.isVaild() && ret.sub_type == td_proto::TYPE_STR);
	assert(strcmp(name, ret._str->c_str()) == 0);
}

void test_encode_map() {
	std::map<std::string, td_proto::Field> fields = {
		{ "name", td_proto::Field(1, "string") },
		{ "index", td_proto::Field(2, "u16") },
	};
	std::map<std::string, td_proto::Proto> protos;
	auto config = td_proto::Config(fields, protos);
	auto buffer = td_proto::Buffer();
	std::map<std::string, td_proto::Values> hash_value;
	hash_value.insert(std::make_pair("name",td_proto::Values(new std::string("I'm a chinese people"))));
	hash_value.insert(std::make_pair("sub_name", td_proto::Values(new std::string("tickdream"))));
	hash_value.insert(std::make_pair("index", td_proto::Values((u16)1)));

	auto value_map = td_proto::Values(&hash_value);
	td_proto::encode_field(buffer, config, value_map);

	auto ret = td_proto::decode_field(buffer, config);
	assert(buffer.isVaild() && ret.sub_type == td_proto::TYPE_MAP);
	assert(ret._map->size() == 2);
	assert(strcmp(ret._map->at("name")._str->c_str(), hash_value.at("name")._str->c_str()) == 0);
	assert(ret._map->at("index")._u16 == hash_value.at("index")._u16);
	value_map.unfree();
}


//#[test]
//fn test_encode_map() {
//	let config = td_rp::Config::new(" { \"name\" : { \"index\" :    1, \"pattern\" : \"string\" }, \
//																		                                        \"index\" : { \"index\" :    2, \"pattern\" : \"u16\" },  \
//																																															                                        \"sub_name\" : { \"index\" :    3, \"pattern\" :\"string\" }   }",
//																																																									"{\"cmd_test_op\"        : { \"msg_type\" :    \"server\", \"args\" : [ \"map\" ] }}");
//	let config = config.unwrap();
//	let mut hash_value = HashMap::<String, Value>::new();
//	hash_value.insert("name".to_string(), Value::Str("I'm a chinese people".to_string()));
//	hash_value.insert("sub_name".to_string(), Value::Str("tickdream".to_string()));
//	hash_value.insert("index".to_string(), Value::U16(1 as u16));
//	{
//		let mut buffer = Buffer::new();
//		td_rp::encode_field(&mut buffer, &config, &Value::Map(hash_value.clone())).unwrap();
//
//		// just read field
//		let read = td_rp::decode_field(&mut buffer, &config).unwrap();
//		match read{
//			Value::Map(val) = > assert_eq!(val, hash_value),
//			_ = > unreachable!("it will not read"),
//		}
//	}
//
//	let mut undefine = hash_value.clone();
//	undefine.insert("undefine".to_string(), Value::U16(1 as u16));
//	{
//		let mut buffer = Buffer::new();
//		td_rp::encode_field(&mut buffer, &config, &Value::Map(undefine.clone())).unwrap();
//
//		// just read field
//		let read = td_rp::decode_field(&mut buffer, &config).unwrap();
//		match read{
//			Value::Map(val) = > assert_eq!(val, hash_value),
//			_ = > unreachable!("it will not read"),
//		}
//	}
//}
//
//#[test]
//fn test_encode_array_u8() {
//	let config = Config::new_empty();
//	let mut array : Vec<Value> = vec![];
//	for i in 0 .. 10 {
//		array.push(Value::U8(i as u8));
//	}
//
//	let mut buffer = Buffer::new();
//	td_rp::encode_field(&mut buffer, &config, &Value::AU8(array.clone())).unwrap();
//
//	let read = td_rp::decode_field(&mut buffer, &config).unwrap();
//	match read{
//		Value::AU8(ref val) = > {
//			assert_eq!(*val, array);
//		},
//			_ = > unreachable!("it will not read"),
//	}
//}
//
//
//#[test]
//fn test_base_proto() {
//	let config = td_rp::Config::new(" { \"name\" : { \"index\" :    1, \"pattern\" : \"string\" }, \
//																		                                        \"index\" : { \"index\" :    2, \"pattern\" : \"u16\" },  \
//																																															                                        \"sub_name\" : { \"index\" :    3, \"pattern\" :\"string\" }   }",
//																																																									"{\"cmd_test_op\"        : { \"msg_type\" :    \"server\", \"args\" : [ \"map\" ] }}");
//	let config = config.unwrap();
//	let mut hash_value = HashMap::<String, Value>::new();
//	hash_value.insert("name".to_string(), Value::Str("I'm a chinese people".to_string()));
//	hash_value.insert("sub_name".to_string(), Value::Str("tickdream".to_string()));
//	hash_value.insert("index".to_string(), Value::U16(1 as u16));
//
//	{
//		let mut buffer = Buffer::new();
//		td_rp::encode_proto(&mut buffer, &config, &"cmd_test_op".to_string(), vec![Value::Map(hash_value.clone())]).unwrap();
//
//		// just read field
//		let read = td_rp::decode_proto(&mut buffer, &config).unwrap();
//		match read{
//			(name, val) = > {
//				assert_eq!(name, "cmd_test_op".to_string());
//				assert_eq!(val[0], Value::Map(hash_value));
//				assert_eq!(val.len(), 1);
//			}
//		}
//	}
//}

int main(int argc, char *argv[]) {
	float xx = 1.0;
	auto value = td_proto::Values(xx);
	{
		td_proto::Values strValue = td_proto::Values(new std::string("fkkk"));
		td_proto::Values strValue1 = td_proto::Values(std::move(strValue));
		//test(std::move(strValue1));
		//test(strValue1);


		//std::cout << "aaaa" << std::endl;
		//Values v = std::move(strValue1);
		td_proto::Values v = strValue1;
	}
	std::cout << "....." << std::endl;

	test_enocde_u8();
	test_encode_u16();
	test_encode_u32();
	test_encode_float();
	test_encode_str();
	test_encode_map();
}