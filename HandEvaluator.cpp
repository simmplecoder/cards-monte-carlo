#include "HandEvaluator.h"
#include "Card.h"
#include <cstring>

HandEvaluator::HandEvaluator()
{
	Initialize();
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

void HandEvaluator::Initialize()
{
	std::memset(HR, 0, sizeof(HR));
	FILE * fin = fopen("HandRanks.dat", "rb");
	size_t bytesread = fread(HR, sizeof(HR), 1, fin);
	fclose(fin);

	int ReferenceIndex = 0;

	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int VIndex = 0; VIndex < 13; VIndex++)
		{
			ReferenceDeck[ReferenceIndex] = std::make_shared<Card>(static_cast<Suit>(SIndex), static_cast<Rank>(VIndex));
			ReferenceIndex++;
		}
	}
}

void HandEvaluator::RandomFill(std::vector<std::shared_ptr<Card>>& _Set, std::vector<std::shared_ptr<Card>>& _Dead, unsigned int _Target)
{
	//Add the cards that are currently in Set as dead cards
	for (auto const& CardInSet : _Set)
	{
		if (CardInSet == nullptr)
			break;

		_Dead.push_back(CardInSet);
	}

	bool IsDead;
	unsigned int RequiredAmt = _Target - _Set.size();

	//std::shared_ptr<Card> RandomCard;
	unsigned int CardIndex = 0;
	std::uniform_int_distribution<unsigned int> CardsDistribution(0, 51);

	for (unsigned int Index = 0; Index < RequiredAmt; Index++)
	{
		while (true)
		{
			IsDead = false;
			CardIndex = CardsDistribution(MTGenerator);

			for (auto const& Dead : _Dead)
			{
				if (ReferenceDeck[CardIndex]->GetRank() == Dead->GetRank() && ReferenceDeck[CardIndex]->GetSuit() == Dead->GetSuit())
				{
					IsDead = true;
					break;
				}
			}

			if (!IsDead)
			{
				_Set.push_back(ReferenceDeck[CardIndex]);
				break;
			}
		}
	}
}

int HandEvaluator::GetCardInt(std::string _CardTxt)
{
	int RankInt = -1, SuitInt = -1;

	char SuitType = _CardTxt.back();
	std::string RankType = _CardTxt.substr(0, _CardTxt.size() - 1);

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
		return -1;

	return (RankInt * 4) + SuitInt + 1;

}

int HandEvaluator::GetCardInt(const std::shared_ptr<Card>& _Card)
{
	return (_Card->get_rank() * 4) + _Card->get_suit() + 1;
}

void HandEvaluator::Get7CardsInt(const std::array<std::shared_ptr<Card>, 7>& _Hand, std::array<int, 7>& _CardInts)
{
	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
		_CardInts[Index] = GetCardInt(_Hand[Index]);
}

float HandEvaluator::DetermineOdds_MonteCarlo_Multi(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Community, unsigned int _PlayerAmt, unsigned int _TrialsAmt)
{
	unsigned int PlayerScore;
	std::array<std::shared_ptr<Card>, 7> PlayerHand;

	std::vector<unsigned int> OpponentScores;
	std::vector<std::array<std::shared_ptr<Card>, 7>> OpponentHands;
	std::vector<std::vector<std::shared_ptr<Card>>> Rand_OpponentsHole;

	std::vector<std::shared_ptr<Card>> Dead;
	std::vector<std::shared_ptr<Card>> Rand_Community;

	unsigned int Win = 0, Draw = 0, GameCount = 0, ExistingCount = 0;

	OpponentScores.reserve(_PlayerAmt - 1);
	OpponentHands.reserve(_PlayerAmt - 1);
	Rand_OpponentsHole.reserve(_PlayerAmt - 1);

	for (unsigned int Index = 0; Index < _PlayerAmt; Index++)
	{
		OpponentScores.push_back(0);
		OpponentHands.push_back({ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr });
		Rand_OpponentsHole.emplace_back({});
	}

	//Insert known information (Player's hole cards and current Community cards)
	for (auto const& Card : _Community)
	{
		if (Card != nullptr)
		{
			Rand_Community.push_back(Card);
			ExistingCount++;
		}
	}

	PlayerHand[0] = _Hole[0];
	PlayerHand[1] = _Hole[1];

	for (unsigned int Index = 0; Index < ExistingCount; Index++)
	{
		PlayerHand[2 + Index] = Rand_Community[Index];

		for (auto &OppoHand : OpponentHands)
			OppoHand[Index] = Rand_Community[Index];
	}

	Dead.insert(Dead.end(), _Hole.begin(), _Hole.end());
	Dead.insert(Dead.end(), Rand_Community.begin(), Rand_Community.end());

	for (unsigned int TrialIndex = 0; TrialIndex < _TrialsAmt; TrialIndex++)
	{
		//Add random cards (excluding dead cards) into Community until it reached 5 cards
		RandomFill(Rand_Community, Dead, 5);

		Dead.insert(Dead.end(), Rand_Community.begin() + ExistingCount, Rand_Community.end());

		for (auto &OppoHole : Rand_OpponentsHole)
		{
			RandomFill(OppoHole, Dead, 2);
			Dead.insert(Dead.end(), OppoHole.begin(), OppoHole.end());
		}

		//Add communal cards into both player's and opponent's hand
		for (unsigned int Index = ExistingCount, Maximum = Rand_Community.size(); Index < Maximum; Index++)
			PlayerHand[Index + 2] = Rand_Community[Index];

		for (unsigned int Index = 0; Index < _PlayerAmt - 1; Index++)
		{
			OpponentHands[Index][ExistingCount] = Rand_OpponentsHole[Index][0];
			OpponentHands[Index][ExistingCount + 1] = Rand_OpponentsHole[Index][1];

			for (unsigned int CommIndex = ExistingCount, Maximum = Rand_Community.size(); CommIndex < Maximum; CommIndex++)
				OpponentHands[Index][CommIndex + 2] = Rand_Community[CommIndex];
		}

		//Get scores for Player's Hand and Opponents' Hand
		PlayerScore = DetermineValue_7Cards(PlayerHand);

		for (unsigned int Index = 0; Index < _PlayerAmt - 1; Index++)
			OpponentScores[Index] = DetermineValue_7Cards(OpponentHands[Index]);

		bool HasPlayerWin = true;
		for (auto &OppoScore : OpponentScores)
		{
			if (PlayerScore < OppoScore)
			{
				HasPlayerWin = false;
				break;
			}
		}

		if (HasPlayerWin)
			Win++;

		GameCount++;

		//Erase all random info
		Dead.erase(Dead.begin() + ExistingCount, Dead.end());
		Rand_Community.erase(Rand_Community.begin() + ExistingCount, Rand_Community.end());

		for (auto &OppoHole : Rand_OpponentsHole)
			OppoHole.clear();
	}

	return (((float)Win) + ((float)Draw) / 2.0f) / ((float)GameCount) * 100.0f;
}

int HandEvaluator::DetermineValue_7Cards(const std::array<std::shared_ptr<Card>, 7>& _Hand)
{
	std::array<int, 7> CardInts = {};
	Get7CardsInt(_Hand, CardInts);

	return HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]];
}
