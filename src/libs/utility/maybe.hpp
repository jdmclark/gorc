#pragma once

#include <type_traits>
#include <utility>

namespace gorc {

    struct nothing_type { };
    constexpr nothing_type nothing = nothing_type();

    template <typename T, typename Enable = void> class maybe;

    template <typename T>
    class maybe<T, typename std::enable_if<std::is_pointer<T>::value>::type> {
    private:
        T value;

    public:
        maybe()
            : value(nullptr)
        {
            return;
        }

        maybe(T value)
            : value(value)
        {
            return;
        }

        maybe(nothing_type)
            : value(nullptr)
        {
            return;
        }

        maybe<T>& operator=(T value)
        {
            this->value = value;
            return *this;
        }

        maybe<T>& operator=(nothing_type const &)
        {
            this->value = nullptr;
            return *this;
        }

        inline bool operator==(maybe<T> const &o) const
        {
            return value == o.value;
        }

        inline bool operator!=(maybe<T> const &o) const
        {
            return value != o.value;
        }

        inline bool has_value() const
        {
            return value != nullptr;
        }

        inline T get_value() const
        {
            return value;
        }

        inline T& get_value()
        {
            return value;
        }
    };

    template <typename T>
    class maybe<T, typename std::enable_if<!std::is_pointer<T>::value>::type> {
    private:
        typename std::aligned_storage<sizeof(T), alignof(T)>::type data;
        bool present = false;

        inline void clear_data_if_present()
        {
            if(present) {
                reinterpret_cast<T*>(&data)->~T();
                present = false;
            }
        }

        inline void set_data(T const &value)
        {
            clear_data_if_present();
            new(&data) T(value);
            present = true;
        }

        inline void set_data(T &&value)
        {
            clear_data_if_present();
            new(&data) T(std::forward<T>(value));
            present = true;
        }

    public:
        maybe()
            : present(false)
        {
            return;
        }

        maybe(T const &value)
            : present(false)
        {
            set_data(value);
            return;
        }

        maybe(T &&value)
            : present(false)
        {
            set_data(std::forward<T>(value));
            return;
        }

        maybe(maybe<T> const &value)
            : present(false)
        {
            if(value.has_value()) {
                set_data(value.get_value());
            }
        }

        maybe(maybe<T> &&value)
            : present(false)
        {
            if(value.has_value()) {
                set_data(std::move(value.get_value()));
            }
        }

        maybe(nothing_type)
            : present(false)
        {
            return;
        }

        ~maybe()
        {
            clear_data_if_present();
            return;
        }

        maybe<T>& operator=(T const &value)
        {
            if(&get_value() != &value) {
                set_data(value);
            }

            return *this;
        }

        maybe<T>& operator=(T &&value)
        {
            set_data(std::forward<T>(value));
            return *this;
        }

        maybe<T>& operator=(maybe<T> &&value)
        {
            if(value.has_value()) {
                set_data(std::move(value.get_value()));
            }
            else {
                clear_data_if_present();
            }

            return *this;
        }

        maybe<T>& operator=(maybe<T> const &value)
        {
            if(this == &value) {
                return *this;
            }

            if(value.has_value()) {
                set_data(value.get_value());
            }
            else {
                clear_data_if_present();
            }

            return *this;
        }

        maybe<T>& operator=(nothing_type const &)
        {
            clear_data_if_present();
            return *this;
        }

        inline bool operator==(T const &o) const
        {
            return present && (get_value() == o);
        }

        inline bool operator==(maybe<T> const &o) const
        {
            return (!present && !o.present) ||
                   (present && o.present && (get_value() == o.get_value()));
        }

        inline bool operator!=(T const &o) const
        {
            return !present || (get_value() != o);
        }

        inline bool operator!=(maybe<T> const &o) const
        {
            return (present != o.present) ||
                   (present && (get_value() != o.get_value()));
        }

        inline bool has_value() const
        {
            return present;
        }

        inline T const& get_value() const
        {
            return *reinterpret_cast<T const *>(&data);
        }

        inline T& get_value()
        {
            return *reinterpret_cast<T*>(&data);
        }
    };

    template <typename T>
    maybe<T> make_maybe(T const &value)
    {
        return maybe<T>(value);
    }

    template <typename ValueT, typename IfFnT>
    auto maybe_if(ValueT &v, IfFnT fn)
        -> decltype(fn(v.get_value()))
    {
        if(v.has_value()) {
            return fn(v.get_value());
        }

        return decltype(fn(v.get_value()))();
    }

    template <typename ValueT, typename IfFnT, typename DefaultT>
    auto maybe_if(ValueT &v, DefaultT const &dflt, IfFnT fn)
        -> typename std::common_type<decltype(fn(v.get_value())), DefaultT>::type
    {
        if(v.has_value()) {
            return fn(v.get_value());
        }

        return dflt;
    }

    template <typename ValueT, typename IfFnT, typename ElseFnT>
    auto maybe_if_else(ValueT &v, IfFnT fn, ElseFnT else_fn)
        -> typename std::common_type<decltype(fn(v.get_value())), decltype(else_fn())>::type
    {
        if(v.has_value()) {
            return fn(v.get_value());
        }
        else {
            return else_fn();
        }
    }

    template <typename ValueT, typename IfFnT, typename ElseFnT>
    auto maybe_if_else(ValueT const &v, IfFnT fn, ElseFnT else_fn)
        -> typename std::common_type<decltype(fn(v.get_value())), decltype(else_fn())>::type
    {
        if(v.has_value()) {
            return fn(v.get_value());
        }
        else {
            return else_fn();
        }
    }
}
