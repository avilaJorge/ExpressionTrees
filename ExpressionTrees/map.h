#pragma once
#include <string>
#include <iterator>

const int HASH_SEED = 5381;
const int HASH_MULTIPLIER = 33;
const int HASH_MASK = unsigned(-1) >> 1;

static int hashCode(const std::string& str)
{
	unsigned hash = HASH_SEED;
	int n = str.length();
	for (int i = 0; i < n; i++)
	{
		hash = HASH_MULTIPLIER*hash + str[i];
	}
	return int(hash&HASH_MASK);
}

template <typename T>
int hashCode(const T& input)
{
	std::string str = std::to_string(input);

	unsigned hash = HASH_SEED;
	int n = str.length();
	for (int i = 0; i < n; i++)
	{
		hash = HASH_MULTIPLIER*hash + str[i];
	}
	return int(hash&HASH_MASK);
}

template<typename KeyType, typename ValueType>
class Map
{
public:
	Map(int(*hashFunction)(const KeyType& input) = hashCode) :
		hashFunction(hashFunction)
	{
		bucketCount_ = INITIAL_BUCKET_COUNT;
		buckets_ = new Cell<KeyType, ValueType>*[bucketCount_];

		for (int i = 0; i < bucketCount_; i++)
		{
			buckets_[i] = nullptr;
		}
	}
	~Map();
	int size() const;
	void clear();
	bool empty() const;
	void put(const KeyType& key, const ValueType& value);
	void remove(const KeyType& key);
	ValueType get(const KeyType& key) const;
	bool contains(const KeyType& key) const;
	ValueType& operator[](const KeyType& key);

	//forward declarations
	template <typename T>
	class MapIterator;
	template<typename KeyType, typename ValueType>
	class Cell;

	/*Iterator typedefs*/
	typedef MapIterator<Cell<KeyType, ValueType>> iterator;
	typedef MapIterator<const Cell<KeyType, ValueType>> const_iterator;
	/*begin and end*/
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
private:
	template<typename KeyType, typename ValueType>
	class Cell
	{
	public:
		KeyType key;
		ValueType value;
		Cell<KeyType, ValueType>* link;
		Cell(KeyType key, ValueType value = ValueType(),
			Cell<KeyType, ValueType>* link = nullptr) :
			key(key), value(value), link(link) {}
		Cell(KeyType key, Cell<KeyType, ValueType>* link = nullptr) :
			key(key), value(ValueType()), link(link) {}
	};
	static const int INITIAL_BUCKET_COUNT = 13;
	const double REHASH_LOAD = 0.7;
	Cell<KeyType, ValueType>** buckets_;
	int cellCount_;
	int bucketCount_;
	Cell<KeyType, ValueType>* findCell(int bucket,
		const KeyType& key) const;
	//Make copying illegal
	Map(const Map<KeyType, ValueType>& source) {}
	Map<KeyType, ValueType>& operator=(const Map<KeyType, ValueType>& source) {}
	void deleteBuckets();
	int(*hashFunction)(const KeyType& input);
	double load() const;
	void rehash();

	/*Iterator definition*/
	template <typename T>
	class MapIterator : public std::iterator < std::forward_iterator_tag, T>
	{
	public:
		MapIterator()
		{
			buckets_ = nullptr;
			cell_ = nullptr;
			bucketCount_ = 0;
			bucketIndex_ = -1;
		}
		MapIterator(Cell<KeyType, ValueType>** buckets, int bucketCount) :
			buckets_(buckets), bucketCount_(bucketCount)
		{
			bucketIndex_ = 0;
			cell_ = buckets_[bucketIndex_];

			while (cell_ == nullptr && bucketIndex_ < bucketCount_ - 1)
			{
				bucketIndex_++;
				cell_ = buckets_[bucketIndex_];
			}
		}
		~MapIterator() {}
		MapIterator(const MapIterator<T>& iter)
		{
			buckets_ = iter.buckets_;
			bucketIndex_ = iter.bucketIndex_;
			bucketCount_ = iter.bucketCount_;
			cell_ = iter.cell_;
		}
		MapIterator<T>& operator=(const MapIterator<T>& iter)
		{
			buckets_ = iter.buckets_;
			bucketIndex_ = iter.bucketIndex_;
			bucketCount_ = iter.bucketCount_;
			cell_ = iter.cell_;
			return *this;
		}
		MapIterator<T>& operator++()
		{
			cell_ = cell_->link;
			while (cell_ == nullptr && bucketIndex_ < bucketCount_ - 1)
			{
				bucketIndex_++;
				cell_ = buckets_[bucketIndex_];
			}
			return *this;
		}
		MapIterator<T> operator++(int)
		{
			MapIterator<T> old(*this);
			this->operator++();
			return old;
		}
		bool operator==(const MapIterator<T>& iter) const
		{
			return (buckets_ == iter.buckets_ && cell_ == iter.cell_ && bucketIndex_ == iter.bucketIndex_);
		}
		bool operator!=(const MapIterator<T>& iter) const
		{
			return !(*this == iter);
		}
		T& operator*() const
		{
			return *cell_;
		}
		T* operator->() const
		{
			return cell_;
		}
		void moveToEnd()
		{
			cell_ = nullptr;
			bucketIndex_ = bucketCount_ - 1;
		}
	private:
		Cell<KeyType, ValueType>** buckets_;
		Cell<KeyType, ValueType>* cell_;
		int bucketCount_;
		int bucketIndex_;
	};

};

