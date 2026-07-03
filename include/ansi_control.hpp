#pragma once

#include <iostream>
#include <string_view>
#include <span>
#include <print>


// Terminal Control Sequences
#define TERM_CLEAR_ALL       "\033[2J"
#define TERM_CLEAR_LINE      "\033[2K"
#define TERM_CURSOR_HOME     "\033[H"
#define TERM_CURSOR_HIDE     "\033[?25l"
#define TERM_CURSOR_SHOW     "\033[?25h"
#define TERM_CURSOR_LINEHOME "\033[1G" 

// Text Attributes
#define TERM_RESET          "\033[0m"
#define TERM_BOLD           "\033[1m"
#define TERM_DIM            "\033[2m"
#define TERM_UNDERLINE      "\033[4m"
#define TERM_BLINK          "\033[5m"
#define TERM_REVERSE        "\033[7m"

// Foreground Colors (Standard)
#define TERM_FG_BLACK       "\033[30m"
#define TERM_FG_RED         "\033[31m"
#define TERM_FG_GREEN       "\033[32m"
#define TERM_FG_YELLOW      "\033[33m"
#define TERM_FG_BLUE        "\033[34m"
#define TERM_FG_MAGENTA     "\033[35m"
#define TERM_FG_CYAN        "\033[36m"
#define TERM_FG_WHITE       "\033[37m"

// Foreground Colors (Bright)
#define TERM_FG_B_BLACK     "\033[90m"
#define TERM_FG_B_RED       "\033[91m"
#define TERM_FG_B_GREEN     "\033[92m"
#define TERM_FG_B_YELLOW    "\033[93m"
#define TERM_FG_B_BLUE      "\033[94m"
#define TERM_FG_B_MAGENTA   "\033[95m"
#define TERM_FG_B_CYAN      "\033[96m"
#define TERM_FG_B_WHITE     "\033[97m"

// Background Colors (Standard)
#define TERM_BG_BLACK       "\033[40m"
#define TERM_BG_RED         "\033[41m"
#define TERM_BG_GREEN       "\033[42m"
#define TERM_BG_YELLOW      "\033[43m"
#define TERM_BG_BLUE        "\033[44m"
#define TERM_BG_MAGENTA     "\033[45m"
#define TERM_BG_CYAN        "\033[46m"
#define TERM_BG_WHITE       "\033[47m"

// Background Colors (Bright)
#define TERM_BG_B_BLACK     "\033[100m"
#define TERM_BG_B_RED       "\033[101m"
#define TERM_BG_B_GREEN     "\033[102m"
#define TERM_BG_B_YELLOW    "\033[103m"
#define TERM_BG_B_BLUE      "\033[104m"
#define TERM_BG_B_MAGENTA   "\033[105m"
#define TERM_BG_B_CYAN      "\033[106m"
#define TERM_BG_B_WHITE     "\033[107m"

namespace term {
  // Moves the cursor to an absolute (row, col) position (1-indexed)
  inline void move_to(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
  }

  // Moves the cursor up N rows
  inline void move_up(int rows, bool clear = false) {
    std::cout << "\033[" << rows << "A" << (clear ? TERM_CLEAR_LINE : "");
  }

  inline void move_up_c(int rows) {
    for (int i=0;i<rows;i++){
      std::cout << "\033[1A" <<TERM_CLEAR_LINE;
    }
  }

  inline void move_down_c(int rows) {
    for (int i=0;i<rows;i++){
      std::cout << "\033[1B" <<TERM_CLEAR_LINE;
    }
  }
  inline void clear_line(){
    fputs(TERM_CLEAR_LINE,stdout);
  }

  inline void clear_all(){
    fputs(TERM_CLEAR_ALL,stdout);
  }
  inline void curser_home(){
    fputs(TERM_CURSOR_LINEHOME, stdout);
  }

  // Moves the cursor down N rows
  inline void move_down(int rows, bool clear = false) {
    std::cout << "\033[" << rows << "B" << (clear ? TERM_CLEAR_LINE : "");
  }

  // Sets standard 8-bit color (0-255)
  inline void set_fg_8bit(int color_id) {
    std::cout << "\033[38;5;" << color_id << "m";
  }

  inline void set_bg_8bit(int color_id) {
    std::cout << "\033[48;5;" << color_id << "m";
  }

  // Sets true 24-bit RGB color
  inline void set_fg_rgb(int r, int g, int b) {
    std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
  }

