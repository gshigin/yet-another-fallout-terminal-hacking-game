// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

struct terminal_layout
{
    static constexpr int width = 53;

    struct attempts_window
    {
        static constexpr int width = terminal_layout::width;
        static constexpr int height = 5;
    };

    struct interaction_window // vbox
    {
        static constexpr int height = 17;
        struct hex_window_1
        {
            static constexpr int width = 6;
            static constexpr int height = interaction_window::height;
        };
        // space
        struct interactive_window_1
        {
            static constexpr int width = 12;
            static constexpr int height = interaction_window::height;
        };
        // space
        struct hex_window_2
        {
            static constexpr int width = 6;
            static constexpr int height = interaction_window::height;
        };
        // space
        struct interactive_window_2
        {
            static constexpr int width = 12;
            static constexpr int height = interaction_window::height;
        };
        // space
        struct log_window
        {
            static constexpr int width = 13;
            static constexpr int height = interaction_window::height;
        };

        static constexpr int width = hex_window_1::width + interactive_window_1::width + hex_window_2::width +
                                     interactive_window_2::width + log_window::width + 4 /* * space::width*/;
        static_assert(terminal_layout::width == interaction_window::width);
    };
    static constexpr int height = attempts_window::height + interaction_window::height;
};
