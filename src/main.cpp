#include <fmt/printf.h>
#include <string>

#include "pch.hpp"

int main(int argc, char **argv)
{
    argc = 2;
    if (argc < 2)
    {
        fmt::println("\033[0;31mToo few parameters\033[0m");
        return -1;
    }

    return 0;
}