#include <iostream>
#include "color_correction_application.hpp"

int main(int argc, char* argv[])
{
    auto args = ColorCorrectionApplication::parseArguments(argc, argv);
    if (!args.has_value()) {
        ColorCorrectionApplication app;
        app.printUsage(argv[0]);
        return -1;
    }
    
    ColorCorrectionApplication app;
    return app.run(args.value());
}