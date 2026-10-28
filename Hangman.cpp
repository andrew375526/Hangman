#include "Hangman.h"
#include "Input.h"
#include "Random.h"
#include <cassert>
#include <expected>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

namespace
{
	using SignedIndex = std::ptrdiff_t;

	constexpr std::vector<SignedIndex> indiciesMatched(
		char guessLetter,
		std::string_view word
	)
	{
		std::vector<SignedIndex> indiciesMatched{};

		for (auto i{ 0z }; i < std::ssize(word); ++i)
			if (guessLetter == std::data(word)[i])
				indiciesMatched.push_back(i);

		return indiciesMatched;
	}

	void updateUnderscores(
		const std::vector<SignedIndex>& arr,
		char guessLetter,
		std::vector<char>& underscores
	);

	bool alreadyGuessed(char guessLetter, const Hangman::Session& session);
	void printIntro();
	void printGuess(bool validGuess, char inputLetter);
	using ExpectedBE = std::expected<bool, Hangman::Session::Error>;
	bool invalidInputError(ExpectedBE guess);
	bool alreadyGuessedError(ExpectedBE guess);
	void handleInvalidGuess(ExpectedBE guess);
	void gameLoop(Hangman::Session& session);
	void printResult(const Hangman::Session& session);
}

namespace Hangman::V2
{
	auto session(std::string_view word, std::size_t maxGuesses)
		-> std::expected<Session, Session::Error>
	{
		if (std::empty(word))
		{
			return std::unexpected(Session::Error::noWordProvided);
		}

		return Session{ word, maxGuesses };
	}

	const std::vector<char>& Session::getUnderscores() const
	{
		return m_underscores;
	}

	const std::vector<char>& Session::getLives() const
	{
		return m_lives;
	}

	const std::vector<char>& Session::getWrongGuesses() const
	{
		return m_wrongGuesses;
	}

	const std::string& Session::getWord() const
	{
		return m_word;
	}

	auto Session::guess(char guessLetter)
		-> std::expected<bool, Session::Error>
	{
		if (guessLetter < 'a' || guessLetter > 'z')
			return std::unexpected(Error::invaldInput);

		if (alreadyGuessed(guessLetter, *this))
			return std::unexpected(Error::alreadyGuessed);

		const auto indiciesMatched{ ::indiciesMatched(guessLetter, m_word) };

		if (std::empty(indiciesMatched))
		{
			m_lives.pop_back();
			m_wrongGuesses.push_back(guessLetter);
			return false;
		}

		updateUnderscores(indiciesMatched, guessLetter, m_underscores);
		return true;
	}

	Session::Session(std::string_view word, std::size_t maxGuesses)
		: m_word{ word }
		, m_underscores{ std::vector<char>(std::size(word), '_') }
		, m_lives{ std::vector<char>(maxGuesses, '+') }
	{
		assert(!std::empty(word) && "Session(): No word provided.");
	}
}

namespace Hangman
{
	auto randomIndex(auto array)
	{
		return Random::get(0z, std::ssize(array) - 1z);
	}

	std::string_view randomWord()
	{
		static const std::vector<std::string_view> s_wordList{
		"mystery",
		"broccoli",
		"account",
		"almost",
		"spaghetti",
		"opinion",
		"beautiful",
		"distance",
		"luggage"
		};

		return std::data(s_wordList)[randomIndex(s_wordList)];
	}

	std::string_view randomWord(const std::vector<std::string_view>& wordList)
	{
		return std::data(wordList)[randomIndex(wordList)];
	}

	char getInput(std::string_view prompt)
	{
		std::cout << prompt;
		char input{};
		std::cin >> input;
		Input::clearBuffer();
		Input::clearFailedExtraction();
		return input;
	}

	template <typename T>
	std::ostream& operator<<(
		std::ostream& out,
		const std::vector<T>& arr
		)
	{
		for (const auto& i : arr) out << i;
		return out;
	}

	// Helper function to allow different session versions with operator<<
	// overload to avoid ambiguity with iostream.
	template <typename Session>
	std::ostream& printSession(std::ostream& out, const Session& hangman)
	{
		out << "The word: " << hangman.getUnderscores() << "   "
			<< "Wrong guesses: " << hangman.getLives()
			<< hangman.getWrongGuesses();

		return out;
	}
}

namespace Hangman::V2
{
	std::ostream& operator<<(std::ostream& out, const Session& hangman)
	{
		return printSession(out, hangman);
	}

	void play()
	{
		Hangman::Session session{ *V2::session(Hangman::randomWord()) };
		printIntro();
		std::cout << session << '\n';
		gameLoop(session);
		printResult(session);
	}

	void play(Session& session)
	{
		printIntro();
		std::cout << session << '\n';
		gameLoop(session);
		printResult(session);
	}
}

namespace Hangman::V1
{
	Session::Session(std::string_view word, std::size_t maxGuesses)
		: m_word{ word }
		, m_underscores{ std::vector<char>(std::size(word), '_') }
		, m_lives{ std::vector<char>(maxGuesses, '+') }
	{
		assert(!std::empty(word) && "Session(): No word provided.");
	}