template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::~Map()
{
	deleteBuckets();
}

template<typename KeyType, typename ValueType>
int Map<KeyType, ValueType>::size() const
{
	return cellCount_;
}

template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::clear()
{
	deleteBuckets();
}

template<typename KeyType, typename ValueType>
bool Map<KeyType, ValueType>::empty() const
{
	return cellCount_ == 0;
}

template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::put(const KeyType& key, const ValueType& value)
{
	int bucket = hashFunction(key) % bucketCount_;

	//Scan through the bucket. If you hit a matching key
	//Then update the associated value for that key.
	//If you get to the end of the chain, then just attach
	//a new (key, value)-pair.
	if (!buckets_[bucket])
	{
		if (load() > REHASH_LOAD)
		{
			rehash();
		}
		buckets_[bucket] = new Cell<KeyType, ValueType>(key, value);
		cellCount_++;
		return;
	}

	Cell<KeyType, ValueType>* cell = buckets_[bucket];

	//Special case where there is exactly one cell and that 
	//cell's key matches the new key
	if (cell->key == key)
	{
		cell->value = value;
		return;
	}

	if (load() > REHASH_LOAD)
	{
		rehash();
	}

	while (cell->link)
	{
		if (cell->key == key)
		{
			cell->value = value;
			return;
		}
		cell = cell->link;
	}
	cell->link = new Cell<KeyType, ValueType>(key, value);
	cellCount_++;
}

template<typename KeyType, typename ValueType>
ValueType Map<KeyType, ValueType>::get(const KeyType& key) const
{
	return findCell(hashFunction(key) % bucketCount_, key)->value;
}
template<typename KeyType, typename ValueType>
bool Map<KeyType, ValueType>::contains(const KeyType& key) const
{
	return findCell(hashFunction(key) % bucketCount_, key);
}

template<typename KeyType, typename ValueType>
ValueType& Map<KeyType, ValueType>::operator[](const KeyType& key)
{
	Cell<KeyType, ValueType>* cell =
		findCell(hashFunction(key) % bucketCount_, key);
	//case 1:  found it
	if (cell)
	{
		return cell->value;
	}
	//case 2: didn't find it
	else
	{
		put(key, ValueType());
		return findCell(hashFunction(key) % bucketCount_, key)->value;
	}
}

template<typename KeyType, typename ValueType>
typename Map<KeyType, ValueType>::Cell<KeyType, ValueType>*
Map<KeyType, ValueType>::findCell(int bucket, const KeyType& key)
const
{

	Cell<KeyType, ValueType>* cell = buckets_[bucket];
	while (cell)
	{
		if (cell->key == key)
		{
			return cell;
		}
		cell = cell->link;
	}
	return nullptr;
}

template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::deleteBuckets()
{
	for (int i = 0; i < bucketCount_; i++)
	{
		Cell<KeyType, ValueType>* cell = buckets_[i];
		while (cell)
		{
			Cell<KeyType, ValueType>* temp = cell;
			cell = cell->link;
			delete temp;
		}
	}
	delete[] buckets_;
}

template<typename KeyType, typename ValueType>
double Map<KeyType, ValueType>::load() const
{
	return cellCount_ / double(bucketCount_);
}

template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::rehash()
{
	int oldBucketCount = bucketCount_;
	bucketCount_ = cellCount_ / 0.3;
	//How empty do you want it to be when you rehash??

	Cell<KeyType, ValueType>** oldTable = buckets_;
	cellCount_ = 0;
	buckets_ = new Cell<KeyType, ValueType>*[bucketCount_];

	for (int i = 0; i < bucketCount_; i++)
	{
		buckets_[i] = nullptr;
	}

	//Move all the old stuff to the new table
	for (int i = 0; i < oldBucketCount; i++)
	{
		Cell<KeyType, ValueType>* cell = oldTable[i];
		while (cell)
		{
			put(cell->key, cell->value);
			Cell<KeyType, ValueType>* temp = cell;
			cell = cell->link;
			delete temp;
		}
	}

	delete[] oldTable;
}

template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::remove(const KeyType& key)
{
	int bucket = hashFunction(key) % bucketCount_;

	Cell<KeyType, ValueType>* cell = buckets_[bucket];

	if (!cell)
	{
		return;
	}

	//Special case where the key is in the very first cell
	if (cell->key == key)
	{
		buckets_[bucket] = cell->link;
		delete cell;
		return;
	}

	while (cell->link)
	{
		if (cell->link->key == key)
		{
			Cell<KeyType, ValueType>* temp = cell->link;
			cell->link = cell->link->link;
			delete temp;
			return;
		}
	}
}

template<typename KeyType, typename ValueType>
typename Map<KeyType, ValueType>::iterator Map<KeyType, ValueType>::begin()
{
	return iterator(buckets_, bucketCount_);
}

template<typename KeyType, typename ValueType>
typename Map<KeyType, ValueType>::iterator Map<KeyType, ValueType>::end()
{
	iterator iter(buckets_, bucketCount_);
	iter.moveToEnd();
	return iter;
}

template<typename KeyType, typename ValueType>
typename Map<KeyType, ValueType>::const_iterator Map<KeyType, ValueType>::begin() const
{
	return const_iterator(buckets_, bucketCount_);
}

template<typename KeyType, typename ValueType>
typename Map<KeyType, ValueType>::const_iterator Map<KeyType, ValueType>::end() const
{
	const_iterator iter(buckets_, bucketCount_);
	iter.moveToEnd();
	return iter;
}