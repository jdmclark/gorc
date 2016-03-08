#include <iostream>

#include "application.hpp"
#include "utility/service_registry.hpp"
#include "vfs/jk_virtual_file_system.hpp"

int main(int argc, char** argv) {
    gorc::jk_virtual_file_system fs("game/resource", "game/restricted");

    gorc::service_registry services;
    services.add<gorc::virtual_file_system>(fs);
    services.add<gorc::jk_virtual_file_system>(fs);

    gorc::client::application app(services);
    app.start(gorc::make_range(argv + 1, argv + argc));
    return 0;
}
