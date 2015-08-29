#pragma once

#include "log/diagnostic_context_location.hpp"
#include "log/diagnostic_context.hpp"
#include "utility/variant.hpp"
#include <vector>

namespace gorc {
    namespace cog {
        namespace ast {

            class node {
            public:
                diagnostic_context_location location;

                node(diagnostic_context_location const &location);
                virtual ~node();
            };

            template <typename SelfT>
            class visitable_node : public node {
            public:
                using node::node;

                virtual ~visitable_node()
                {
                    return;
                }

                template <typename VisitorT>
                auto accept(VisitorT &v)
                    -> decltype(v.visit(*static_cast<SelfT*>(this)))
                {
                    return v.visit(*static_cast<SelfT*>(this));
                }

                template <typename VisitorT>
                auto accept(VisitorT const &v)
                    -> decltype(v.visit(*static_cast<SelfT*>(this)))
                {
                    return v.visit(*static_cast<SelfT*>(this));
                }
            };

            template <typename ElementT>
            class list_node : public visitable_node<list_node<ElementT>> {
            public:
                std::vector<ElementT> elements;

                list_node(diagnostic_context_location const &location)
                    : visitable_node<list_node<ElementT>>(location)
                {
                    return;
                }

                list_node(diagnostic_context_location const &location, ElementT const &first)
                    : visitable_node<list_node<ElementT>>(location)
                {
                    elements.push_back(first);
                }
            };

            template <typename VisitorT, typename EmT>
            auto visit(VisitorT &v, EmT *n)
                -> decltype(v.visit(*n))
            {
                diagnostic_context dc(n->location);
                return n->accept(v);
            }

            template <typename VisitorT, typename EmT>
            auto visit(VisitorT const &v, EmT *n)
                -> decltype(v.visit(*n))
            {
                diagnostic_context dc(n->location);
                return n->accept(v);
            }

            template <typename VisitorT>
            class variant_node_visitor {
            public:
                VisitorT &v;

                variant_node_visitor(VisitorT &v)
                    : v(v)
                {
                    return;
                }

                template <typename EmT>
                auto visit(EmT &em) const
                    -> decltype(::gorc::cog::ast::visit(v, em))
                {
                    return ::gorc::cog::ast::visit(v, em);
                }
            };

            template <typename VisitorT>
            class variant_node_const_visitor {
            public:
                VisitorT const &v;

                variant_node_const_visitor(VisitorT const &v)
                    : v(v)
                {
                    return;
                }

                template <typename EmT>
                auto visit(EmT &em) const
                    -> decltype(::gorc::cog::ast::visit(v, em))
                {
                    return ::gorc::cog::ast::visit(v, em);
                }
            };

            template <typename VisitorT, typename ...EmT>
            auto visit(VisitorT &v, variant<EmT...> &n)
                -> decltype(n.accept(variant_node_visitor<VisitorT>(v)))
            {
                return n.accept(variant_node_visitor<VisitorT>(v));
            }

            template <typename VisitorT, typename ...EmT>
            auto visit(VisitorT const &v, variant<EmT...> &n)
                -> decltype(n.accept(variant_node_const_visitor<VisitorT>(v)))
            {
                return n.accept(variant_node_const_visitor<VisitorT>(v));
            }
        }
    }
}
