#pragma once
#include <memory>
#include <string>

enum class Rank { Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };
enum class Suit { Club, Diamond, Heart, Spade };

class Card
{
public:
	Card() = default;

	Card(Suit suit, Rank rank) :
		suit(suit),
		rank(rank)
	{}

	Card(std::string text);

	Suit get_suit() const { return suit; };
	Rank get_rank() const { return rank; };

	bool operator>(const Card& other) const {
		if (rank == other.rank)
			return suit > other.suit;

		return rank > other.rank;
	}

	bool operator==(const Card& other) const {
		return suit == other.suit && rank == other.rank;
	}

	std::string to_string() const;

	explicit operator int() {
        return (static_cast<int>(rank) * 4) + static_cast<int>(suit) + 1;
	}
private:
	Suit suit;
	Rank rank;
};

