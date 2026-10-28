#include "Hangman.h"
#include <iostream>
#include <string_view>
#include <utility>

#ifdef _DEBUG
#include "Test.h"
#endif

int main()
{
#ifdef _DEBUG
	{
		std::cout << "Tests: \n";

		for (auto i{ 0 }; i < 18; ++i)
		{
			std::cout << Hangman::randomWord() << '\n';
		}

		std::cout << '\n';

		const std::vector<std::string_view> wordList{ "apple", "banana", "cherry" };

		for (auto i{ 0 }; i < 6; ++i)
		{
			std::cout << Hangman::randomWord(wordList) << '\n';
		}

		std::cout << '\n';

		const auto word{ Hangman::randomWord() };

		std::cout << "Generating hangman with word: " << word << '\n';
		Hangman::V1::Session hangmanv1{ word, 6uz };
		Hangman::Session hangmanv2{ *Hangman::session(word, 6uz) };

		std::cout << "Version 1: \n";
		std::cout << hangmanv1 << '\n';

		std::cout << "Guessing a\n";
		hangmanv1.guess('a');
		std::cout << "Guessing i\n";
		hangmanv1.guess('i');

		std::cout << hangmanv1 << '\n';

		std::cout << '\n';

		std::cout << "Version 2: \n";
		std::cout << hangmanv2 << '\n';

		std::cout << "Guessing a\n";
		std::ignore = hangmanv2.guess('a');
		std::cout << "Guessing i\n";
		std::ignore = hangmanv2.guess('i');

		std::cout << hangmanv2 << '\n';

		Test::Hangman::V2::session();

		Test::Hangman::V2::guess();
		Test::Hangman::V1::guess();

		Test::Hangman::V2::hasLives();
		Test::Hangman::V1::hasLives();

		Test::Hangman::V2::won();
		Test::Hangman::V1::won();

		std::cout << '\n';

		Hangman::getInput("Testing input: ");

		std::cout << "Tests Ended\n";
		std::cout << '\n';
	}
#endif

	//std::expected customSession{ Hangman::session("apple", 12uz) };
	//Hangman::play(*customSession);
	Hangman::play();

	return 0;
}