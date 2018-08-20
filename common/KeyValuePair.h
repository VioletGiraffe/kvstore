#pragma once

#include <QHash>
#include <QIODevice>

template <typename Key, typename Value>
struct KeyValuePair
{
	Key key;
	Value value;

	uint keyHash() const
	{
		return qHash(key);
	}

	bool write(QIODevice& device)
	{
		if (device.write(&key, sizeof(key)) != sizeof(key))
			return false;

		if (device.write(&value, sizeof(value)) != sizeof(value))
			return false;

		return true;
	}

	bool read(QIODevice& device)
	{
		if (device.read(&key, sizeof(key)) != sizeof(key))
			return false;

		if (device.read(&value, sizeof(value)) != sizeof(value))
			return false;

		return true;
	}

	bool valid() const
	{
		return key != Key{};
	}

	size_t spaceRequired() const
	{
		return sizeof(key) + sizeof(value);
	}
};
