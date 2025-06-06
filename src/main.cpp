// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include <yafth/app/application.h>
#include <yafth/util/args.h>
// stl
#include <iostream>

int main(int argc, char *argv[])
{
    yafth::args arguments{};
    try
    {
        arguments = yafth::util::argparser::parse(argc, argv);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    yafth::app::application app(arguments);
    app.run();

    return 0;
}
