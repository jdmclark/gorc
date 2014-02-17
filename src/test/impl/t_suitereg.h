#pragma once

namespace test {

class suite_factory;

typedef std::unordered_map<std::string, std::shared_ptr<suite_factory>> suite_map;

class suite_registry : public singleton<suite_registry> {
    friend class singleton<suite_registry>;

private:
    suite_registry() {
        return;
    }

public:
    suite_map factories;
};

template <typename T> void register_suite(const std::string& s,
    const std::string& fileName, int lineNumber) {
    suite_registry::get().factories.insert(
        std::make_pair(s, std::make_shared<specified_suite_factory<T>>(
            fileName, lineNumber)));
    return;
}

}
