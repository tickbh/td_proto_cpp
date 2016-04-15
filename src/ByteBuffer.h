#ifndef __NET_BYTEBUFFER_H__
#define __NET_BYTEBUFFER_H__

#include <assert.h>
#include <vector>
#include <list>
#include <map>
#include <string>

#define BL_LITTLEENDIAN 0
#define BL_BIGENDIAN    1

#if (('1234' >> 24) == '1')
#    define BL_ENDIAN BL_LITTLEENDIAN
#  elif (('4321' >> 24) == '1')
#    define BL_ENDIAN BL_BIGENDIAN
#endif

#include <algorithm>


namespace ByteConverter
{
	template<size_t T>
	inline void convert(char *val)
	{
		std::swap(*val, *(val + T - 1));
		convert<T - 2>(val + 1);
	}

	template<> inline void convert<0>(char *) {}
	template<> inline void convert<1>(char *) {}            // ignore central byte

	template<typename T> inline void apply(T *val)
	{
		convert<sizeof(T)>((char *)(val));
	}

#if BL_ENDIAN == BL_LITTLEENDIAN
	template<typename T> inline void EndianConvert(T& val) { ByteConverter::apply<T>(&val); }
#else
	template<typename T> inline void EndianConvert(T& val) {  }
#endif

	template<typename T> T getValue(char* value)
	{
		T val = *((T const*)value);
		EndianConvert(val);
		return val;
	}

	template<typename T> inline T ByteGetValue(char* value) { return ByteConverter::getValue<T>(value); }
}

using namespace ByteConverter;
class ByteBuffer
{
public:
	const static size_t DEFAULT_SIZE = 0x1000;

	ByteBuffer() : _rpos(0), _wpos(0), _vaild(true)
	{
		_storage.reserve(DEFAULT_SIZE);
	}

	ByteBuffer(size_t res) : _rpos(0), _wpos(0), _vaild(true)
	{
		_storage.reserve(res);
	}

