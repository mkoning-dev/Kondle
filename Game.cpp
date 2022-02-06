module;

#include <iostream>
#include <fstream>
#include <format>
#include <regex>
#include <random>
#include <chrono>
#include <thread>
#include <gsl/gsl-lite.hpp>

module Game;

namespace mk {
	int random_number(int size)
	{
		static std::mt19937 gen(std::random_device{}());
		std::uniform_int_distribution<> const distrib(0, size - 1);

		return (distrib(gen));
	}

	std::string& str_toupper(std::string& s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return gsl::narrow_cast<char>(std::toupper(c)); }
		);
		return s;
	}

	Game::Game(int w, int g, bool h) :
		m_word_length{ w }, m_max_guesses{ g }, m_hard_mode{ h }{
		parse_file();
		set_word();
		build_grid();
		print_keyboard();
	};

	void Game::parse_file()
	{
		m_word_list.clear();
		auto filename = "words.txt";
		std::string line;
		const std::regex txt_regex("^[a-z]+$");

		if (auto input = std::fstream{ filename }) {
			// Read file and store word_length lowercase words in vector.
			while (std::getline(input, line)) {
				if (line.size() == m_word_length && std::regex_match(line, txt_regex)) {
					m_word_list.push_back(str_toupper(line));
				}
			}
		}
		else {
			throw std::runtime_error{
				std::format("Failed to open \"{}\".\n", filename)
			};
		}
		std::cout << std::endl;
	}

	void Game::set_word() {
		const auto rand_index = random_number(gsl::narrow_cast<int>(m_word_list.size()));
		m_word = m_word_list.at(rand_index);
	}

	void Game::build_grid()
	{
		const auto dash_length = (m_word_length * 4) + 1;
		auto line = std::format("{:-<{}}", "", dash_length);

		for (int i = 0; i < m_max_guesses + 1; ++i) {
			const auto row_offset = i * 2;
			ansi::move(m_grid_start + row_offset, m_col_start);
			std::cout << line << std::endl;
			if (i < m_max_guesses) {
				for (int j = 0; j < m_word_length + 1; ++j) {
					const auto col_offset = j * 4;
					ansi::move((m_grid_start + 1)
						+ row_offset, m_col_start + col_offset);
					std::cout << "|";
				}
			}
		}
	}

	bool Game::check_input()
	{
		clear_error_line();

		// Ensure all hinted letters are found in guessed word.
		if (m_hard_mode) {
			for (auto& e : m_key_letters) {
				if (e.second == Color::green || e.second == Color::yellow) {
					if (!m_guess.contains(e.first)) {
						ansi::move(m_error_line);
						std::cout << "Guess must contain " << e.first;
						return false;
					}
				}
			}
		}
		const std::regex txt_regex("^[A-Z]+$");
		if (!std::regex_match(m_guess, txt_regex)) {
			std::cout << "The entered word contains invalid characters."
				<< std::endl;
			return false;
		}

		if (m_guess.size() != m_word_length) {
			std::cout << std::format("Words must be {} letters long.", m_word_length) << std::endl;
			return false;
		}

		if (std::find(m_word_list.begin(), m_word_list.end(), m_guess) == m_word_list.end()) {
			std::cout << "Word not in word list!" << std::endl;
			return false;
		}
		return true;
	}

	void Game::fill_letter_map()
	{
		m_letter_count.clear();
		for (int i = 0; i < m_word_length; ++i) {
			// Count letters found in word.
			// Used to color the right amount of letters.
			// https://en.cppreference.com/w/cpp/container/unordered_map/find
			if (!m_key_letters.contains(gsl::at(m_guess, i))) {
				m_key_letters.insert_or_assign(gsl::at(m_guess, i), Color::blue);
			}

			if (m_letter_count.contains(gsl::at(m_word, i))) {
				m_letter_count[gsl::at(m_word, i)]++;
			}
			else {
				m_letter_count.insert({ gsl::at(m_word, i), 1 });
			}
		}
	}

	void Game::color_letters()
	{
		m_color.clear();

		// Mark letters to be printed green.
		for (int i = 0; i < m_word_length; ++i) {
			if (gsl::at(m_guess, i) == gsl::at(m_word, i)) {
				m_color.push_back(Color::green);
				m_key_letters.insert_or_assign(gsl::at(m_guess, i), Color::green);
				m_letter_count[gsl::at(m_word, i)]--;
			}
			else {
				m_color.push_back(Color::white);
			}
		}

		// Mark letters to be printed yellow.
		for (int i = 0; i < m_word_length; ++i) {

			// Skip letters already marked to be printed green.
			if (gsl::at(m_color, i) != Color::green) {
				for (int j = 0; j < m_word_length; ++j) {
					if (gsl::at(m_guess, i) == gsl::at(m_word, j)) {
						if (m_letter_count[gsl::at(m_word, j)] > 0) {
							gsl::at(m_color, i) = Color::yellow;
							m_letter_count[gsl::at(m_word, j)]--;
						}
						if (m_key_letters.at(gsl::at(m_guess, i)) != Color::green) {
							m_key_letters.at(gsl::at(m_guess, i)) = Color::yellow;
						}
					}
				}
			}
		}
	}

	void Game::print_guess()
	{
		// Print entered letters on grid.
		std::chrono::milliseconds const timespan(250);
		for (int i = 0; i < m_word_length; ++i) {
			const auto col_offset = i * 4;
			const auto row_offset = (m_guess_num - 1) * 2;
			const auto start_row = m_grid_start + 1;
			const auto start_col_color = m_col_start + 1;
			const auto start_col_plain = m_col_start + 2;

			if (gsl::at(m_color, i) == Color::green || gsl::at(m_color, i) == Color::yellow) {
				ansi::move(start_row + row_offset, start_col_color + col_offset);
				std::cout << ansi::color(std::format("{: ^3}", gsl::at(m_guess, i)), Color::black, gsl::at(m_color, i));
			}
			else {
				ansi::move(start_row + row_offset, start_col_plain + col_offset);
				std::cout << gsl::at(m_guess, i);
			}
			std::this_thread::sleep_for(timespan);
		}
	}

	void Game::print_keyboard()
	{
		// Keyboard rows
		constexpr auto rows = 3;
		std::string key_row[rows];
		key_row[0] = "QWERTYUIOP";
		key_row[1] = "ASDFGHJKL";
		key_row[2] = "ZXCVBNM";

		// Print keyboard layout to screen with used letters highlighted.
		ansi::move(m_keyboard_start);
		for (int i = 0; i < rows; ++i) {
			// Center keyboard below grid.
			const int keyboard_col = m_col_start + (2 * m_word_length - 10);

			const std::string tmp = gsl::at(key_row, i);
			ansi::move_col(keyboard_col);
			std::cout << std::format("{: <{}}", "", i + 1);
			for (int j = 0; j < tmp.length(); ++j) {
				const auto c = gsl::at(tmp, j);
				const auto colored = m_key_letters.contains(c);

				if (colored)
					std::cout << ansi::color(c, m_key_letters[c]) << " ";
				else
					std::cout << c << " ";
			}
			std::cout << std::endl;;
		}
	}

	bool Game::check_guess()
	{
		fill_letter_map();
		color_letters();
		print_guess();
		print_keyboard();

		if (m_guess == m_word) {
			return true;
		}

		return false;
	}

	void Game::clear_error_line()
	{
		ansi::move(m_error_line);
		ansi::erase_line();
	}

	void Game::incr_guess_num() noexcept { m_guess_num++; }
	void Game::set_guess(std::string& guess) { m_guess = str_toupper(guess); }
	bool Game::game_over() noexcept { return m_guess_num >= m_max_guesses; }
	int Game::end_position() noexcept { return m_end_msg; }
}