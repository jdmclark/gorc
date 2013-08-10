#pragma once

#include <array>
#include <vector>
#include <functional>
#include <memory>
#include <boost/integer/static_log2.hpp>
#include <boost/range/adaptor/reversed.hpp>

namespace Gorc {

template <typename T> class PoolPtr {
public:
	std::unique_ptr<T> Value;

	inline T& operator*() {
		return *Value;
	}

	inline const T& operator*() const {
		return *Value;
	}

	inline T* operator->() {
		return Value.get();
	}

	inline T const* operator->() const {
		return Value.get();
	}
};

template <typename T, size_t PageSize = 128> class Pool {
	static_assert(PageSize > 0 && (PageSize & (PageSize - 1)) == 0, "PageSize must be a power of 2");
	static const int PageNumberShift = boost::static_log2<PageSize>::value;
	static const size_t PageOffsetMask = PageSize - 1;
	friend class Iterator;

public:
	class Element : public T {
		friend class Pool;
		Element* pool_next_free = nullptr;
		int pool_element_id;

	public:
		inline int GetId() const {
			return pool_element_id;
		}

		Element() = default;

		Element(const T& value) {
			*this = value;
		}

		template <typename U> auto operator=(const U& value) -> decltype(T::operator=(value)) {
			return T::operator=(value);
		}
	};

private:
	std::function<void(Pool&, Element&)> del_cb;
	Element* first_free = nullptr;
	std::vector<std::unique_ptr<std::array<Element, PageSize>>> pages;

	void AddPage() {
		int page_base = (pages.size() << PageNumberShift) | PageOffsetMask;

		pages.emplace_back(new std::array<Element, PageSize>());

		Element* next_free = nullptr;
		for(auto& obj : *pages.back() | boost::adaptors::reversed) {
			obj.pool_next_free = next_free;
			obj.pool_element_id = page_base--;
			next_free = &obj;
		}

		first_free = next_free;
	}

	Element& GetPoolObject(int index) {
		unsigned int page_num = index >> PageNumberShift;
		unsigned int item_num = index & PageOffsetMask;

		return (*pages[page_num])[item_num];
	}

	const Element& GetPoolObject(int index) const {
		unsigned int page_num = index >> PageNumberShift;
		unsigned int item_num = index & PageOffsetMask;

		return (*pages[page_num])[item_num];
	}

	static void DefaultDestroyCallback(Pool& pool, Element& value) {
		return;
	}

	int GetIndexUpperBound() const {
		return ((pages.size() - 1) << PageNumberShift) | PageOffsetMask;
	}

public:
	class Iterator {
	private:
		int index;
		Pool* pool;

		void Advance() {
			int upper_bound = pool->GetIndexUpperBound();
			++index;
			while(index < upper_bound) {
				if(!pool->GetPoolObject(index).pool_next_free) {
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

		Iterator(int index, Pool* pool)
			: index(index), pool(pool) {
			if(pool->GetPoolObject(index).pool_next_free) {
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

		Element& operator*() {
			return (*pool)[index];
		}

		Element* operator->() {
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
	};

	Pool(std::function<void(Pool&, Element&)> delete_callback = DefaultDestroyCallback)
		: del_cb(delete_callback), first_free(nullptr) {
		AddPage();
		return;
	}

	Element& operator[](int index) {
		return GetPoolObject(index);
	}

	const Element& operator[](int index) const {
		return GetPoolObject(index);
	}

	Element& Create() {
		if(first_free) {
			Element* obj = first_free;
			first_free = obj->pool_next_free;
			obj->pool_next_free = nullptr;
			return *obj;
		}
		else {
			AddPage();
			return Create();
		}
	}

	void Destroy(Element& em) {
		em.pool_next_free = first_free;
		first_free = &em;
		del_cb(*this, em);
	}

	void Destroy(int index) {
		auto& obj = GetPoolObject(index);
		Destroy(obj);
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
