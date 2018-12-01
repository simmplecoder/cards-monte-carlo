#pragma once
#include <memory>
#include <string>

enum class Rank { Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };
enum class Suit { Club, Diamond, Heart, Spade };

class Card
{
public:
	Suit get_suit() const { return suit; };
	Rank get_rank() const { return rank; };

	int int_suit() const { return static_cast<int>(suit); }
	int int_rank() const { return static_cast<int>(rank); }

	bool operator>(const Card& other) const {
		if (rank == other.rank)
			return suit > other.suit;

		return rank > other.rank;
	}

	std::string to_string() const;
private:
	Suit suit;
	Rank rank;
};

