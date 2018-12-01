#include "Card.h"

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