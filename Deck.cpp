#include "Card.h"
#include "Deck.h"

#include <random>
#include <iostream>
#include <algorithm>

Deck::Deck()
{
	cards.reserve(52);
	Refill();
	Shuffle();
}
void Deck::Refill()
{
	cards.clear();

	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int VIndex = 0; VIndex < 13; VIndex++)
		{
			cards.emplace_back(static_cast<Suit>(SIndex), static_cast<Rank>(VIndex));
		}
	}
}

void Deck::Shuffle()
{
	std::mt19937_64 twister{std::random_device{}()};
	std::shuffle(cards.begin(), cards.end(), twister);
}

Card Deck::Draw()
{
	auto result = cards.back();
	cards.pop_back();
	return result;
}

std::vector<Card> Deck::DrawMulti(std::size_t amount)
{
	return {cards.rbegin(), std::next(cards.rbegin(), amount)};
}

void Deck::Print() const
{
	std::cout << "Deck's information: \n";
	std::cout << "(Remaining cards): " << cards.size() << "\n\n";

	unsigned int Count = 0;
	for (auto const& card : cards)//unsigned int CIndex = 0; CIndex < Cards.size(); CIndex++)
	{
		std::cout << card.to_string() << "/";
		Count++;

		if (Count >= 13)
		{
			std::cout << "\n";
			Count = 0;
		}
	}

	std::cout << "\n";
}