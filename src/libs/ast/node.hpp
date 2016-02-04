#pragma once

#include "log/diagnostic_context_location.hpp"
#include "log/diagnostic_context.hpp"
#include "utility/variant.hpp"
#include <vector>

namespace gorc {

    class ast_node {
    public:
        diagnostic_context_location location;

        ast_node(diagnostic_context_location const &location);
        virtual ~ast_node();
    };

    template <typename SelfT>
    class visitable_ast_node : public ast_node {
    public:
        using ast_node::ast_node;

        virtual ~visitable_ast_node()
        {
            return;
        }

        template <typename VisitorT>
        auto accept(VisitorT &v)
            -> decltype(v.visit(*reinterpret_cast<SelfT*>(this)))
        {
            return v.visit(*reinterpret_cast<SelfT*>(this));
        }

        template <typename VisitorT>
        auto accept(VisitorT const &v)
            -> decltype(v.visit(*reinterpret_cast<SelfT*>(this)))
        {
            return v.visit(*reinterpret_cast<SelfT*>(this));
        }
    };

    template <typename ElementT>
    class ast_list_node : public visitable_ast_node<ast_list_node<ElementT>> {
    public:
        std::vector<ElementT> elements;

        ast_list_node(diagnostic_context_location const &location)
            : visitable_ast_node<ast_list_node<ElementT>>(location)
        {
            return;
        }

        ast_list_node(diagnostic_context_location const &location, ElementT const &first)
            : visitable_ast_node<ast_list_node<ElementT>>(location)
        {
            elements.push_back(first);
        }
    };

    template <typename VisitorT, typename EmT>
    auto ast_visit(VisitorT &v, EmT *n)
        -> decltype(v.visit(*n))
    {
        diagnostic_context dc(n->location);
        return n->accept(v);
    }

    template <typename VisitorT, typename EmT>
    auto ast_visit(VisitorT const &v, EmT *n)
        -> decltype(v.visit(*n))
    {
        diagnostic_context dc(n->location);
        return n->accept(v);
    }

    template <typename VisitorT>
    class variant_ast_node_visitor {
    public:
        VisitorT &v;

        variant_ast_node_visitor(VisitorT &v)
            : v(v)
        {
            return;
        }

        template <typename EmT>
        auto visit(EmT &em) const
            -> decltype(::gorc::ast_visit(v, em))
        {
            return ::gorc::ast_visit(v, em);
        }
    };

    template <typename VisitorT>
    class variant_ast_node_const_visitor {
    public:
        VisitorT const &v;

        variant_ast_node_const_visitor(VisitorT const &v)
            : v(v)
        {
            return;
        }

        template <typename EmT>
        auto visit(EmT &em) const
            -> decltype(::gorc::ast_visit(v, em))
        {
            return ::gorc::ast_visit(v, em);
        }
    };

    template <typename VisitorT, typename ...EmT>
    auto ast_visit(VisitorT &v, variant<EmT...> &n)
        -> decltype(n.accept(variant_ast_node_visitor<VisitorT>(v)))
    {
        return n.accept(variant_ast_node_visitor<VisitorT>(v));
    }

    template <typename VisitorT, typename ...EmT>
    auto ast_visit(VisitorT const &v, variant<EmT...> &n)
        -> decltype(n.accept(variant_ast_node_const_visitor<VisitorT>(v)))
    {
        return n.accept(variant_ast_node_const_visitor<VisitorT>(v));
    }

}
