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

void HandEvaluator::RandomFill(std::vector<Card>& set, std::vector<Card>& dead, std::size_t target)
{
	std::size_t required_amount = target - set.size();
	std::uniform_int_distribution<unsigned int> CardsDistribution(0, 51);

	auto generate_card = [&]() {
		while (true)
		{
			auto picked_card = ReferenceDeck[CardsDistribution(MTGenerator)];
			bool alive = std::find(dead.begin(), dead.end(), picked_card) == dead.end();

			if (alive)
			{
				return picked_card;
			}
		}
	};

	std::generate_n(std::back_inserter(set), required_amount, generate_card);
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

float HandEvaluator::DetermineOdds_MonteCarlo_Multi(std::array<Card, 2> hole, const std::vector<Card> community, unsigned int _PlayerAmt, unsigned int _TrialsAmt)
{
	unsigned int PlayerScore;
	std::array<Card, 7> PlayerHand{};

	std::vector<unsigned int> OpponentScores;
	std::vector<std::array<Card, 7>> OpponentHands;
	std::vector<std::vector<Card>> Rand_OpponentsHole;

	std::vector<Card> dead;
	std::vector<Card> rand_community(community);

	unsigned int Win = 0, Draw = 0, GameCount = 0, ExistingCount = 0;

	OpponentScores.reserve(_PlayerAmt - 1);
	OpponentHands.reserve(_PlayerAmt - 1);
	Rand_OpponentsHole.reserve(_PlayerAmt - 1);

	for (unsigned int Index = 0; Index < _PlayerAmt; Index++)
	{
		OpponentScores.push_back(0);
		OpponentHands.emplace_back();
		Rand_OpponentsHole.emplace_back();
	}

	//Insert known information (Player's hole cards and current Community cards)

	PlayerHand[0] = hole[0];
	PlayerHand[1] = hole[1];

	for (unsigned int Index = 0; Index < ExistingCount; Index++)
	{
		PlayerHand[2 + Index] = rand_community[Index];

		for (auto &OppoHand : OpponentHands)
			OppoHand[Index] = rand_community[Index];
	}

	dead.insert(dead.end(), hole.begin(), hole.end());
	dead.insert(dead.end(), rand_community.begin(), rand_community.end());

	for (unsigned int TrialIndex = 0; TrialIndex < _TrialsAmt; TrialIndex++)
	{
		//Add random cards (excluding dead cards) into Community until it reached 5 cards
		RandomFill(rand_community, dead, 5);

		dead.insert(dead.end(), rand_community.begin() + ExistingCount, rand_community.end());

		for (auto &OppoHole : Rand_OpponentsHole)
		{
			RandomFill(OppoHole, dead, 2);
			dead.insert(dead.end(), OppoHole.begin(), OppoHole.end());
		}

		//Add communal cards into both player's and opponent's hand
		for (unsigned int Index = ExistingCount, Maximum = rand_community.size(); Index < Maximum; Index++)
			PlayerHand[Index + 2] = rand_community[Index];

		for (unsigned int Index = 0; Index < _PlayerAmt - 1; Index++)
		{
			OpponentHands[Index][ExistingCount] = Rand_OpponentsHole[Index][0];
			OpponentHands[Index][ExistingCount + 1] = Rand_OpponentsHole[Index][1];

			for (unsigned int CommIndex = ExistingCount, Maximum = rand_community.size(); CommIndex < Maximum; CommIndex++)
				OpponentHands[Index][CommIndex + 2] = rand_community[CommIndex];
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
		dead.erase(dead.begin() + ExistingCount, dead.end());
		rand_community.erase(rand_community.begin() + ExistingCount, rand_community.end());

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
