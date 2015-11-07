#pragma once

#include <cstdio>
#include <type_traits>
#include <iterator>
#include <stack>
#include <stdexcept>
#include <boost/format.hpp>
#include "io/input_stream.hpp"
#include "io/output_stream.hpp"
#include "io/file.hpp"
#include "log/log.hpp"
#include "generic_tokenizer.hpp"

namespace gorc {

    class json_input_stream {
        template <typename EmFmtFnT>
        friend auto json_deserialize_heterogeneous(json_input_stream &f, EmFmtFnT efn)
            -> decltype(efn(f, std::string()));
        template <typename EmFmtFn>
        friend void json_deserialize_members(json_input_stream &f, EmFmtFn efn);
        template <typename EmFmtFn>
        friend void json_deserialize_array(json_input_stream &f, EmFmtFn efn);
        template <typename ValueT, typename OutItT>
        friend void json_deserialize_array(json_input_stream &f, OutItT const &rng);
        template <typename OutItT, typename EmFmtFn>
        friend void json_deserialize_array(json_input_stream &f, OutItT const &rng, EmFmtFn efn);
        friend void json_deserialize_null(json_input_stream &f);
        template <typename T>
        friend typename std::enable_if<std::is_same<std::string, T>::value, T>::type
            json_deserialize(json_input_stream &f);
        template <typename T>
        friend typename std::enable_if<std::is_fundamental<T>::value, T>::type
            json_deserialize(json_input_stream &f);

    private:
        generic_tokenizer tok;

        void advance_token();
        void assert_token(std::string const &);

        inline bool at_end()
        {
            return tok.get_type() == token_type::end_of_file;
        }

        int64_t read_integer();
        double read_double();

        template <typename T>
        typename std::enable_if<std::is_same<T, bool>::value, T>::type read_value()
        {
            if(tok.get_type() == token_type::identifier && tok.get_value() == "true") {
                advance_token();
                return true;
            }
            else if(tok.get_type() == token_type::identifier && tok.get_value() == "false") {
                advance_token();
                return false;
            }
            else {
                diagnostic_context dc(get_diagnostic_context());
                LOG_FATAL(format("'%s' is not a boolean literal") % tok.get_value());
            }
        }

        template <typename T>
        typename std::enable_if<std::is_integral<T>::value &&
                                !std::is_same<T, bool>::value, T>::type read_value()
        {
            int64_t value = read_integer();
            advance_token();
            return static_cast<T>(value);
        }

        template <typename T>
        typename std::enable_if<std::is_floating_point<T>::value, T>::type read_value()
        {
            double value = read_double();
            advance_token();
            return static_cast<T>(value);
        }

    public:
        explicit json_input_stream(input_stream& f);
        virtual ~json_input_stream();

        diagnostic_context_location get_diagnostic_context() const;
    };

    struct json_deserialization_constructor {
        json_deserialization_constructor();
    };

    json_deserialization_constructor const json_deserialization_constructor_tag;

    template <typename T>
    typename std::enable_if<std::is_fundamental<T>::value, T>::type
        json_deserialize(json_input_stream &f)
    {
        return f.read_value<T>();
    }

    template <typename T>
    typename std::enable_if<std::is_same<std::string, T>::value, T>::type
        json_deserialize(json_input_stream &f)
    {
        std::string rv = f.tok.get_value();
        f.advance_token();
        return rv;
    }

    template <typename T>
    typename std::enable_if<!std::is_fundamental<T>::value &&
                            !std::is_same<std::string, T>::value>::type
        json_deserialize(json_input_stream &f, T &obj)
    {
        obj.json_deserialize(f);
    }

    template <typename T>
    typename std::enable_if<!std::is_fundamental<T>::value &&
                            !std::is_same<std::string, T>::value, T>::type
        json_deserialize(json_input_stream &f)
    {
        T obj(json_deserialization_constructor_tag, f);
        return obj;
    }

    inline void json_deserialize_null(json_input_stream &f)
    {
        f.assert_token("null");
    }

    template <typename EmFmtFn>
    void json_deserialize_array(json_input_stream &f, EmFmtFn efn)
    {
        f.assert_token("[");

        while(!f.at_end()) {
            if(f.tok.get_value() == "]") {
                f.advance_token();
                return;
            }
            else if(f.tok.get_value() == ",") {
                f.advance_token();
                continue;
            }
            else {
                diagnostic_context ec(f.get_diagnostic_context());
                efn(f);
            }
        }

        diagnostic_context dc(f.get_diagnostic_context());
        LOG_FATAL("unexpected end of file inside array");
    }

