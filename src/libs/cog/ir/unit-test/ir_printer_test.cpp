#include "test/test.hpp"
#include "cog/ir/ir_printer.hpp"
#include "io/memory_file.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(ir_printer_test);

test_case(label_redefinition)
{
    memory_file mf;
    message_table mt;
    ir_printer ir(mf, mt);

    auto lid = ir.generate_label();
    ir.label(lid);

    assert_log_empty();
    assert_throws_logged(ir.label(lid));
    assert_log_message(log_level::error, "label id 0 used for multiple branch targets");
    assert_log_empty();
}

end_suite(ir_printer_test);
