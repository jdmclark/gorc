#pragma once

#include "virtual_file_system.hpp"

namespace gorc {

    class native_file_system : public virtual_file_system {
    private:
        path const base_path;

    public:
        native_file_system();
        native_file_system(path const &base_path);

        virtual std::unique_ptr<input_stream> open(path const &filename) const override;
        virtual std::tuple<path, std::unique_ptr<input_stream>>
            find(path const &filename, std::vector<path> const &prefixes) const override;
    };

}
