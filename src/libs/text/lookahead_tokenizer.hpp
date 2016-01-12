#pragma once

#include "tokenizer.hpp"
#include "utility/runtime_assert.hpp"
#include <memory>
#include <vector>
#include <string>

namespace gorc {

    template <typename TokenizerT>
    class token_lookahead {
    private:
        TokenizerT &tok;
        size_t num_lookahead = 1;

    public:
        using TokenTypeT = decltype(tok.get_type());

        class token {
        public:
            std::string value;
            std::string reason;
            TokenTypeT type;
            diagnostic_context_location location;

            token(std::string const &value,
                  std::string const &reason,
                  TokenTypeT type,
                  diagnostic_context_location const &location)
                : value(value)
                , reason(reason)
                , type(type)
                , location(location)
            {
                return;
            }
        };

    private:
        std::vector<std::unique_ptr<token>> token_buffer;

        void grow()
        {
            token_buffer.push_back(std::make_unique<token>(tok.get_value(),
                                                           tok.get_reason(),
                                                           tok.get_type(),
                                                           tok.get_location()));
            tok.advance();
        }

    public:
        token_lookahead(TokenizerT &tok,
                        size_t num_lookahead)
            : tok(tok)
            , num_lookahead(num_lookahead <= 1 ? 1 : num_lookahead)
        {
            while(token_buffer.size() < num_lookahead) {
                grow();
            }

            return;
        }

        token const& get_token(size_t index) const
        {
            return *token_buffer.at(index);
        }

        void advance()
        {
            // Bubble front to back
            auto jt = token_buffer.begin();
            auto it = jt++;
            while(jt != token_buffer.end()) {
                std::swap(*it, *jt);
                ++it;
                ++jt;
            }

            // Replace back with new token
            token &t = **it;
            t.value = tok.get_value();
            t.reason = tok.get_reason();
            t.type = tok.get_type();
            t.location = tok.get_location();
            tok.advance();
        }

        TokenTypeT get_type() const
        {
            return token_buffer[0]->type;
        }

        std::string const & get_value() const
        {
            return token_buffer[0]->value;
        }

        diagnostic_context_location const & get_location() const
        {
            return token_buffer[0]->location;
        }

        std::string const & get_reason() const
        {
            return token_buffer[0]->reason;
        }
    };

}
