#include <print>
#include <string>

#include "pch.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::println("\033[0;31mToo few parameters\033[0m");
        return -1;
    }

    return 0;
}