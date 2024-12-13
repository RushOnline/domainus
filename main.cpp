#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <list>

#include <tgbot/tgbot.h>
#include <fmt/core.h>

#include "utils.h"
#include "Application.h"

int main(int argc, char* argv[]) {

    auto &app = Application::getInstance();
    app.init(argc, argv);
    return app.run();
}
