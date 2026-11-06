// Copyright 2026 Gleb Shigin. All rights reserved.
// SPDX-License-Identifier: MIT
// See LICENSE.
#define YAFTH_IMPLEMENTATION
#include "yafth.h"

#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

namespace {
constexpr std::string_view usage = "Usage: yafth [options]\n"
                                   "  -h, --help             Show this help message\n"
                                   "  -s, --science VALUE    Science skill (0-100)\n"
                                   "  -l, --lock VALUE       Difficulty (0-4 or very_easy, easy, average, hard, very_hard)\n"
                                   "  --seed VALUE           Unsigned 64-bit seed\n"
                                   "Use the mouse to select entries; Escape or q exits.\n";

struct options {
    yafth_difficulty difficulty = YAFTH_AVERAGE;
    std::uint32_t science = 50;
    std::uint64_t seed = 0;
    bool help = false;
};

enum class option_error_code : std::uint8_t {
    unknown_argument,
    missing_value,
    duplicate_science,
    invalid_science,
    duplicate_lock,
    invalid_lock,
    duplicate_seed,
    invalid_seed
};

struct option_error {
    option_error_code code;
    std::string_view argument{}; // Option name borrowed from argv.
};

enum class option_kind : std::uint8_t {
    science,
    lock,
    seed
};

auto classify_option(std::string_view arg) noexcept -> std::optional<option_kind> {
    if (arg == "-s" || arg == "--science") {
        return option_kind::science;
    }
    if (arg == "-l" || arg == "--lock") {
        return option_kind::lock;
    }
    if (arg == "--seed") {
        return option_kind::seed;
    }
    return {};
}

template <class T> auto parse_number(std::string_view text) noexcept -> std::optional<T> {
    T value{};
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        return {};
    }
    return value;
}

auto parse_options(int argc, char const *const *argv) noexcept -> std::variant<options, option_error> {
    options result;
    std::optional<std::uint32_t> science;
    std::optional<yafth_difficulty> difficulty;
    std::optional<std::uint64_t> seed;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            result.help = true;
            return result;
        }
        const auto kind = classify_option(arg);
        if (!kind) {
            return option_error{option_error_code::unknown_argument, arg};
        }
        if (++i == argc) {
            return option_error{option_error_code::missing_value, arg};
        }
        const std::string_view value = argv[i];
        switch (*kind) {
        case option_kind::science: {
            if (science) {
                return option_error{option_error_code::duplicate_science};
            }
            science = parse_number<std::uint32_t>(value);
            if (!science) {
                return option_error{option_error_code::invalid_science};
            }
            break;
        }
        case option_kind::lock: {
            if (difficulty) {
                return option_error{option_error_code::duplicate_lock};
            }
            constexpr std::array<std::string_view, 5> names{"very_easy", "easy", "average", "hard", "very_hard"};
            const auto found = std::find(names.begin(), names.end(), value);
            const auto number = found == names.end() ? parse_number<std::uint32_t>(value) : static_cast<std::uint32_t>(found - names.begin());
            if (!number || *number > YAFTH_VERY_HARD) {
                return option_error{option_error_code::invalid_lock};
            }
            difficulty = static_cast<yafth_difficulty>(*number);
            break;
        }
        case option_kind::seed: {
            if (seed) {
                return option_error{option_error_code::duplicate_seed};
            }
            seed = parse_number<std::uint64_t>(value);
            if (!seed) {
                return option_error{option_error_code::invalid_seed};
            }
            break;
        }
        }
    }

    if (difficulty) {
        result.difficulty = *difficulty;
        result.science = science.value_or(*difficulty == YAFTH_VERY_EASY ? 15u : 25u * static_cast<unsigned>(*difficulty));
    } else if (science) {
        result.science = *science;
        result.difficulty = static_cast<yafth_difficulty>(std::min<unsigned>(YAFTH_VERY_HARD, *science / 25));
    }
    result.seed = seed.value_or(static_cast<std::uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()));
    return result;
}

void print_option_error(const option_error &error) {
    std::cerr << "Error: ";
    switch (error.code) {
    case option_error_code::unknown_argument: {
        std::cerr << "Unknown argument";
        break;
    }
    case option_error_code::missing_value: {
        std::cerr << "Missing value for";
        break;
    }
    case option_error_code::duplicate_science: {
        std::cerr << "--science specified multiple times";
        break;
    }
    case option_error_code::invalid_science: {
        std::cerr << "--science must be an unsigned integer";
        break;
    }
    case option_error_code::duplicate_lock: {
        std::cerr << "--lock specified multiple times";
        break;
    }
    case option_error_code::invalid_lock: {
        std::cerr << "--lock must be 0-4 or a difficulty name";
        break;
    }
    case option_error_code::duplicate_seed: {
        std::cerr << "--seed specified multiple times";
        break;
    }
    case option_error_code::invalid_seed: {
        std::cerr << "--seed must be an unsigned 64-bit integer";
        break;
    }
    }
    if (!error.argument.empty()) {
        std::cerr << ' ' << error.argument;
    }
    std::cerr << '\n';
}

