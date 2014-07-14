#include "test/test.h"
#include "cog/compiler.h"
#include "base/io/memory_file.h"
#include "base/diagnostics/stored_report.h"
#include "base/io/exception.h"
#include "cog/scripts/script_presenter.h"
#include "cog/vm/virtual_machine.h"
#include "cog/instance.h"

using namespace gorc::cog;

begin_suite(compiler_test);

test_case(syntax_error) {
    std::string content =
        "symbols \
         end \
         code \
         if() { return; } \
         end";

    gorc::diagnostics::stored_report report;
    verbs::verb_table verbs;
    gorc::cog::compiler compiler(verbs);
    script output;

    gorc::io::memory_file mf;
    mf.write(content.data(), content.size());
    mf.set_position(0);

    try {
        compiler.compile("filename", mf, output, report);
    }
    catch(gorc::io::file_corrupt_exception &) {
        return;
    }

    assert_always("exception not thrown");
}

test_case(semantic_error) {
    std::string content =
        "symbols \
         message startup=0\n\
         end \
         code \
         end";

    gorc::diagnostics::stored_report report;
    verbs::verb_table verbs;
    gorc::cog::compiler compiler(verbs);
    script output;

    gorc::io::memory_file mf;
    mf.write(content.data(), content.size());
    mf.set_position(0);

    try {
        compiler.compile("filename", mf, output, report);
    }
    catch(gorc::io::file_corrupt_exception &) {
        return;
    }

    assert_always("exception not thrown");
}

test_case(call_code) {
    std::string content =
        "symbols \
         end \
         code \
         startup: \
         some_fn(); \
         return; \
         end";

    gorc::diagnostics::stored_report report;
    verbs::verb_table verbs;

    bool called = false;
    verbs.add_verb<void, 0>("some_fn", [&]() {
        called = true;
    });

    gorc::cog::compiler compiler(verbs);
    script output;

    gorc::io::memory_file mf;
    mf.write(content.data(), content.size());
    mf.set_position(0);

    compiler.compile("filename", mf, output, report);

    vm::virtual_machine vm_instance;

    instance inst(output);

    inst.call(verbs, vm_instance, message_id::startup, nullptr);

    assert_true(called);
}

end_suite(compiler_test);
