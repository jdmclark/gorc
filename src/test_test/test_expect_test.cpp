#include "test/test.h"
#include "mock_reporter.h"

namespace test_test {

class expect_test_fixture : public test::fixture
{
protected:
    std::shared_ptr<test::reporter> originalReporter;
    std::shared_ptr<mock_reporter> mockReporter;

public:
    expect_test_fixture() {
        originalReporter = test::test_reporter;
        mockReporter = std::make_shared<mock_reporter>();
    }
};

begin_suite_fixture(expect_test, expect_test_fixture);

test_case(base) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_true(false);

    test::test_reporter = originalReporter;

    expect_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 1);
}

test_case(always) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_always("Meaningful message");

    test::test_reporter = originalReporter;

    assert_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 1);
}

test_case(eq) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_eq(1, 0);
    expect_eq(1, 1);

    test::test_reporter = originalReporter;

    assert_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 1);
}

test_case(gt) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_gt(0, 1);
    expect_gt(1, 0);
    expect_gt(0, 0);

    test::test_reporter = originalReporter;

    assert_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 2);
}

test_case(lt) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_lt(1, 0);
    expect_lt(0, 1);
    expect_lt(0, 0);

    test::test_reporter = originalReporter;

    assert_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 2);
}

test_case(ge) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_ge(0, 1);
    expect_ge(1, 0);
    expect_ge(1, 1);

    test::test_reporter = originalReporter;

    assert_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 1);
}

test_case(le) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_le(1, 0);
    expect_le(0, 1);
    expect_le(1, 1);

    test::test_reporter = originalReporter;

    assert_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 1);
}

test_case(ne) {
    // Exchange out the original test reporter.
    test::test_reporter = mockReporter;

    expect_ne(1, 1);
    expect_ne(1, 0);

    test::test_reporter = originalReporter;

    assert_true(!test::test_passing);
    test::test_passing = true;

    assert_eq(mockReporter->case_expectation_fail_called, 1);
}

end_suite(expect_test);

}
