#include "test/test.h"
#include "mock_reporter.h"

namespace test_test {

class framework_test_fixture : public test::fixture {
protected:
    std::shared_ptr<test::reporter> originalReporter;
    std::shared_ptr<mock_reporter> mockReporter;

public:
    framework_test_fixture()
        : originalReporter(test::test_reporter), mockReporter(std::make_shared<mock_reporter>()) {
        return;
    }
};

begin_suite_fixture(framework_test, framework_test_fixture);

test_case(empty) {
}

test_case(verify_names) {
    expect_eq(::test::test_suite_name, std::string("framework_test"));
    expect_eq(::test::test_case_name, std::string("verify_names"));
}

end_suite(framework_test);

}