  inline void set_bg_rgb(int r, int g, int b) {
    std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
  }
}


template<typename T>
concept Printable = requires(std::ostream& os, const T& value) {
  os << value;
};

template<typename T>
struct container_traits {
  using value_type = typename T::value_type;
};
template<typename T>
struct container_traits<T[]> {
  using value_type = T;
};
template<typename T, size_t N>
struct container_traits<T[N]> {
  using value_type = T;
};
namespace term {
  template<typename Container>
    requires Printable<typename container_traits<std::remove_cvref_t<Container>>::value_type>

    // requires Printable<typename Container::value_type>
    inline void print_labeled_array(std::string_view label, const Container &values) {
      std::cout << "\033[1;36m" << label << ": \033[0m";
      if (std::ranges::empty(values)) return (void)(std::cout << "??\n");
      bool first = true;

      for (const auto& item : values) {
        if (!first) {
          std::cout << ", ";
        }
        std::cout << item;
        first = false;
      }
      std::cout << "\n";
    }
  template<typename T>
    requires Printable<T>
    inline void print_labeled_array(std::string_view label, std::initializer_list<T> values) {
      print_labeled_array(label, std::span<const T>(values));
    }
  template<typename T>
    requires Printable<T>
    inline void print_labeled_array(std::string_view title, std::initializer_list<T> values, std::initializer_list<std::string_view> labels) {
      std::cout << "\033[1;36m" << title << ": \033[0m";
      std::span<const T> val_span{values};
      std::span<const std::string_view> lab_span{labels};
      size_t count = std::min(val_span.size(), lab_span.size());
      if (count == 0) {
        std::cout << "??\n";
        return;
      }
      for (size_t i = 0; i < count; ++i) {
        if (i > 0) {
          std::cout << ", ";
        }
        std::cout << lab_span[i] << ": " << val_span[i];
      }
      std::cout << "\n";
    }



inline void print_engine_perf(double fps, double total_ms, double update_ms, double render_ms) {
  // Move cursor back to home top-left position
  std::print("{}", TERM_CURSOR_HOME);

  const char* performance_color = (total_ms > 16.67) ? TERM_FG_B_RED : TERM_FG_B_GREEN;
  double budget_percent = (total_ms / 16.67) * 100.0;

  // Header Box Block - \033[2K clears the line before printing to remove older content overlaps
  std::print("\033[2K{}{}{}┌────────────────────────────────────────┐\n", TERM_BOLD, TERM_FG_B_CYAN, TERM_RESET);
  std::print("\033[2K{}{}│ {}{}STATS:{}│\n", TERM_BOLD, TERM_FG_B_CYAN, TERM_FG_B_YELLOW, TERM_BOLD, " ");
  std::print("\033[2K{}{}├────────────────────────────────────────┤\n{}", TERM_BOLD, TERM_FG_B_CYAN, TERM_RESET);

  // Frame Rate Row
  std::print("\033[2K{}{}│ {}Frame Rate:  {}{:>6.2f} FPS{} │\n", 
             TERM_BOLD, TERM_FG_B_CYAN, TERM_FG_WHITE, performance_color, fps, TERM_RESET);

  // Frame Budget Row
  std::print("\033[2K{}{}│ {}Frame Budget: {}{:>6.2f} ms {}({:>5.1f}%%){} │\n", 
             TERM_BOLD, TERM_FG_B_CYAN, TERM_FG_WHITE, performance_color, total_ms, TERM_DIM, budget_percent, TERM_RESET);

  std::print("\033[2K{}{}├────────────────────────────────────────┤\n{}", TERM_BOLD, TERM_FG_B_CYAN, TERM_RESET);

  // Engine Work Breakdowns
  std::print("\033[2K{}{}│ {} -> {}Core Update: {:>6.10f} ms{} │\n", 
             TERM_BOLD, TERM_FG_B_CYAN, TERM_RESET, TERM_FG_B_BLUE, update_ms, TERM_RESET);

  std::print("\033[2K{}{}│ {} -> {}GPU Render:  {:>6.10f} ms{} │\n", 
             TERM_BOLD, TERM_FG_B_CYAN, TERM_RESET, TERM_FG_B_MAGENTA, render_ms, TERM_RESET);

  std::print("\033[2K{}{}└────────────────────────────────────────┘\n{}", TERM_BOLD, TERM_FG_B_CYAN, TERM_RESET);
  
  // Flash out stream
  fflush(stdout);
}

}
