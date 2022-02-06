module;

#include <vector>
#include <unordered_map>

export module Game;

import ANSI_Esc;

namespace mk {
	export class Game {
	public:
		Game(int w, int g, bool h);
		void parse_file();
		void build_grid();
		void print_keyboard();
		bool check_input();
		void clear_error_line();
		void fill_letter_map();
		void color_letters();
		void print_guess();
		bool check_guess();
		void incr_guess_num() noexcept;
		void set_word();
		void set_guess(std::string& guess);
		bool game_over() noexcept;
		int end_position() noexcept;

		auto get_word() const { return m_word; }
		auto get_max_guesses() const noexcept { return m_max_guesses; }
		auto get_word_list() const { return m_word_list; }
		auto get_guess_num() const noexcept { return m_guess_num; }
		auto get_guess() const { return m_guess; }

	private:
		// Game rules.
		int m_word_length = 0;
		int m_max_guesses = 0;
		bool m_hard_mode = false;

		// Positioning constants.
		const int m_col_start = 8;
		const int m_grid_length = (m_max_guesses * 2) + 1;
		const int m_error_line = 3;
		const int m_grid_start = m_error_line + 2;
		const int m_grid_end = m_grid_start + m_grid_length;
		const int m_keyboard_start = m_grid_end + 2;
		const int m_keyboard_end = m_keyboard_start + 3;
		const int m_end_msg = m_keyboard_end + 1;

		// Current guess/attempt number.
		int m_guess_num = 0;

		std::string m_word{};
		std::string m_guess{};

		std::vector<Color> m_color;
		std::vector<std::string> m_word_list;

		///////////////
		// Key letter map stores letters with corresponding colors to be
		// printed to keyboard below the grid.
		// 
		std::unordered_map<unsigned char, Color> m_key_letters;

		///////////////
		// Letter count map keeps track of letter count of the winning word.
		// 
		std::unordered_map<unsigned char, int> m_letter_count;
	};
}