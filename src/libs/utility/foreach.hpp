#pragma once

namespace gorc {

    template <typename FnT>
    void foreach(FnT &)
    {
        return;
    }

    template <typename FnT, typename HeadT, typename ...TailT>
    void foreach(FnT &fn, HeadT const &head, TailT const &...tail)
    {
        fn(head);
        foreach(fn, tail...);
    }

}
