#pragma once

#include <array>
#include <vector>
#include <functional>
#include <memory>
#include <boost/integer/static_log2.hpp>
#include <boost/range/adaptor/reversed.hpp>

namespace gorc {

template <typename T> class pool_ptr {
public:
	std::unique_ptr<T> value;

	inline T& operator*() {
		return *value;
	}

	inline const T& operator*() const {
		return *value;
	}

	inline T* operator->() {
		return value.get();
	}

	inline T const* operator->() const {
		return value.get();
	}
};

template <typename T, size_t PageSize = 128> class pool {
	static_assert(PageSize > 0 && (PageSize & (PageSize - 1)) == 0, "PageSize must be a power of 2");
	static const int PageNumberShift = boost::static_log2<PageSize>::value;
	static const size_t PageOffsetmask = PageSize - 1;
	friend class Iterator;

public:
	class element : public T {
		friend class pool;
		element* pool_next_free = nullptr;
		int pool_element_id;

	public:
		inline int get_id() const {
			return pool_element_id;
		}

		element() = default;

		element(const T& value) {
			*this = value;
		}

		template <typename U> auto operator=(const U& value) -> decltype(T::operator=(value)) {
			return T::operator=(value);
		}
	};

private:
	std::function<void(pool&, element&)> del_cb;
	element* first_free = nullptr;
	std::vector<std::unique_ptr<std::array<element, PageSize>>> pages;

	void add_page() {
		int page_base = (pages.size() << PageNumberShift) | PageOffsetmask;

		pages.emplace_back(new std::array<element, PageSize>());

		element* next_free = nullptr;
		for(auto& obj : *pages.back() | boost::adaptors::reversed) {
			obj.pool_next_free = next_free;
			obj.pool_element_id = page_base--;
			next_free = &obj;
		}

		first_free = next_free;
	}

	element& get_pool_object(int index) {
		unsigned int page_num = index >> PageNumberShift;
		unsigned int item_num = index & PageOffsetmask;

		return (*pages[page_num])[item_num];
	}

	const element& get_pool_object(int index) const {
		unsigned int page_num = index >> PageNumberShift;
		unsigned int item_num = index & PageOffsetmask;

		return (*pages[page_num])[item_num];
	}

	static void default_destroy_callback(pool& pool, element& value) {
		return;
	}

	int get_index_upper_bound() const {
		return ((pages.size() - 1) << PageNumberShift) | PageOffsetmask;
	}

public:
	class iterator {
	private:
		int index;
		pool* pool;

		void advance() {
			int upper_bound = pool->get_index_upper_bound();
			++index;
			while(index < upper_bound) {
				if(!pool->get_pool_object(index).pool_next_free) {
					return;
				}
				++index;
			}

			index = 0;
			pool = nullptr;
		}

	public:
		iterator()
			: index(0), pool(nullptr) {
			return;
		}

		iterator(int index, class pool* pool)
			: index(index), pool(pool) {
			if(pool->get_pool_object(index).pool_next_free) {
				advance();
			}
			return;
		}

		bool operator==(const iterator& it) const {
			return pool == it.pool && index == it.index;
		}

		bool operator!=(const iterator& it) const {
			return pool != it.pool || index != it.index;
		}

		element& operator*() {
			return (*pool)[index];
		}

		element* operator->() {
			return &(*pool)[index];
		}

		iterator& operator++() {
			advance();
			return *this;
		}

		iterator operator++(int) {
			iterator cpy(*this);
			advance();
			return cpy;
		}
	};

	pool(std::function<void(pool&, element&)> delete_callback = default_destroy_callback)
		: del_cb(delete_callback), first_free(nullptr) {
		add_page();
		return;
	}

	element& operator[](int index) {
		return get_pool_object(index);
	}

	const element& operator[](int index) const {
		return get_pool_object(index);
	}

	element& create() {
		if(first_free) {
			element* obj = first_free;
			first_free = obj->pool_next_free;
			obj->pool_next_free = nullptr;
			return *obj;
		}
		else {
			add_page();
			return create();
		}
	}

	void destroy(element& em) {
		em.pool_next_free = first_free;
		first_free = &em;
		del_cb(*this, em);
	}

	void destroy(int index) {
		auto& obj = get_pool_object(index);
		destroy(obj);
	}

	iterator begin() {
		return iterator(0, this);
	}

	iterator end() {
		return iterator();
	}

	size_t size() const {
		return pages.size() * PageSize;
	}
};

}