constexpr int column_rows = YAFTH_BUFFER_SIZE / YAFTH_ROW_WIDTH / 2;
constexpr std::size_t log_rows = column_rows - 2;
constexpr int log_width = YAFTH_ROW_WIDTH + 1;
// Store the full score line. The log panel clips it to log_width.
constexpr std::size_t log_text_capacity = std::max(std::size_t{YAFTH_ROW_WIDTH}, std::string_view{"00/00 correct."}.size());
static_assert(column_rows * YAFTH_ROW_WIDTH * 2 == YAFTH_BUFFER_SIZE);
static_assert(column_rows > 2);

struct terminal_log {
    std::array<std::array<char, log_text_capacity + 2>, log_rows> lines{}; // Include '>' and '\0'.
    std::size_t first = 0;
    std::size_t size = 0;
};

void append_log(terminal_log &log, std::string_view text) {
    auto &line = log.lines[(log.first + log.size) % log_rows];
    line.front() = '>';
    const auto clipped = text.substr(0, log_text_capacity);
    const auto end = std::copy(clipped.begin(), clipped.end(), line.begin() + 1);
    *end = '\0';

    if (log.size < log_rows) {
        ++log.size;
    } else {
        log.first = (log.first + 1) % log_rows;
    }
}

struct terminal_view {
    std::uint32_t address;
    std::array<char, YAFTH_BUFFER_SIZE> field{};
    std::array<ftxui::Box, 2> columns{};
    std::optional<yafth_range> focus{};
    terminal_log log{};
};

auto memory_column(terminal_view &view, std::size_t column) -> ftxui::Element {
    ftxui::Elements addresses;
    ftxui::Elements rows;
    for (int row = 0; row < column_rows; ++row) {
        const auto offset = static_cast<std::uint32_t>((column * column_rows + row) * YAFTH_ROW_WIDTH);
        std::array<char, 7> hex{};
        std::snprintf(hex.data(), hex.size(), "0x%04X", static_cast<unsigned>((view.address + offset) & 0xffffu));
        addresses.push_back(ftxui::text(hex.data()));

        const std::string_view line(view.field.data() + offset, YAFTH_ROW_WIDTH);
        const auto row_end = offset + YAFTH_ROW_WIDTH;
        if (!view.focus || view.focus->end <= offset || view.focus->begin >= row_end) {
            rows.push_back(ftxui::text(line));
            continue;
        }
        // Clip highlighted rows cell by cell so a narrow terminal keeps the highlight aligned.
        ftxui::Elements cells;
        for (auto index = offset; index < row_end; ++index) {
            auto cell = ftxui::text(line.substr(index - offset, 1));
            if (view.focus->begin <= index && index < view.focus->end) {
                cell = std::move(cell) | ftxui::inverted;
            }
            cells.push_back(std::move(cell));
        }
        rows.push_back(ftxui::hbox(std::move(cells)));
    }
    return ftxui::hbox({ftxui::vbox(std::move(addresses)), ftxui::text(" "), ftxui::vbox(std::move(rows)) | ftxui::reflect(view.columns[column])});
}

auto render_terminal(const yafth_state &state, terminal_view &view) -> ftxui::Element {
    const auto attempts = yafth_attempts_left(&state);
    ftxui::Elements blocks;
    for (std::uint32_t i = 0; i < attempts; ++i) {
        blocks.push_back(ftxui::text(" ") | ftxui::inverted);
        blocks.push_back(ftxui::text(" "));
    }

    const char *message = attempts <= 1 ? "!!! WARNING: LOCKOUT IMMINENT !!!" : "ENTER PASSWORD NOW";
    switch (yafth_status(&state)) {
    case YAFTH_ACTIVE: {
        break;
    }
    case YAFTH_WON: {
        message = "ACCESS GRANTED";
        break;
    }
    case YAFTH_LOCKED: {
        message = "TERMINAL LOCKED";
        break;
    }
    }
    auto header = ftxui::vbox({ftxui::text("ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL"), ftxui::text(message), ftxui::text(""),
                               ftxui::hbox({ftxui::text(std::to_string(attempts) + " ATTEMPT(S) LEFT : "), ftxui::hbox(std::move(blocks))}), ftxui::text("")});

    ftxui::Elements lines;
    for (std::size_t offset = 0; offset < view.log.size; ++offset) {
        const auto &line = view.log.lines[(view.log.first + offset) % log_rows];
        lines.push_back(ftxui::text(line.data()));
    }
    const auto hovered = view.focus ? std::string(view.field.data() + view.focus->begin, view.focus->end - view.focus->begin) : std::string{};
    auto log = ftxui::vbox({ftxui::filler(), ftxui::vbox(std::move(lines)), ftxui::text(""), ftxui::text(">" + hovered)}) |
               ftxui::size(ftxui::WIDTH, ftxui::EQUAL, log_width);
    return ftxui::vbox({std::move(header), ftxui::hbox({memory_column(view, 0), ftxui::text(" "), memory_column(view, 1), ftxui::text(" "), std::move(log)})}) |
           ftxui::border;
}

