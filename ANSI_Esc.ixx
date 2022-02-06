module;

#include <iostream>
#include <format>

export module ANSI_Esc;

export enum class Color { black, red, green, yellow, blue, magenta, cyan, white };

// ANSI escape codes.
namespace ansi {
	std::string esc = "\x1b";

	// Clear entire screen and move cursor position to top left.
	export void clear()
	{
		std::cout << std::format("{0}[2J{0}[H", esc);
	}

	// Move cursor position to top left.
	export void home()
	{
		std::cout << std::format("{}[H", esc);
	}

	// Erase from cursor to end of line.
	export void erase()
	{
		std::cout << std::format("{}[0K", esc);
	}

	// Erase entire line.
	export void erase_line()
	{
		std::cout << std::format("{}[2K", esc);
	}

	// Move cursor to specified column.
	export void move_col(int col)
	{
		std::cout << std::format("{0}[{1}G", esc, col);
	}

	// Move cursor to specified line.
	export void move(int line)
	{
		std::cout << std::format("{0}[{1};0H", esc, line);
	}

	// Move cursor to specified line and column.
	export void move(int line, int col)
	{
		std::cout << std::format("{0}[{1};{2}H", esc, line, col);
	}

	// Returns colored version of input.
	export template <typename T>
		std::string color(const T& s, Color c)
	{
		return std::format("{0}[3{1}m{2}{0}[m", esc, std::to_underlying(c), s);
	}

	// Returns colored version of input with foreground and background colors.
	export template <typename T>
		std::string color(const T& s, Color f, Color b)
	{
		return std::format("{0}[3{1};4{2}m{3}{0}[m", esc,
			std::to_underlying(f), std::to_underlying(b), s);
	}

	// Save and restore cursor position.
	export void save_cursor() { std::cout << std::format("{}[s", esc); }
	export void restore_cursor() { std::cout << std::format("{}[u", esc); }
}