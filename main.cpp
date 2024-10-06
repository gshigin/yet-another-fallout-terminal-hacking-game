#include "yafth/engine.h"
#include "yafth/random.h"

#include <array>
#include <numeric>
#include <array>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "ftxui/component/animation.hpp"       // for BackOut, Duration
#include <ftxui/component/component.hpp>       // for Button, Renderer, etc.
#include <ftxui/component/screen_interactive.hpp>  // for ScreenInteractive
#include <ftxui/dom/elements.hpp>              // for text, separator, etc.
#include <ftxui/dom/node.hpp>

using namespace ftxui;

struct terminal_state
{
  terminal_state(const yafth::LockLevel ll, const uint32_t pss, const uint64_t s) 
  : eng{ll, pss, s}, 
    hex{yafth::random::xoroshiro128{static_cast<uint64_t>(-1), yafth::random::seed(s)}.next()}
  {}
  
  yafth::engine eng;
  int highlight_begin = -1;
  int highlight_end = -1;
  std::vector<ftxui::Element> log = {};
  std::string last_loocked = "";
  uint64_t hex = 0;
  bool end_game = false;
};

int main() {
  // initialize engine
  terminal_state shared_term(yafth::LockLevel::Average, 65, time(0));
  yafth::engine & engine = shared_term.eng;

  auto screen = ScreenInteractive::FitComponent();

  // First horizontal window (static, height 5).
  auto attempts_window = Renderer([&] {
    auto attempts = shared_term.eng.get_attempts();
    std::string attempts_text = std::to_string(attempts) + " ATTEMPT(S) LEFT : ";

    Elements attempt_elements;
    for (int i = 0; i < attempts; ++i) {
      attempt_elements.push_back(text(" ") | inverted);
      attempt_elements.push_back(text(" "));
    }

    std::string warning = attempts <= 1 ? "!!! WARNING: LOCKOUT IMMINENT !!!" : "ENTER PASSWORD NOW";

    return vbox({
      text("ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL"),
      text(warning),
      text(""),
      hbox({text(attempts_text), hbox(attempt_elements)}),
      text(""),
    }) | size(HEIGHT, EQUAL, 5);
  });

  // Vertical sub-windows: two interactive and three static.

  // First static vertical window.
  auto static_window_1 = vbox(
    [&](){
      std::vector<ftxui::Element> hexes;
      auto & start = shared_term.hex;
      start &= 0xFFFF;
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
      auto s = shared_term.eng.get_term_chars();
      for(int i = 0; i < 17; ++i)
      {
        std::vector<ftxui::Element> line;
        for(auto j = i * 12; j < (i + 1) * 12; ++j)
        {
          if(shared_term.highlight_begin <= j && j < shared_term.highlight_end)
          {
            line.push_back(text(std::string{s[j]}) | inverted);
          }
          else
          {
            line.push_back(text(std::string{s[j]}));
          }
        }
        lines.push_back(hbox(line));
      }
      return lines;
    }());
  }) ;

  auto mouse_handler = CatchEvent([&](Event event) 
  {
    if(shared_term.end_game)
    {
      shared_term.highlight_begin = -1;
      shared_term.highlight_end = -1;
      shared_term.last_loocked = "";
      screen.PostEvent(Event::Custom);
      screen.ExitLoopClosure()();
    }
    if(event.is_mouse())
    {
      auto m_x = event.mouse().x;
      auto m_y = event.mouse().y;

      if( (8 <= m_x && m_x <= 19) && (6 <= m_y && m_y <= 22) ) // first window
      {
        m_x -= 8;
        m_y -= 6;
      }
      else if( (28 <= m_x && m_x <= 39) && (6 <= m_y && m_y <= 22) ) // first window
      {
        m_x -= 28;
        m_y -= 6;
        m_y += 17;
      }
      else {
        shared_term.highlight_begin = -1;
        shared_term.highlight_end = -1;
        return false;        
      }

      const auto & [b, e] = shared_term.eng.look_at(12*m_y + m_x);

      shared_term.highlight_begin = b;
      shared_term.highlight_end = e;
      shared_term.last_loocked = shared_term.eng.get_term_chars().substr(b, e - b);

      screen.PostEvent(Event::Custom);

      if(event.mouse().button == Mouse::Left && event.mouse().motion == Mouse::Pressed)
      {
        auto cs = shared_term.eng.click_at(12*m_y + m_x);
        switch (cs.state)
        {
        case yafth::ClickState::Error :
          break;
        case yafth::ClickState::DudRemoved :
          shared_term.log.push_back(text(">" + shared_term.last_loocked));
          shared_term.log.push_back(text(">Dud Removed."));
          shared_term.last_loocked= "";
          break;
        case yafth::ClickState::AllowanceReplenished :
          shared_term.log.push_back(text(">" + shared_term.last_loocked));
          shared_term.log.push_back(text(">Allowance"));
          shared_term.log.push_back(text(">Replenished"));
          shared_term.last_loocked= "";
          break;
        case yafth::ClickState::EntryDenied :
          shared_term.log.push_back(text(">" + shared_term.last_loocked));
          shared_term.log.push_back(text(">Entry Denied"));
          shared_term.log.push_back(text(">" + std::to_string(cs.match.value()) + "/" + std::to_string(shared_term.eng.get_word_length()) + " correct."));
          shared_term.last_loocked= "";
          break;
        case yafth::ClickState::LockoutInProgress :
          shared_term.log.push_back(text(">" + shared_term.last_loocked));
          shared_term.log.push_back(text(">Entry Denied"));
          shared_term.log.push_back(text(">" + std::to_string(cs.match.value()) + "/" + std::to_string(shared_term.eng.get_word_length()) + " correct."));
          shared_term.log.push_back(text(">Lockout in"));
          shared_term.log.push_back(text(">progress."));
          shared_term.last_loocked= "";
          shared_term.end_game = true;
          break;
        case yafth::ClickState::ExactMatch :
          shared_term.log.push_back(text(">" + shared_term.last_loocked));
          shared_term.log.push_back(text(">Exact match!"));
          shared_term.log.push_back(text(">Please wait"));
          shared_term.log.push_back(text(">while system"));
          shared_term.log.push_back(text(">is accesed."));
          shared_term.last_loocked= "";
          shared_term.end_game = true;
          break;
        default:
          break;
        }

        if (shared_term.log.size() > 15)
        {
          std::size_t to_erase = shared_term.log.size() - 15;
          shared_term.log.erase(shared_term.log.begin(), shared_term.log.begin() + to_erase);
        }

        screen.PostEvent(Event::Custom);

        return true;
      }
      return true;
    }
    return false;
  });

  // Third static vertical window.
  auto static_window_2 = vbox(
    [&](){
      std::vector<ftxui::Element> hexes;
      auto & start = shared_term.hex;
      start &= 0xFFFF;
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
      auto s = shared_term.eng.get_term_chars();
      for(int i = 17; i < 34; ++i)
      {
        std::vector<ftxui::Element> line;
        for(auto j = i * 12; j < (i + 1) * 12; ++j)
        {
          if(shared_term.highlight_begin <= j && j < shared_term.highlight_end)
          {
            line.push_back(text(std::string{s[j]}) | inverted);
          }
          else
          {
            line.push_back(text(std::string{s[j]}));
          }
        }
        lines.push_back(hbox(line));
      }
      return lines;
    }());
  });

  // Fifth static vertical window.
  auto log_window = Renderer([&] {
    return vbox({
      filler(),
      vbox(shared_term.log),
      text(""),
      text(">" + shared_term.last_loocked),
    });
  });

  // Combine the vertical windows into a horizontal container (height 17).
  auto bottom_windows = Renderer([&] {
      return  hbox({
      static_window_1 | size(WIDTH, EQUAL, 6),
      text(" "),
      interactive_window_1->Render() | size(WIDTH, EQUAL, 12) | size(WIDTH, EQUAL, 12),
      text(" "),
      static_window_2 | size(WIDTH, EQUAL, 6),
      text(" "),
      interactive_window_2->Render() | size(WIDTH, EQUAL, 12),
      text(" "),
      log_window->Render() | size(WIDTH, EQUAL, 13),
    }) | size(HEIGHT, EQUAL, 17);
  });

  // Main layout combining the top window and bottom windows.
  auto main_container = Renderer([&] {
    return vbox({
      attempts_window->Render(),
      bottom_windows->Render(),
    }) | size(WIDTH, EQUAL, 53) | size(HEIGHT, EQUAL, 22) | border;
  });

  // Create the screen and loop.
  screen.Loop(main_container | mouse_handler);

  return 0;
}

