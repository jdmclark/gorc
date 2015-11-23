#include "program/program.hpp"
#include "io/std_input_stream.hpp"
#include "io/std_output_stream.hpp"
#include "system/process.hpp"
#include "system/pipe.hpp"

class parent_program : public gorc::program {
public:
    std::string prog_to_run;
    std::vector<std::string> extra_args;

    virtual void create_options(gorc::options &opts) override
    {
        opts.insert(gorc::make_value_option("run", prog_to_run));
        opts.insert(gorc::make_multi_value_option("extra", std::back_inserter(extra_args)));
        opts.emplace_constraint<gorc::required_option>("run");
    }

    virtual int main() override
    {
        gorc::std_input_stream std_input_stream;
        gorc::std_output_stream std_output_stream;

        std::cout << "= executing " << prog_to_run << std::endl;

        gorc::pipe std_input;
        gorc::pipe std_output;
        gorc::pipe std_error;

        gorc::process child;
        child.redirect_standard_input(std::move(std_input.input));
        child.redirect_standard_output(std::move(std_output.output));
        child.redirect_standard_error(std::move(std_error.output));

        child.execute(prog_to_run, extra_args);

        std_input_stream.copy_to(*std_input.output);
        std_input.output.reset();

        std::cout << "==== stdout ====" << std::endl;
        std_output.input->copy_to(std_output_stream);
        std::cout << std::endl;

        std::cout << "==== stderr ====" << std::endl;
        std_error.input->copy_to(std_output_stream);
        std::cout << std::endl;

        int child_result = child.join();

        std::cout << "= exit code " << child_result << std::endl;


        return EXIT_SUCCESS;
    }
};

MAKE_MAIN(parent_program)
