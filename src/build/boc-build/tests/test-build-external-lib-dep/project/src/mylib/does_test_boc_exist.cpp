#include <iostream>
#include <cstdlib>
#include <boost/filesystem.hpp>

bool does_test_boc_exist()
{
    return boost::filesystem::is_regular_file("project.json");
}
