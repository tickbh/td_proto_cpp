#td_proto_cpp

tickbh cpp bin protocol

## suport type
base type is contain "u8",   "i8",   "u16",   "i16",   "u32",   "i32",   "float",   "string",   "raw",   "map"

array type is contain "u8[]", "i8[]", "u16[]", "i16[]", "u32[]", "i32[]", "float[]", "string[]", "raw[]", "map[]"

# data detail
data will be format like as Id, Type, Data store by little endian, Id is 2bytes, Type is 2bytes
 - "u8",   "i8"                   -- 1bytes 
 - "u16",   "i16"                 -- 2bytes
 - "u32",  "i32",  "float"        -- 4bytes, float decode with i32 div 1000
 - "string",  "raw"               -- 2bytes len, len bytes datas
 - map                            -- key always encode string, contains id, type, value is base value, end will key type is nil
 - array                          -- write base data, stop with id = 0, type = 0

# example data u8
```cpp

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
```
>the bytes is  
>[0, 0, 1, 0, 1] -- [0, 0] is id = 0, [1, 0] is type = 1 is TYPE_U8, [1] is data is 1u8

# example proto
```cpp

void test_base_proto() {
	std::map<std::string, td_proto::Field> fields = {
		{ "name", td_proto::Field(1, "string") },
		{ "index", td_proto::Field(2, "u16") },
	};
	std::map<std::string, td_proto::Proto> protos = {
		{ "cmd_test_op", td_proto::Proto("server", { "map" }) },
	};
	auto config = td_proto::Config(fields, protos);
	auto buffer = td_proto::Buffer();

	std::map<std::string, td_proto::Values>* hash_value = new std::map<std::string, td_proto::Values>();
	hash_value->insert(std::make_pair("name", td_proto::Values(new std::string("I'm a chinese people"))));
	hash_value->insert(std::make_pair("sub_name", td_proto::Values(new std::string("tickdream"))));
	hash_value->insert(std::make_pair("index", td_proto::Values((u16)1)));

	auto value_map = td_proto::Values(hash_value);
	std::vector<td_proto::Values> array;
	array.push_back(std::move(value_map));
	td_proto::encode_proto(buffer, config, "cmd_test_op", array);

	auto ret = td_proto::decode_proto(buffer, config);
	assert(buffer.isVaild());
	assert(std::get<0>(ret) == "cmd_test_op");
	auto& val = std::get<1>(ret);
	assert(val.size() == 1);
	assert(val[0]._map->size() == 2);
	assert(strcmp(val[0]._map->at("name")._str->c_str(), hash_value->at("name")._str->c_str()) == 0);
	assert(val[0]._map->at("index")._u16 == hash_value->at("index")._u16);
}
```

# compatible
it will ensure data decoded maximum
 - old protocol can decode the new protocol if new protocol not change the old field info, but it will miss some info
 - new protocol can decode the old protocol all datas
