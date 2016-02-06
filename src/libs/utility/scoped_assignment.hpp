#pragma once

#include "maybe.hpp"

namespace gorc {

    template <typename VarT>
    class [[gnu::unused]] scoped_assignment {
    private:
        maybe<VarT*> var;
        VarT original_value;

        void restore()
        {
            maybe_if(var, [&](VarT *v) { *v = original_value; });
        }

    public:
        scoped_assignment(VarT &var, VarT new_value)
            : var(&var)
            , original_value(var)
        {
            var = new_value;
            return;
        }

        scoped_assignment(scoped_assignment const &) = delete;

        scoped_assignment(scoped_assignment &&v)
            : var(v.var)
            , original_value(v.original_value)
        {
            v.var = nothing;
            return;
        }

        ~scoped_assignment()
        {
            restore();
            return;
        }

        scoped_assignment& operator=(scoped_assignment const &) = delete;

        scoped_assignment& operator=(scoped_assignment &&v)
        {
            restore();
            var = v.var;
            original_value = v.original_value;
            v.var = nothing;
            return *this;
        }
    };

    template <typename VarT>
    scoped_assignment<VarT> make_scoped_assignment(VarT &var, VarT orig)
    {
        return scoped_assignment<VarT>(var, orig);
    }

}
