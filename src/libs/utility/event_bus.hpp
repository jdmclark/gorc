#pragma once

#include <typeindex>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include "maybe.hpp"
#include "scoped_assignment.hpp"

namespace gorc {

    class abstract_delegate_container {
    public:
        virtual ~abstract_delegate_container();
        virtual void unregister(size_t id, bool is_const) = 0;
    };

    class scoped_delegate {
    private:
        bool should_unregister;
        abstract_delegate_container *observer;
        size_t id;
        bool is_const;

        void unregister();

    public:
        scoped_delegate(abstract_delegate_container *, size_t, bool);
        scoped_delegate(scoped_delegate const &) = delete;
        scoped_delegate(scoped_delegate &&);
        ~scoped_delegate();

        scoped_delegate& operator=(scoped_delegate const &) = delete;
        scoped_delegate& operator=(scoped_delegate&&);
    };

    class event_bus {
    private:
        template <typename T>
        class delegate_container : public abstract_delegate_container {
        private:
            bool caller_inside = false;
            std::vector<maybe<std::function<void(T&)>>> handlers;
            std::vector<maybe<std::function<void(T const &)>>> const_handlers;

            template <typename FnT, typename FnVecT>
            scoped_delegate insert_internal(FnT fn, FnVecT &vec, bool is_const)
            {
                if(!caller_inside) {
                    // Use space-efficient, non-reentrant registration
                    for(size_t i = 0; i < vec.size(); ++i) {
                        auto &em = vec[i];
                        if(!em.has_value()) {
                            em = fn;
                            return scoped_delegate(this, i, is_const);
                        }
                    }
                }

                // Fall back to space-inefficient but reentrant registration
                size_t next_id = vec.size();
                vec.push_back(fn);
                return scoped_delegate(this, next_id, is_const);
            }

        public:
            virtual void unregister(size_t id, bool is_const) override
            {
                if(is_const) {
                    if(id < const_handlers.size()) {
                        const_handlers[id] = nothing;
                    }
                }
                else {
                    if(id < handlers.size()) {
                        handlers[id] = nothing;
                    }
                }
            }

            scoped_delegate insert(std::function<void(T&)> fn)
            {
                return insert_internal(fn, handlers, false);
            }

            scoped_delegate insert(std::function<void(T const &)> fn)
            {
                return insert_internal(fn, const_handlers, true);
            }

            void dispatch_event(T &event)
            {
                auto caller_inside_guard = make_scoped_assignment(caller_inside, true);

                // Dispatch to normal handlers
                for(size_t i = 0; i < handlers.size(); ++i) {
                    auto &handler = handlers[i];
                    if(handler.has_value()) {
                        handler.get_value()(event);
                    }
                }

                // Dispatch to const handlers
                for(size_t i = 0; i < const_handlers.size(); ++i) {
                    auto &handler = const_handlers[i];
                    if(handler.has_value()) {
                        handler.get_value()(event);
                    }
                }
            }

            void dispatch_event(T const &event)
            {
                if(!handlers.empty()) {
                    throw std::runtime_error("fatal: dispatched constant event when "
                                             "non-constant handlers are registered");
                }

                auto caller_inside_guard = make_scoped_assignment(caller_inside, true);

                for(size_t i = 0; i < const_handlers.size(); ++i) {
                    auto &handler = const_handlers[i];
                    if(handler.has_value()) {
                        handler.get_value()(event);
                    }
                }
            }
        };

        std::unordered_map<std::type_index, std::unique_ptr<abstract_delegate_container>> handlers;

        template <typename T, typename RealT = typename std::decay<T>::type>
        delegate_container<RealT>& get_handler()
        {
            auto it = handlers.find(typeid(RealT));
            if(it == handlers.end()) {
                it = handlers.emplace(typeid(RealT),
                                      std::make_unique<delegate_container<RealT>>()).first;
            }

            return *reinterpret_cast<delegate_container<RealT>*>(it->second.get());
        }

    public:
        template <typename T>
        void fire_event(T &event)
        {
            get_handler<T>().dispatch_event(event);
        }

        template <typename T>
        void fire_event(T const &event)
        {
            get_handler<T>().dispatch_event(event);
        }

        template <typename T>
        scoped_delegate add_handler(std::function<void(T&)> handler)
        {
            return get_handler<T>().insert(handler);
        }

        template <typename T>
        scoped_delegate add_handler(std::function<void(T const &)> handler)
        {
            return get_handler<T>().insert(handler);
        }
    };

}
