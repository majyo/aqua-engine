//
// Created by lihw on 26/12/22.
//

#include "application.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    aqua::Application app{};

    try {
        app.run();
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}