#pragma once

#include "narrow.hpp"
#include "runtime_assert.hpp"
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace gorc {
    constexpr const ptrdiff_t span_to_end = -1;

    template <class SpanT, bool ReturnConstRef>
    class span_iterator {
    private:
        SpanT const *underlying_span;
        size_t index;

        using element_type = typename SpanT::element_type;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::remove_cv_t<element_type>;
        using pointer = std::add_pointer_t<element_type>;
        using reference = std::conditional_t<ReturnConstRef, element_type const, element_type> &;
        using iterator_category = std::random_access_iterator_tag;

        constexpr span_iterator(SpanT const &underlying_span, size_t index)
            : underlying_span(&underlying_span)
            , index(index)
        {
        }

        constexpr reference operator*() const
        {
            return underlying_span->base[index];
        }

        constexpr pointer operator->() const
        {
            return &underlying_span->base[index];
        }

        constexpr span_iterator &operator++()
        {
            ++index;
            return *this;
        }

        constexpr span_iterator const operator++(int)
        {
            auto rv = *this;
            ++index;
            return rv;
        }

        constexpr span_iterator &operator--()
        {
            --index;
            return *this;
        }

        constexpr span_iterator const operator--(int)
        {
            auto rv = *this;
            --index;
            return rv;
        }

        constexpr span_iterator operator+(ptrdiff_t offset) const
        {
            return span_iterator(*underlying_span, index + offset);
        }

        constexpr span_iterator &operator+=(ptrdiff_t offset)
        {
            index += offset;
            return *this;
        }

        constexpr span_iterator operator-(ptrdiff_t offset) const
        {
            return span_iterator(*underlying_span, index - offset);
        }

        constexpr span_iterator &operator-=(ptrdiff_t offset)
        {
            index -= offset;
            return *this;
        }

        constexpr ptrdiff_t operator-(span_iterator const &other) const
        {
            logic_assert(underlying_span == other.underlying_span,
                         "span_iterator computed difference from multiple spans");
            return narrow<ptrdiff_t>(index) - narrow<ptrdiff_t>(other.index);
        }

        constexpr bool operator==(span_iterator const &other) const
        {
            logic_assert(underlying_span == other.underlying_span,
                         "span_iterator compared multiple spans");
            return index == other.index;
        }

        constexpr bool operator!=(span_iterator const &other) const
        {
            return !(*this == other);
        }

        constexpr bool operator<(span_iterator const &other) const
        {
            logic_assert(underlying_span == other.underlying_span,
                         "span_iterator compared multiple spans");
            return index < other.index;
        }

        constexpr bool operator<=(span_iterator const &other) const
        {
            return !(other < *this);
        }

        constexpr bool operator>(span_iterator const &other) const
        {
            return other < *this;
        }

        constexpr bool operator>=(span_iterator const &other) const
        {
            return !(*this < other);
        }
    };

    template <class EmT>
    class span {
        friend span_iterator<span, true>;
        friend span_iterator<span, false>;

    private:
        EmT *base = nullptr;
        size_t count = 0;

    public:
        using element_type = EmT;
        using const_iterator = span_iterator<span, true>;
        using iterator = span_iterator<span, false>;

        span(EmT *base, size_t count)
            : base(base)
            , count(count)
        {
        }

        iterator begin()
        {
            return iterator(*this, 0);
        }

        iterator end()
        {
            return iterator(*this, count);
        }

        const_iterator begin() const
        {
            return const_iterator(*this, 0);
        }

        const_iterator end() const
        {
            return const_iterator(*this, count);
        }

        const_iterator cbegin() const
        {
            return const_iterator(*this, 0);
        }

        const_iterator cend() const
        {
            return const_iterator(*this, count);
        }

        span subspan(size_t start, size_t num) const
        {
            logic_assert(start <= count, "span::subspan starts past end of span");

            if(num == span_to_end) {
                return span(base + start, count - start);
            }

            logic_assert((start + num) <= count, "span::subspan exceeds dimensions of span");
            return span(base + start, num);
        }

        size_t size() const
        {
            return count;
        }

        bool empty() const
        {
            return count == 0;
        }

        size_t size_bytes() const
        {
            return narrow<size_t>(reinterpret_cast<uint8_t const *>(base + count) -
                                  reinterpret_cast<uint8_t const *>(base));
        }

        EmT *data()
        {
            return base;
        }

        EmT const *data() const
        {
            return base;
        }

        span<char> as_bytes()
        {
            return span<char>(reinterpret_cast<char *>(base), size_bytes());
        }

        span<unsigned char> as_unsigned_bytes()
        {
            return span<unsigned char>(reinterpret_cast<unsigned char *>(base), size_bytes());
        }

        span<char const> as_const_bytes() const
        {
            return span<char const>(reinterpret_cast<char const *>(base), size_bytes());
        }

        span<unsigned char const> as_unsigned_const_bytes() const
        {
            return span<unsigned char const>(reinterpret_cast<unsigned char const *>(base),
                                             size_bytes());
        }

        span<char const> as_bytes() const
        {
            return span<char const>(reinterpret_cast<char const *>(base), size_bytes());
        }

        span<unsigned char const> as_unsigned_bytes() const
        {
            return span<unsigned char const>(reinterpret_cast<unsigned char const *>(base),
                                             size_bytes());
        }

        operator span<EmT const>() const
        {
            return span<EmT const>(base, count);
        }
    };

    template <class EmT, size_t N>
    span<EmT> make_span(EmT (&arr)[N])
    {
        return span<EmT>(static_cast<EmT *>(arr), N);
    }

    template <class EmT, size_t N>
    span<EmT> make_string_span(EmT (&arr)[N])
    {
        return span<EmT>(static_cast<EmT *>(arr), N - 1);
    }

    template <class EmT>
    span<EmT> make_span(EmT *base, size_t n)
    {
        return span<EmT>(base, n);
    }

    template <
        class SeqRangeT,
        class EmT = typename std::remove_pointer_t<decltype(std::declval<SeqRangeT>().data())>>
    span<EmT> make_span(SeqRangeT &&seq)
    {
        return span<EmT>(seq.data(), seq.size());
    }
}
