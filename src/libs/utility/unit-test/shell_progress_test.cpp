#include "test/test.hpp"
#include "utility/shell_progress.hpp"
#include "../zip.hpp"
#include <sstream>

using namespace gorc;

begin_suite(shell_shell_progress_test);

test_case(simple)
{
    std::stringstream ss;
    shell_progress prog(5, 12, ss);

    std::string expected = "\r◐ ▫▫▫▫▫▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◓ ▪▫▫▫▫▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◑ ▪▪▪▫▫▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◒ ▪▪▪▪▪▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◐ ▪▪▪▪▪▪▪▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◓ ▪▪▪▪▪▪▪▪▪";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.finished();

    expected = "\r● ▪▪▪▪▪▪▪▪▪\n";
    assert_eq(expected, ss.str());
}

test_case(beyond)
{
    std::stringstream ss;
    shell_progress prog(5, 12, ss);

    std::string expected = "\r◐ ▫▫▫▫▫▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance(5);

    expected = "\r◓ ▪▪▪▪▪▪▪▪▪";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◑ ▪▪▪▪▪▪▪▪▪";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◒ ▪▪▪▪▪▪▪▪▪";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.finished();

    expected = "\r● ▪▪▪▪▪▪▪▪▪\n";
    assert_eq(expected, ss.str());
}

test_case(incomplete)
{
    std::stringstream ss;
    shell_progress prog(5, 12, ss);

    std::string expected = "\r◐ ▫▫▫▫▫▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance(3);

    expected = "\r◒ ▪▪▪▪▪▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.finished();

    expected = "\r● ▪▪▪▪▪▫▫▫▫\n";
    assert_eq(expected, ss.str());
}

test_case(zero)
{
    std::stringstream ss;
    shell_progress prog(0, 12, ss);

    std::string expected = "\r◐ ▫▫▫▫▫▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.advance();

    expected = "\r◓ ▫▫▫▫▫▫▫▫▫";
    assert_eq(expected, ss.str());

    ss.str(std::string());
    ss.clear();
    prog.finished();

    expected = "\r● ▫▫▫▫▫▫▫▫▫\n";
    assert_eq(expected, ss.str());
}

end_suite(shell_shell_progress_test);
