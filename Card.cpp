#include "Card.h"

Card::Card(std::string text)
{
	int RankInt = 0;
	int SuitInt = 0;

	char SuitType = text.back();
	std::string RankType = text.substr(0, text.size() - 1);

	switch (SuitType)
	{
		case 's':
			SuitInt = static_cast<int>(Suit::Spade);
			break;
		case 'h':
			SuitInt = static_cast<int>(Suit::Heart);
			break;
		case 'd':
			SuitInt = static_cast<int>(Suit::Diamond);
			break;
		case 'c':
			SuitInt = static_cast<int>(Suit::Club);
			break;
		default:
			SuitInt = -1;
			break;
	}

	if (RankType == "J")
		RankInt = static_cast<int>(Rank::Jack);
	else if (RankType == "Q")
		RankInt = static_cast<int>(Rank::Queen);
	else if (RankType == "K")
		RankInt = static_cast<int>(Rank::King);
	else if (RankType == "A")
		RankInt = static_cast<int>(Rank::Ace);
	else
		RankInt = std::stoi(RankType) - 2;

	if (SuitInt == -1 || RankInt == -1)
		throw std::invalid_argument("invalid card text");

	rank = static_cast<Rank>(RankInt);
	suit = static_cast<Suit>(SuitInt);
}

std::string Card::to_string() const {
	std::string info;
	switch (static_cast<int>(rank))
	{
		case 0: info += "2";
			break;
		case 1: info += "3";
			break;
		case 2: info += "4";
			break;
		case 3: info += "5";
			break;
		case 4: info += "6";
			break;
		case 5: info += "7";
			break;
		case 6: info += "8";
			break;
		case 7: info += "9";
			break;
		case 8: info += "10";
			break;
		case 9: info += "J";
			break;
		case 10: info += "Q";
			break;
		case 11: info += "K";
			break;
		case 12: info += "A";
			break;
	}

	switch (static_cast<int>(suit))
	{
		case 0: info += "c";
			break;
		case 1: info += "d";
			break;
		case 2: info += "h";
			break;
		case 3: info += "s";
			break;
	}
	return info;
}