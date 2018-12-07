#include "HandEvaluator.h"
#include "Card.h"
#include <cstring>
#include <random>
#include <algorithm>
#include <chrono>

HandEvaluator::HandEvaluator()
{
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	std::seed_seq seq{static_cast<unsigned int>(now), std::random_device{}(), 42u};
	MTGenerator.seed(seq);
	Initialize();
}

void HandEvaluator::Initialize()
{
	HR.resize(32487834);
	FILE * fin = fopen("HandRanks.dat", "rb");
	(void)fread(HR.data(), HR.size(), 1, fin);
	fclose(fin);

	int ReferenceIndex = 0;

	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int VIndex = 0; VIndex < 13; VIndex++)
		{
			ReferenceDeck[ReferenceIndex] = Card(static_cast<Suit>(SIndex), static_cast<Rank>(VIndex));
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

std::array<int, 7> HandEvaluator::Get7CardsInt(const std::array<Card, 7>& hand)
{
	std::array<int, 7> cards_as_ints = {};
	std::transform(hand.begin(), hand.end(), cards_as_ints.begin(), [](Card card) {
		return static_cast<int>(card);
	});

	return cards_as_ints;
}

float HandEvaluator::DetermineOdds_MonteCarlo_Multi(std::array<Card, 2> hole, const std::vector<Card> community, unsigned int _PlayerAmt, unsigned int _TrialsAmt)
{
	unsigned int PlayerScore;
	std::array<Card, 7> PlayerHand{hole[0], hole[1]};

	std::vector<unsigned int> OpponentScores(_PlayerAmt);
	std::vector<std::array<Card, 7>> OpponentHands(_PlayerAmt);
	std::vector<std::vector<Card>> Rand_OpponentsHole(_PlayerAmt);

	std::vector<Card> dead;
	std::vector<Card> rand_community(community);

	unsigned int Win = 0, Draw = 0, GameCount = 0;
	auto ExistingCount = rand_community.size();

	for (unsigned int Index = 0; Index < ExistingCount; Index++)
	{
		PlayerHand[2 + Index] = rand_community[Index];

		for (auto& OppoHand : OpponentHands)
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

int HandEvaluator::DetermineValue_7Cards(const std::array<Card, 7>& _Hand)
{
	std::array<int, 7> CardInts = Get7CardsInt(_Hand);

	return HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]];
}
