#pragma once

#include <expected>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

// To choose which version is the default.
namespace Hangman::inline V2 {}

namespace Hangman::V2
{
	class Session
	{
	public:
		enum class Error
		{
			unknown,
			alreadyGuessed,
			invaldInput,
			noWordProvided,

			maxErrors
		};

		friend auto session(std::string_view word, std::size_t maxGuesses)
			-> std::expected<Session, Session::Error>;

		const std::vector<char>& getUnderscores() const;
		const std::vector<char>& getLives() const;
		const std::vector<char>& getWrongGuesses() const;
		const std::string& getWord() const;
		auto guess(char guessLetter) -> std::expected<bool, Error>;

	private:
		std::string m_word{};
		std::vector<char> m_underscores{};
		std::vector<char> m_lives{};
		std::vector<char> m_wrongGuesses{};
		Session(std::string_view word, std::size_t maxGuesses);
	};

	auto session(std::string_view word, std::size_t maxGuesses = 6uz)
		-> std::expected<Session, Session::Error>;
}

namespace Hangman::V1
{
	class Session
	{
	public:
		Session(std::string_view word, std::size_t maxGuesses = 6uz);

		const std::vector<char>& getUnderscores() const;
		const std::vector<char>& getLives() const;
		const std::vector<char>& getWrongGuesses() const;
		const std::string& getWord() const;

		std::optional<bool> guess(char guessLetter);

	private:
		std::string m_word{};
		std::vector<char> m_underscores{};
		std::vector<char> m_lives{};
		std::vector<char> m_wrongGuesses{};

		bool alreadyGuessed(char guessLetter) const;
	};
}

namespace Hangman
{
	template <typename Session>
	bool hasLives(const Session& session)
	{
		return std::ssize(session.getLives()) > 0z;
	}

	template <typename Session>
	bool won(const Session& session)
	{
		if (!hasLives(session))
		{
			return false;
		}

		for (const auto& underscore : session.getUnderscores())
		{
			if (underscore == '_')
			{
				return false;
			}
		}

		return true;
	}

	std::string_view randomWord();
	std::string_view randomWord(const std::vector<std::string_view>& wordList);
	char getInput(std::string_view prompt = "");
}

namespace Hangman::V2
{
	std::ostream& operator<<(std::ostream& out, const Session& session);
	void play();
	void play(Session& session);
}

namespace Hangman::V1
{
	std::ostream& operator<<(std::ostream& out, const Session& hangman);
	void play(std::size_t maxGuesses = 6uz);
}