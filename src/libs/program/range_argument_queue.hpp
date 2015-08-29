#pragma once

#include "abstract_argument_queue.hpp"

namespace gorc {

    template <typename ItT>
    class range_argument_queue : public abstract_argument_queue {
    private:
        ItT range_begin, range_end;

    public:
        range_argument_queue(ItT const &range_begin, ItT const &range_end)
            : range_begin(range_begin)
            , range_end(range_end)
        {
            return;
        }

        virtual bool empty() const override
        {
            return range_begin == range_end;
        }

        virtual std::string peek() const override
        {
            return static_cast<std::string>(*range_begin);
        }

        virtual void pop() override
        {
            ++range_begin;
        }
    };

    template <typename ItT>
    range_argument_queue<ItT> make_range_argument_queue(ItT const &range_begin,
                                                        ItT const &range_end)
    {
        return range_argument_queue<ItT>(range_begin, range_end);
    }

}
