#include "program/program.hpp"
#include "io/std_input_stream.hpp"
#include "jk/vfs/episode.hpp"
#include <iostream>
#include <iomanip>

namespace gorc {

    class episode_program : public program {
    public:
        virtual void create_options(options &) override
        {
            return;
        }

        std::string consequence(int value)
        {
            if(value >= 0) {
                return str(format("goto line %d") % value);
            }
            else {
                return "game over";
            }
        }

        virtual int run() override
        {
            diagnostic_context dc("stdin");
            std_input_stream ss;

            episode ep(ss);

            // Print info
            std::cout << "Episode information" << std::endl;
            std::cout << "    Name: " << ep.get_episode_name() << std::endl;
            std::cout << "    Type: " << static_cast<int>(ep.get_type()) << std::endl;
            std::cout << "    Lines: " << ep.size() << std::endl;

            for(auto const &line : ep) {
                std::cout << std::endl;
                std::cout << "    Line " << line.number << ":" << std::endl;
                std::cout << "        Disc: " << line.disc << std::endl;
                std::cout << "        Level: " << line.level_number << std::endl;
                std::cout << "        Type: " << as_string(line.type) << std::endl;
                std::cout << "        File: " << line.filename.generic_string() << std::endl;

                if(line.light_power != 0) {
                    std::cout << "        Light power: " << line.light_power << std::endl;
                }

                if(line.dark_power != 0) {
                    std::cout << "        Dark power: " << line.dark_power << std::endl;
                }

                switch(line.type) {
                case episode_entry_type::cutscene:
                case episode_entry_type::level:
                    std::cout << "        On success: " << consequence(line.goto_a) << std::endl;
                    std::cout << "        On failure: " << consequence(line.goto_b) << std::endl;
                    break;

                case episode_entry_type::decision:
                    std::cout << "        If good: " << consequence(line.goto_a) << std::endl;
                    std::cout << "        If evil: " << consequence(line.goto_b) << std::endl;
                    break;
                }
            }

            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::episode_program)
