#pragma once

#include "Hangman.h"
#include <cassert>
#include <expected>
#include <optional>

namespace Test::Hangman::V2
{
	void session()
	{
		namespace Hangman = ::Hangman::V2;

		std::expected validSession{ Hangman::session("mystery") };
		std::expected invalidSession{ Hangman::session("") };

		assert(validSession);
		assert(!invalidSession);

		assert(invalidSession.error()
			== Hangman::Session::Error::noWordProvided);
	}

	// Assuming an input function handles invalid inputs.
	void guess()
	{
		namespace Hangman = ::Hangman::V2;

		Hangman::Session hangman{ *Hangman::session("mystery") };
		assert(*hangman.guess('a') == false);
		assert(*hangman.guess('b') == false);

		assert(hangman.guess('&').error()
			== Hangman::Session::Error::invaldInput);

		assert(hangman.guess('b').error()
			== Hangman::Session::Error::alreadyGuessed);

		Hangman::Session hangman1{ *Hangman::session("m") };
		assert(*hangman1.guess('m') == true);
		assert(*hangman1.guess('a') == false);

		Hangman::Session hangman2{ *Hangman::session("my") };
		assert(*hangman2.guess('m') == true);
		assert(*hangman2.guess('a') == false);
	}

	void hasLives()
	{
		namespace Hangman = ::Hangman;

		Hangman::Session hangman{ *Hangman::V2::session("mystery", 1uz) };
		assert(Hangman::hasLives(hangman) == true);
		assert(hangman.guess('a'));
		assert(Hangman::hasLives(hangman) == false);
	}

	void won()
	{
		namespace Hangman = ::Hangman;

		Hangman::Session hangman1{ *Hangman::V2::session("m") };
		assert(Hangman::won(hangman1) == false);
		assert(hangman1.guess('m'));
		assert(Hangman::won(hangman1) == true);

		Hangman::Session hangman2{ *Hangman::V2::session("my") };
		assert(Hangman::won(hangman2) == false);
		assert(hangman2.guess('m'));
		assert(hangman2.guess('y'));
		assert(Hangman::won(hangman2) == true);
	}
}

namespace Test::Hangman::V1
{
	// Assuming an input function handles invalid inputs.
	void guess()
	{
		namespace Hangman = ::Hangman::V1;

		Hangman::Session hangman{ "mystery" };
		assert(*hangman.guess('a') == false);
		assert(*hangman.guess('b') == false);
		assert(hangman.guess('b') == std::nullopt);

		//Hangman::Session hangman0{ "" };
		//assert(hangman0.guess('a') == false);

		Hangman::Session hangman1{ "m" };
		assert(*hangman1.guess('m') == true);
		assert(*hangman1.guess('a') == false);

		Hangman::Session hangman2{ "my" };
		assert(*hangman2.guess('m') == true);
		assert(*hangman2.guess('a') == false);
	}

	void hasLives()
	{
		namespace Hangman = ::Hangman;

		Hangman::V1::Session hangman{ "mystery", 1uz };
		assert(Hangman::hasLives(hangman) == true);
		assert(hangman.guess('a'));
		assert(Hangman::hasLives(hangman) == false);
	}

	void won()
	{
		namespace Hangman = ::Hangman;

		//Hangman::Session hangman0{ "" };
		//assert(Hangman::won(hangman0) == true);

		Hangman::V1::Session hangman1{ "m" };
		assert(Hangman::won(hangman1) == false);
		assert(hangman1.guess('m'));
		assert(Hangman::won(hangman1) == true);

		Hangman::V1::Session hangman2{ "my" };
		assert(Hangman::won(hangman2) == false);
		assert(hangman2.guess('m'));
		assert(Hangman::won(hangman2) == false);
		assert(hangman2.guess('y'));
		assert(Hangman::won(hangman2) == true);
	}
}