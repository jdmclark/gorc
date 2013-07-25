#pragma once

#include <array>
#include <vector>
#include <functional>
#include <memory>
#include <boost/integer/static_log2.hpp>
#include <boost/range/adaptor/reversed.hpp>

namespace Gorc {

template <typename T, size_t PageSize = 128> class Pool {
	static_assert(PageSize > 0 && (PageSize & (PageSize - 1)) == 0, "PageSize must be a power of 2");
	static const int PageNumberShift = boost::static_log2<PageSize>::value;
	static const size_t PageOffsetMask = PageSize - 1;
	friend class Iterator;

	class PoolObject {
	public:
		T value;
		unsigned int number;
		PoolObject* next_free = nullptr;
	};

	std::function<void(Pool&, unsigned int, T&)> del_cb;
	PoolObject* first_free = nullptr;
	std::vector<std::unique_ptr<std::array<PoolObject, PageSize>>> pages;

	void AddPage() {
		unsigned int page_base = (pages.size() << PageNumberShift) | PageOffsetMask;

		pages.emplace_back(new std::array<PoolObject, PageSize>());

		PoolObject* next_free = nullptr;
		for(auto& obj : *pages.back() | boost::adaptors::reversed) {
			obj.next_free = next_free;
			obj.number = page_base--;
			next_free = &obj;
		}

		first_free = next_free;
	}

	PoolObject& GetPoolObject(unsigned int index) {
		unsigned int page_num = index >> PageNumberShift;
		unsigned int item_num = index & PageOffsetMask;

		return (*pages[page_num])[item_num];
	}

	const PoolObject& GetPoolObject(unsigned int index) const {
		unsigned int page_num = index >> PageNumberShift;
		unsigned int item_num = index & PageOffsetMask;

		return (*pages[page_num])[item_num];
	}

	static void DefaultDestroyCallback(Pool& pool, unsigned int index, T& value) {
		return;
	}

	unsigned int GetIndexUpperBound() const {
		return ((pages.size() - 1) << PageNumberShift) | PageOffsetMask;
	}

public:
	class Iterator {
	private:
		unsigned int index;
		Pool* pool;

		void Advance() {
			unsigned int upper_bound = pool->GetIndexUpperBound();
			++index;
			while(index < upper_bound) {
				if(!pool->GetPoolObject(index).next_free) {
					return;
				}
				++index;
			}

			index = 0;
			pool = nullptr;
		}

	public:
		Iterator()
			: index(0), pool(nullptr) {
			return;
		}

		Iterator(unsigned int index, Pool* pool)
			: index(index), pool(pool) {
			if(pool->GetPoolObject(index).next_free) {
				Advance();
			}
			return;
		}

		bool operator==(const Iterator& it) const {
			return pool == it.pool && index == it.index;
		}

		bool operator!=(const Iterator& it) const {
			return pool != it.pool || index != it.index;
		}

		T& operator*() {
			return (*pool)[index];
		}

		T* operator->() {
			return &(*pool)[index];
		}

		Iterator& operator++() {
			Advance();
			return *this;
		}

		Iterator operator++(int) {
			Iterator cpy(*this);
			Advance();
			return cpy;
		}

		unsigned int GetIndex() const {
			return index;
		}
	};

	Pool(std::function<void(Pool&, unsigned int, T&)> delete_callback = DefaultDestroyCallback)
		: del_cb(delete_callback), first_free(nullptr) {
		AddPage();
		return;
	}

	T& operator[](unsigned int index) {
		return GetPoolObject(index).value;
	}

	const T& operator[](unsigned int index) const {
		return GetPoolObject(index).value;
	}

	std::tuple<T*, unsigned int> Create() {
		if(first_free) {
			PoolObject* obj = first_free;
			first_free = obj->next_free;
			obj->next_free = nullptr;
			return std::make_tuple(&obj->value, obj->number);
		}
		else {
			AddPage();
			return Create();
		}
	}

	void Destroy(unsigned int index) {
		auto& obj = GetPoolObject(index);
		obj.next_free = first_free;
		first_free = &obj;
		del_cb(*this, index, obj.value);
	}

	Iterator begin() {
		return Iterator(0, this);
	}

	Iterator end() {
		return Iterator();
	}

	size_t size() const {
		return pages.size() * PageSize;
	}
};

}