	ByteBuffer(const ByteBuffer &buf) : _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage), _vaild(buf._vaild) { }

	void clear()
	{
		_storage.clear();
		_rpos = _wpos = 0;
		_vaild = true;
	}

	template <typename T> void write(T value)
	{
		EndianConvert(value);
		append((u8 *)&value, sizeof(value));
	}

	template <typename T> void append(T value)
	{
		EndianConvert(value);
		append((u8 *)&value, sizeof(value));
	}

	template <typename T> void put(size_t pos, T value)
	{
		EndianConvert(value);
		put(pos, (u8 *)&value, sizeof(value));
	}

	ByteBuffer &operator<<(u8 value)
	{
		append<u8>(value);
		return *this;
	}
	ByteBuffer &operator<<(u16 value)
	{
		append<u16>(value);
		return *this;
	}
	ByteBuffer &operator<<(u32 value)
	{
		append<u32>(value);
		return *this;
	}
	ByteBuffer &operator<<(u64 value)
	{
		append<u64>(value);
		return *this;
	}


	ByteBuffer &operator<<(i8 value)
	{
		append<i8>(value);
		return *this;
	}
	ByteBuffer &operator<<(i16 value)
	{
		append<i16>(value);
		return *this;
	}
	ByteBuffer &operator<<(i32 value)
	{
		append<i32>(value);
		return *this;
	}
	ByteBuffer &operator<<(i64 value)
	{
		append<i64>(value);
		return *this;
	}

	ByteBuffer &operator<<(float value)
	{
		append<float>(value);
		return *this;
	}
	ByteBuffer &operator<<(double value)
	{
		append<double>(value);
		return *this;
	}
	ByteBuffer &operator<<(const std::string &value)
	{
		append((u8 const *)value.c_str(), value.length());
		append((u8)0);
		return *this;
	}
	ByteBuffer &operator<<(const char *str)
	{
		append((u8 const *)str, str ? strlen(str) : 0);
		append((u8)0);
		return *this;
	}

	ByteBuffer &operator>>(bool &value)
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}

	ByteBuffer &operator>>(u8 &value)
	{
		value = read<u8>();
		return *this;
	}
	ByteBuffer &operator>>(u16 &value)
	{
		value = read<u16>();
		return *this;
	}
	ByteBuffer &operator>>(u32 &value)
	{
		value = read<u32>();
		return *this;
	}
	ByteBuffer &operator>>(u64 &value)
	{
		value = read<u64>();
		return *this;
	}

	ByteBuffer &operator>>(i8 &value)
	{
		value = read<i8>();
		return *this;
	}
	ByteBuffer &operator>>(i16 &value)
	{
		value = read<i16>();
		return *this;
	}
	ByteBuffer &operator>>(i32 &value)
	{
		value = read<i32>();
		return *this;
	}
	ByteBuffer &operator>>(i64 &value)
	{
		value = read<i64>();
		return *this;
	}

	ByteBuffer &operator>>(float &value)
	{
		value = read<float>();
		return *this;
	}
	ByteBuffer &operator>>(double &value)
	{
		value = read<double>();
		return *this;
	}
	ByteBuffer &operator>>(std::string& value)
	{
		value.clear();
		while (rpos() < size())
		{
			char c = read<char>();
			if (c == 0)
				break;
			value += c;
		}
		return *this;
	}

	u8 operator[](size_t pos)
	{
		return read<u8>(pos);
	}

	size_t rpos() const { return _rpos; }

	size_t rpos(size_t rpos_)
	{
		_rpos = rpos_;
		return _rpos;
	};

	size_t wpos() const { return _wpos; }

	size_t wpos(size_t wpos_)
	{
		_wpos = wpos_;
		if (this->size() < _wpos) {
			this->_storage.resize(_wpos);
		}
		return _wpos;
	}

	bool isVaild() { return _vaild; }
	void setVaild(bool vaild) { this->_vaild = vaild; }

	template <typename T> int readByte()
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	};

	template <typename T> T read()
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	};
	template <typename T> T read(size_t pos)
	{
		//assert(pos + sizeof(T) <= size() || PriPosError(false,pos,sizeof(T)));
		if (pos + sizeof(T) > size())
		{
			T val = 0;
			_vaild = false;
			return val;
		}

		T val = *((T const*)&_storage[pos]);
		EndianConvert(val);
		return val;
	}

	void read(u8 *dest, size_t len)
	{
		//assert(_rpos  + len  <= size() || PriPosError(false,_rpos,len));
		if (_rpos + len > size()) {
			_vaild = false;
			return;
		}

		memcpy(dest, &_storage[_rpos], len);
		_rpos += len;
	}

	const u8 *contents() const { return &_storage[0]; }

	size_t size() const { return _storage.size(); }
	bool empty() const { return _storage.empty(); }

	void resize(size_t newsize)
	{
		_storage.resize(newsize);
		_rpos = 0;
		_wpos = size();
	};
	void reserve(size_t ressize)
	{
		if (ressize > size()) _storage.reserve(ressize);
	};

	void append(const std::string& str)
	{
		append((u8 const*)str.c_str(), str.size() + 1);
	}
	void append(const char *src, size_t cnt)
	{
		return append((const u8 *)src, cnt);
	}
	template<class T>
	void append(const T *src, size_t cnt)
	{
		return append((const u8 *)src, cnt*sizeof(T));
	}
	void append(const u8 *src, size_t cnt)
	{
		if (!cnt) return;

		if (size() > 100000000) {
			return;
		}

		if (_storage.size() < _wpos + cnt)
			_storage.resize(_wpos + cnt);
		memcpy(&_storage[_wpos], src, cnt);
		_wpos += cnt;
	}
	void append(const ByteBuffer& buffer)
	{
		if (buffer.size()) append(buffer.contents(), buffer.size());
	}

	void appendPackGUID(u64 guid)
	{
		size_t mask_position = wpos();
		*this << u8(0);
		for (u8 i = 0; i < 8; i++)
		{
			if (guid & 0xFF)
			{
				_storage[mask_position] |= u8(1 << i);
				*this << u8(guid & 0xFF);
			}

			guid >>= 8;
		}
	}

	void put(size_t pos, const u8 *src, size_t cnt)
	{
		assert(pos + cnt <= size() || PriPosError(true, pos, cnt));
		memcpy(&_storage[pos], src, cnt);
	}

	const u8 *get() const { return &_storage[_rpos]; }

	const u8 at(u32 pos) const { return _storage[pos]; }

	void hexlike() const
	{
		u32 j = 1, k = 1;

		for (u32 i = 0; i < size(); i++)
		{
			if ((i == (j * 8)) && ((i != (k * 16))))
			{
				++j;
			}
			else if (i == (k * 16))
			{
				++k;
				++j;
			}
		}
	}

protected:
	bool PriPosError(bool add, size_t pos, size_t esize) const
	{
		return false;
	}

	bool _vaild;
	size_t _rpos, _wpos;
	std::vector<u8> _storage;
};

template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> v)
{
	b << (u32)v.size();
	for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++)
	{
		b << *i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
	u32 vsize;
	b >> vsize;
	v.clear();
	while (vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::list<T> v)
{
	b << (u32)v.size();
	for (typename std::list<T>::iterator i = v.begin(); i != v.end(); i++)
	{
		b << *i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
	u32 vsize;
	b >> vsize;
	v.clear();
	while (vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> &m)
{
	b << (u32)m.size();
	for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); i++)
	{
		b << i->first << i->second;
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
	u32 msize;
	b >> msize;
	m.clear();
	while (msize--)
	{
		K k;
		V v;
		b >> k >> v;
		m.insert(make_pair(k, v));
	}
	return b;
}
#endif