	const std::vector<char>& Session::getUnderscores() const
	{
		return m_underscores;
	}

	const std::vector<char>& Session::getLives() const
	{
		return m_lives;
	}

	const std::vector<char>& Session::getWrongGuesses() const
	{
		return m_wrongGuesses;
	}

	const std::string& Session::getWord() const
	{
		return m_word;
	}

	std::optional<bool> Session::guess(char guessLetter)
	{
		if (std::empty(m_word))
		{
			return false;
		}

		if (alreadyGuessed(guessLetter))
		{
			return {};
		}

		bool found{ false };
		for (auto i{ 0z }; i < std::ssize(m_word); ++i)
		{
			if (guessLetter == std::data(m_word)[i])
			{
				std::data(m_underscores)[i] = guessLetter;
				found = true;
			}
		}

		if (!found)
		{
			m_lives.pop_back();
			m_wrongGuesses.push_back(guessLetter);
		}

		return found;
	}

	bool Session::alreadyGuessed(char guessLetter) const
	{
		for (const auto& guess : getUnderscores())
		{
			if (guess == guessLetter)
			{
				return true;
			}
		}

		for (const auto& guess : getWrongGuesses())
		{
			if (guess == guessLetter)
			{
				return true;
			}
		}

		return false;
	}
}

namespace Hangman::V1
{
	std::ostream& operator<<(std::ostream& out, const Session& hangman)
	{
		return printSession(out, hangman);
	}

	void play(std::size_t maxGuesses)
	{
		std::cout << "Welcome to C++man (a variant of Hangman)\n";
		std::cout << "To win: guess the word.  To lose: run out of pluses.\n";
		std::cout << '\n';

		Hangman::V1::Session hangman{ Hangman::randomWord(), maxGuesses };

		while (Hangman::hasLives(hangman))
		{
			std::cout << hangman << '\n';

			if (won(hangman))
			{
				std::cout << "You Won!\n";
				return;
			}

			std::optional inputLetter{
				Hangman::getInput("Enter your next letter: ")
			};

			if (!inputLetter)
			{
				std::cout << "That wasn't a valid input.  Try again.\n";
				std::cout << '\n';
				continue;
			}

			std::optional validGuess{ hangman.guess(*inputLetter) };

			if (!validGuess)
			{
				std::cout << "You already guessed that.  Try again.\n";
				std::cout << '\n';
				continue;
			}

			std::cout << (*validGuess ? "Yes" : "No") << ", \'"
				<< *inputLetter << "\' is " << (*validGuess ? "in" : "not in")
				<< " the word!\n";

			std::cout << '\n';
		}

		std::cout << hangman << '\n';
		std::cout << "You lost!  The word was: " << hangman.getWord() << '\n';
	}
}

namespace
{
	void updateUnderscores(
		const std::vector<SignedIndex>& arr,
		char guessLetter,
		std::vector<char>& underscores
	)
	{
		for (const auto& i : arr)
			std::data(underscores)[i] = guessLetter;
	}

	bool alreadyGuessed(char guessLetter, const Hangman::Session& session)
	{
		for (const auto& guess : session.getUnderscores())
			if (guess == guessLetter) return true;

		for (const auto& guess : session.getWrongGuesses())
			if (guess == guessLetter) return true;

		return false;
	}

	bool inWord(char guessLetter, std::string_view word)
	{
		for (const auto& letter : word)
			if (guessLetter == letter) return true;

		return false;
	}

	void printIntro()
	{
		std::cout << "Welcome to C++man (a variant of Hangman)\n";
		std::cout << "To win: guess the word.  To lose: run out of pluses.\n";
		std::cout << '\n';
	}

	void printGuess(bool validGuess, char inputLetter)
	{
		std::cout << (validGuess ? "Yes" : "No") << ", \'" << inputLetter
			<< "\' is " << (validGuess ? "in" : "not in") << " the word!\n";
	}

	bool invalidInputError(ExpectedBE guess)
	{
		return guess.error() == Hangman::Session::Error::invaldInput;
	}

	bool alreadyGuessedError(ExpectedBE guess)
	{
		return guess.error() == Hangman::Session::Error::alreadyGuessed;
	}

	void handleInvalidGuess(ExpectedBE guess)
	{
		if (invalidInputError(guess))
			std::cout << "That wasn't a valid input.  Try again.\n";
		else if (alreadyGuessedError(guess))
			std::cout << "You already guessed that.  Try again.\n";
		else
			std::cout << "Unexpected Error\n";
	}

	void gameLoop(Hangman::Session& session)
	{
		while (Hangman::hasLives(session))
		{
			if (won(session)) return;

			const auto inputLetter{
				Hangman::getInput("Enter your next letter: ")
			};

			const auto guess{ session.guess(inputLetter) };

			if (!guess)
			{
				handleInvalidGuess(guess);
				continue;
			}

			printGuess(*guess, inputLetter);
			std::cout << '\n';
			std::cout << session << '\n';
		}
	}

	void printResult(const Hangman::Session& session)
	{
		if (!won(session))
		{
			std::cout << "You lost!  The word was: " << session.getWord()
				<< '\n';

			return;
		}

		std::cout << "You won!\n";
	}
}