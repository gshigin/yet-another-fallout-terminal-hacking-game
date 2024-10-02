#include "yafth/engine.h"

#include <array>
#include <numeric>
#include <array>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <ftxui/component/component.hpp>       // for Button, Renderer, etc.
#include <ftxui/component/screen_interactive.hpp>  // for ScreenInteractive
#include <ftxui/dom/elements.hpp>              // for text, separator, etc.
#include <ftxui/dom/node.hpp>

using namespace ftxui;

struct terminal_state
{
  terminal_state(const yafth::LockLevel ll, const uint32_t pss, const uint64_t s) : eng{ll, pss, s} {}

  yafth::engine eng;
  int highlight_begin = -1;
  int highlight_end = -1;
  std::string log = "";
};

int main() {
  // initialize engine
  terminal_state shared_term(yafth::LockLevel::Average, 65, 1ull);

  // First horizontal window (static, height 5).
  auto top_window = vbox({
    text("ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL"),
    text("!!! WARNING: LOCKOUT IMMINENT !!!"),
    text(""),
    text("1 ATTEMPT(S) LEFT : X"),
    text(""),
  }) | size(HEIGHT, EQUAL, 5);

  // Vertical sub-windows: two interactive and three static.

  // First static vertical window.
  auto static_window_1 = vbox(
    [&](){
      std::vector<ftxui::Element> hexes;
      int start = 1234567 & 0xFFFF;
      for (int i = 0; i < 17; ++i) 
      {
        std::stringstream stream;
        stream << "0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << start;
        hexes.push_back(ftxui::text(stream.str()));
        start += 12;
      }
      return hexes;
    }());

  // Second interactive vertical window.
  auto interactive_window_1 = Renderer([&] {
    return vbox([&](){
      std::vector<ftxui::Element> lines;
      auto s = engine.get_term_chars();
      for(int i = 0; i < 17; ++i)
      {
        lines.push_back(ftxui::text(std::string{s.begin() + i * 12, s.begin() + (i + 1) * 12}));
      }
      return lines;
    }());
  });

  // Third static vertical window.
  auto static_window_2 = vbox(
    [&](){
      std::vector<ftxui::Element> hexes;
      int start = 7654321 & 0xFFFF;
      for (int i = 0; i < 17; ++i) 
      {
        std::stringstream stream;
        stream << "0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << start;
        hexes.push_back(ftxui::text(stream.str()));
        start += 12;
      }
      return hexes;
    }());

  // Fourth interactive vertical window.
  auto interactive_window_2 = Renderer([&] {
    return vbox([&](){
      std::vector<ftxui::Element> lines;
      auto s = engine.get_term_chars();
      for(int i = 17; i < 34; ++i)
      {
        lines.push_back(ftxui::text(std::string{s.begin() + i * 12, s.begin() + (i + 1) * 12}));
      }
      return lines;
    }());
  });

  // Fifth static vertical window.
  auto static_window_3 = vbox({
    text("Static Window 3"),
    separator(),
    text("Final static content"),
  });

  // Combine the vertical windows into a horizontal container (height 17).
  auto bottom_windows = hbox({
    static_window_1 | size(WIDTH, EQUAL, 6),
    text(" "),
    interactive_window_1->Render() | size(WIDTH, EQUAL, 12),
    text(" "),
    static_window_2 | size(WIDTH, EQUAL, 6),
    text(" "),
    interactive_window_2->Render() | size(WIDTH, EQUAL, 12),
    text(" "),
    static_window_3 | size(WIDTH, EQUAL, 10),
  }) | size(HEIGHT, EQUAL, 17);

  // Main layout combining the top window and bottom windows.
  auto main_container = vbox({
    top_window,
    bottom_windows,
  }) | size(WIDTH, EQUAL, 53) | size(HEIGHT, EQUAL, 22) | border;

  // Create the screen and loop.
  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(Renderer([&](){ return main_container; }));

  return 0;
}