auto field_position(const terminal_view &view, const ftxui::Mouse &mouse) -> std::optional<std::uint32_t> {
    for (std::size_t column = 0; column < view.columns.size(); ++column) {
        const auto &box = view.columns[column];
        // Map the mouse only when the whole column is visible.
        if (box.x_max - box.x_min + 1 == YAFTH_ROW_WIDTH && box.y_max - box.y_min + 1 == column_rows && box.Contain(mouse.x, mouse.y)) {
            const auto row = static_cast<std::uint32_t>(column * column_rows + (mouse.y - box.y_min));
            return row * YAFTH_ROW_WIDTH + static_cast<std::uint32_t>(mouse.x - box.x_min);
        }
    }
    return {};
}

void log_wrong_guess(terminal_log &log, const yafth_click_result &result) {
    append_log(log, "Entry Denied");
    std::array<char, log_text_capacity + 1> likeness{};
    std::snprintf(likeness.data(), likeness.size(), "%u/%u correct.", static_cast<unsigned>(result.matching_letter_count),
                  static_cast<unsigned>(result.word_length));
    append_log(log, likeness.data());
}

void log_click(terminal_view &view, const yafth_click_result &result) {
    if (result.kind == YAFTH_IGNORED) {
        return;
    }
    const auto range = result.selected_range;
    auto &log = view.log;
    append_log(log, {view.field.data() + range.begin, range.end - range.begin});
    switch (result.kind) {
    case YAFTH_DUD_REMOVED: {
        append_log(log, "Dud Removed.");
        break;
    }
    case YAFTH_ALLOWANCE_REPLENISHED: {
        append_log(log, "Allowance");
        append_log(log, "Replenished");
        break;
    }
    case YAFTH_ENTRY_DENIED: {
        log_wrong_guess(log, result);
        break;
    }
    case YAFTH_LOCKOUT: {
        log_wrong_guess(log, result);
        append_log(log, "Lockout in");
        append_log(log, "progress.");
        break;
    }
    case YAFTH_EXACT_MATCH: {
        append_log(log, "Exact match!");
        append_log(log, "Please wait");
        append_log(log, "while system");
        append_log(log, "is accessed.");
        break;
    }
    case YAFTH_IGNORED: {
        break;
    }
    }
}

auto handle_input(ftxui::App &app, yafth_state &state, terminal_view &view, ftxui::Event &event) -> bool {
    if (event == ftxui::Event::Custom || event == ftxui::Event::Escape || event == ftxui::Event::Character('q')) {
        app.Exit();
        return true;
    }
    if (!event.is_mouse()) {
        return false;
    }

    const auto &mouse = event.mouse();
    const auto position = field_position(view, mouse);
    view.focus.reset();
    if (!position) {
        return true;
    }
    if (mouse.button != ftxui::Mouse::Left || mouse.motion != ftxui::Mouse::Released) {
        view.focus = yafth_look_at(&state, *position);
        return true;
    }

    const auto result = yafth_click_at(&state, *position);
    // The selected range refers to the field before the click.
    log_click(view, result);
    if (result.kind == YAFTH_DUD_REMOVED) {
        yafth_fill_buffer(&state, view.field.data());
    }
    if (result.kind == YAFTH_EXACT_MATCH || result.kind == YAFTH_LOCKOUT) {
        app.PostEvent(ftxui::Event::Custom);
    }
    return true;
}

auto make_terminal_component(ftxui::App &app, yafth_state &state, terminal_view &view) -> ftxui::Component {
    auto renderer = ftxui::Renderer([&state, &view]() -> ftxui::Element { return render_terminal(state, view); });
    return ftxui::CatchEvent(std::move(renderer), [&app, &state, &view](ftxui::Event event) -> bool { return handle_input(app, state, view, event); });
}
} // namespace

auto main(int argc, char *argv[]) -> int {
    try {
        const auto parsed = parse_options(argc, argv);
        if (const auto *error = std::get_if<option_error>(&parsed)) {
            print_option_error(*error);
            return 1;
        }
        const auto &config = std::get<options>(parsed);
        if (config.help) {
            std::cout << usage;
            return 0;
        }

        yafth_state state;
        switch (yafth_init(&state, config.difficulty, config.science, config.seed)) {
        case YAFTH_INVALID_DIFFICULTY: {
            std::cerr << "Error: invalid difficulty\n";
            return 1;
        }
        case YAFTH_INVALID_SCIENCE: {
            std::cerr << "Error: Science must be between 0 and 100\n";
            return 1;
        }
        case YAFTH_INSUFFICIENT_SCIENCE: {
            std::cerr << "Error: insufficient Science for the selected difficulty\n";
            return 1;
        }
        case YAFTH_OK: {
            break;
        }
        }

        terminal_view view{static_cast<std::uint32_t>(config.seed & 0xffffu)};
        yafth_fill_buffer(&state, view.field.data());
        auto app = ftxui::App::FitComponent();
        app.Loop(make_terminal_component(app, state, view));
        return 0;
    } catch (std::exception const &error) {
        // Use stdio so reporting the error cannot throw.
        // NOLINTNEXTLINE(modernize-use-std-print)
        std::fprintf(stderr, "Error: %s\n", error.what());
        return 1;
    }
}
