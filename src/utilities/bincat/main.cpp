#include "program/program.hpp"
#include "io/binary_output_stream.hpp"
#include "io/std_output_stream.hpp"
#include "log/log.hpp"
#include <iostream>
#include <string>

namespace gorc {

    class bincat_program : public program {
    public:
        virtual void create_options(options &) override
        {
            return;
        }

        virtual int run() override
        {
            std_output_stream os;
            os.reopen_as_binary();

            binary_output_stream bos(os);

            process_script(std::cin, bos);

            return EXIT_SUCCESS;
        }

        void process_script(std::istream &is, binary_output_stream &bos)
        {
            for(;;) {
                std::string directive;
                is >> directive;

                if(!is) {
                    return;
                }

                dispatch_directive(directive, is, bos);
            }
        }

        void dispatch_directive(std::string const &directive,
                                std::istream &is,
                                binary_output_stream &bos)
        {
            if(directive.at(0) == '#') {
                // Line comment
                is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
            else if(directive == "raw") {
                std::string value = get_escaped_string(is);
                bos.write(value.data(), value.size());
                return;
            }
            else if(directive == "uint8") {
                handle_numeric<uint8_t>(is, bos);
                return;
            }
            else if(directive == "uint8h") {
                handle_numeric_hex<uint8_t>(is, bos);
                return;
            }
            else if(directive == "uint16") {
                handle_numeric<uint16_t>(is, bos);
                return;
            }
            else if(directive == "uint16h") {
                handle_numeric_hex<uint16_t>(is, bos);
                return;
            }
            else if(directive == "uint32") {
                handle_numeric<uint32_t>(is, bos);
                return;
            }
            else if(directive == "uint32h") {
                handle_numeric_hex<uint32_t>(is, bos);
                return;
            }
            else {
                LOG_FATAL(format("unknown directive '%s'") % directive);
            }
        }

        template <typename T>
        void handle_numeric(std::istream &is, binary_output_stream &bos) {
            int64_t value;
            is >> value;

            if(!is) {
                LOG_FATAL("expected number");
            }

            binary_serialize(bos, static_cast<T>(value));
        }

        template <typename T>
        void handle_numeric_hex(std::istream &is, binary_output_stream &bos) {
            int64_t value;
            is >> std::hex >> value >> std::dec;

            if(!is) {
                LOG_FATAL("expected hex number");
            }

            binary_serialize(bos, static_cast<T>(value));
        }

        std::string get_escaped_string(std::istream &is)
        {
            // Ignore whitespace immediately following directive
            is.ignore(1);

            std::string rv;
            for(;;) {
                char ch;
                is.get(ch);

                if(!is) {
                    LOG_FATAL("expected $ at end of string");
                    break;
                }

                if(ch == '$') {
                    char ch2;
                    is.get(ch2);

                    if(is && ch2 == '$') {
                        // Escaped $
                        rv.push_back('$');
                        continue;
                    }

                    // End of string
                    is.unget();
                    break;
                }

                rv.push_back(ch);
            }

            return rv;
        }
    };

}

MAKE_MAIN(gorc::bincat_program)
