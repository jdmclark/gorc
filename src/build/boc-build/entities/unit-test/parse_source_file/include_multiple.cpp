#include "foo.hpp"
#include "dir1/foo.hpp"
#include <stdio.h>

void some_non_include_function()
{
#ifdef FOOBAR
    printf("Hello!\n");
#endif
}

#include "dir2/foo.hpp"
#pragma once
#include "bar.hpp"
