//
// Created by Kacper Trzciński on 13.01.2025.
//

#include <spdlog/spdlog.h>
#include <iostream>

int main()
{
    std::cout << "starting\n";
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Hello World!");
    std::cout << "Made it past spdlog\n" << std::endl;

    return 0;
}