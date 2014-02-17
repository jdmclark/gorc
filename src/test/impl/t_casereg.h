#pragma once

namespace test {

typedef std::vector<std::pair<std::string, std::shared_ptr<case_factory>>> case_vector;
typedef std::unordered_map<std::string, std::shared_ptr<case_factory>> case_map;

template <typename T> class case_registry : public singleton< case_registry<T> > {
    friend class singleton< case_registry<T> >;

private:
    case_registry() : locked(false) {
        return;
    }

public:
    bool locked;
    case_vector factories;
    case_map factory_map;
};

template <typename T, typename C> void register_case(const std::string& s,
    const std::string& fileName, int lineNumber) {
    case_registry<T>& reg = case_registry<T>::get();

    if(!reg.locked) {
        auto pair = std::make_pair(s, std::make_shared<specified_case_factory<C>>(fileName, lineNumber));

        reg.factories.push_back(pair);
        reg.factory_map.insert(pair);
    }

    return;
}

template <typename T> void lock_case_registry() {
    case_registry<T>::get().locked = true;
}

}