    template <typename OutItT, typename EmFmtFn>
    void json_deserialize_array(json_input_stream &f, OutItT const &rng, EmFmtFn efn)
    {
        auto out_it = rng;

        f.assert_token("[");

        while(!f.at_end()) {
            if(f.tok.get_value() == "]") {
                f.advance_token();
                return;
            }
            else if(f.tok.get_value() == ",") {
                f.advance_token();
                continue;
            }
            else {
                diagnostic_context ec(f.get_diagnostic_context());
                *out_it = efn(f);
                ++out_it;
            }
        }

        diagnostic_context dc(f.get_diagnostic_context());
        LOG_FATAL("unexpected end of file inside array");
    }

    template <typename ValueT, typename OutItT>
    void json_deserialize_array(json_input_stream &f, OutItT const &rng)
    {
        auto out_it = rng;

        f.assert_token("[");

        while(!f.at_end()) {
            if(f.tok.get_value() == "]") {
                f.advance_token();
                return;
            }
            else if(f.tok.get_value() == ",") {
                f.advance_token();
                continue;
            }
            else {
                *out_it = json_deserialize<ValueT>(f);
                ++out_it;
            }
        }

        diagnostic_context dc(f.get_diagnostic_context());
        LOG_FATAL("unexpected end of file inside array");
    }

    template <typename ValueT, typename OutItT>
    void json_deserialize_array(json_input_stream &f, OutItT const &begin, OutItT const &end)
    {
        auto it = begin;
        json_deserialize_array(f, [&](json_input_stream &f) {
                if(it == end) {
                    LOG_FATAL("array contains too many elements");
                }

                *it = json_deserialize<ValueT>(f);
                ++it;
            });

        if(it != end) {
            diagnostic_context dc(f.get_diagnostic_context());
            LOG_FATAL("array contains too few elements");
        }
    }

    template <typename EmFmtFn>
    void json_deserialize_members(json_input_stream &f, EmFmtFn efn)
    {
        f.assert_token("{");

        std::string name_buf;

        while(!f.at_end()) {
            if(f.tok.get_value() == "}") {
                f.advance_token();
                return;
            }
            else if(f.tok.get_value() == ",") {
                f.advance_token();
                continue;
            }
            else {
                // Current token is a member name.
                if(f.tok.get_type() != token_type::string &&
                   f.tok.get_type() != token_type::identifier) {
                    diagnostic_context dc(f.get_diagnostic_context());
                    LOG_FATAL(format("expected key, found '%s'") % f.tok.get_value());
                }
                name_buf = f.tok.get_value();
                f.advance_token();
                f.assert_token(":");

                diagnostic_context ec(f.get_diagnostic_context());
                efn(f, name_buf);
            }
        }

        diagnostic_context dc(f.get_diagnostic_context());
        LOG_FATAL("unexpected end of file inside object");
    }

    template <typename EmFmtFn>
    auto json_deserialize_heterogeneous(json_input_stream &f, EmFmtFn efn)
        -> decltype(efn(f, std::string()))
    {
        if(f.tok.get_type() != token_type::string &&
           f.tok.get_type() != token_type::identifier) {
            diagnostic_context dc(f.get_diagnostic_context());
            LOG_FATAL(format("expected type, found '%s'") % f.tok.get_value());
        }

        std::string type_buf = f.tok.get_value();
        f.advance_token();

        diagnostic_context ec(f.get_diagnostic_context());
        return efn(f, type_buf);
    }

    template <typename OutItT, typename EmFmtFn>
    void json_deserialize_heterogeneous_array(json_input_stream &f, OutItT const &rng, EmFmtFn efn)
    {
        json_deserialize_array(f, rng, [&](json_input_stream &f) {
                return json_deserialize_heterogeneous(f, efn);
            });
    }

    template <typename EmFmtFn>
    void json_deserialize_heterogeneous_array(json_input_stream &f, EmFmtFn efn)
    {
        json_deserialize_array(f, [&](json_input_stream &f) {
                json_deserialize_heterogeneous(f, efn);
            });
    }

}
