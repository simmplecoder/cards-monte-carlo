#pragma once

#include <vector>

class Card;

class Deck
{
public:
	Deck();

	void Refill();
	void Shuffle();

	Card Draw();
	std::vector<Card> DrawMulti(std::size_t amount);//std::vector<std::shared_ptr<Card>> DrawMultiple(unsigned int _Amt);

	void Print() const;
	std::vector<Card> GetCards() const {
		return cards;
	}

	std::size_t size() const {
		return cards.size();
	}

private:
	std::vector<Card> cards;

};

