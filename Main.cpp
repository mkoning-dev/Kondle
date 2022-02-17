#include <iostream>
#include <format>
#include <array>
#include <gsl/gsl-lite.hpp>

import ANSI_Esc;
import Game;

void print_text(int main_header, int quit_line, int str_offset, int query_line,
	int min_guess, int max_guess, int min_word_length, int max_word_length)
{
	ansi::clear();

	ansi::move(main_header);
	std::cout << "Console Wordle - Main Menu" << std::endl;
	std::cout << std::format("\nFind the word!") << std::endl;
	std::cout << "\nCorrect Letters in the right place are colored "
		<< ansi::color(" G R E E N ", Color::black, Color::green) << "." << std::endl;
	std::cout << "Correct letters in the wrong place are colored "
		<< ansi::color(" Y E L L O W ", Color::black, Color::yellow) << "." << std::endl;
	std::cout << "\nOriginal Wordle word length is 5, and allowed guesses is 6." << std::endl;
	std::cout << "Hard mode requires the next guess to use previously found"
		"\ngreen and yellow letters." << std::endl;

	ansi::move(query_line);
	std::string word_query = std::format("Word length ({} - {}) : ", min_word_length, max_word_length);
	std::string guess_query = std::format("Allowed guesses ({} - {}) : ", min_guess, max_guess);
	std::cout << std::format("{:>{}}", word_query, str_offset);
	std::cout << std::format("\n{:>{}}", guess_query, str_offset);
	std::cout << std::format("\n{:>{}}", "Game mode (N)ormal or (H)ard : ", str_offset);

	ansi::move(quit_line);
	std::cout << "Enter Q to quit." << std::endl;
}

bool set_rules(int min, int max, int line, int offset, int& length)
{
	std::string input_line{};
	auto input = 0;

	ansi::move(line, offset + 1);
	ansi::save_cursor();
	while (input < min || input > max) {
		ansi::erase();
		std::getline(std::cin, input_line);

		if (!input_line.empty()) {
			if (input_line.at(0) == 'q' || input_line.at(0) == 'Q') {
				return true;
			}
			// Try to get int from string.
			try {
				input = std::stoi(input_line);
			}
			catch (const std::exception& e)
			{
				ansi::move(18);
				std::cout << e.what();
			}
		}
		ansi::restore_cursor();
	}

	length = input;
	return false;
}

int main()
{
	using namespace mk;
	auto playing = true;
	auto main_menu = true;

	while (main_menu) {
		constexpr auto main_header = 1;
		constexpr auto query_line = main_header + 11;
		constexpr auto quit_line = query_line + 5;
		constexpr auto exit_line = quit_line + 1;
		constexpr auto str_offset = 35;

		constexpr auto min_guess = 1;
		constexpr auto max_guess = 9; // Larger values cause formatting issues.
		constexpr auto min_word_length = 2;
		constexpr auto max_word_length = 9;

		auto valid_input = false;
		auto word_length = 0;
		auto num_guesses = 0;
		auto hard_mode = false;

		print_text(main_header, quit_line, str_offset, query_line,
			min_guess, max_guess, min_word_length, max_word_length);

		while (!valid_input) {
			std::string input_line{};
			auto input = '\0';
			auto quit = false;

			quit = set_rules(min_word_length, max_word_length, query_line, str_offset, word_length);

			if (!quit) {
				quit = set_rules(min_guess, max_guess, query_line + 1, str_offset, num_guesses);
			}

			if (quit) { input = 'Q'; }

			// Enter hard mode.
			ansi::move(query_line + 2, str_offset + 1);
			ansi::save_cursor();

			while (input != 'N' && input != 'H' && input != 'Q') {
				ansi::erase();
				std::getline(std::cin, input_line);
				input = gsl::narrow_cast<char>
					(std::toupper(gsl::narrow_cast<unsigned char>(input_line.at(0))));
				ansi::restore_cursor();
			}

			valid_input = true;
			switch (input) {
			case 'N':
				hard_mode = false;
				playing = true;
				break;
			case  'H':
				hard_mode = true;
				playing = true;
				break;
			case  'Q':
				main_menu = false;
				playing = false;
				ansi::move(exit_line);
				std::cout << "Exiting game..." << std::endl;
				break;
			default:
				playing = true;
				valid_input = false;
				ansi::move(exit_line);
				std::cout << "Invalid input";
				break;
			}
		}
		if (!main_menu) {
			break;
		}

		ansi::clear();

		Game game{ word_length, num_guesses, hard_mode };

		while (playing) {
			constexpr auto guess_header = 1;
			constexpr auto guess_entry = guess_header + 1;

			std::string guess = "";

			//ansi::move(guess_header);
			ansi::home();
			std::cout << "Guess " << game.get_guess_num() + 1 << "/" << game.get_max_guesses();
			ansi::move(guess_entry);
			std::cout << "Enter your guess here or Q to exit to main menu: ";
			ansi::erase();
			ansi::save_cursor();
			while (guess == "") {
				std::getline(std::cin, guess);
				ansi::restore_cursor();
			}

			if (guess == "q" || guess == "Q") {
				playing = false;
				ansi::move(game.end_position());
				break;
			}

			game.set_guess(guess);

			if (!game.check_input()) {
				continue;
			}

			game.incr_guess_num();

			if (game.check_guess()) {
				ansi::move(game.end_position());
				std::cout << "You win!" << std::endl
					<< "Press enter to continue...";
				std::cin.get();
				playing = false;
			}
			else if (game.game_over()) {
				ansi::move(game.end_position());
				std::cout << "The word was "
					<< game.get_word() << ". Better luck next time!" << std::endl;
				std::cout << "Press enter to continue...";
				std::cin.get();
				playing = false;
			}
		}
	}
}