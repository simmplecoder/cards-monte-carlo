#pragma once
#include <array>
#include <vector>
#include <random>

#include "Card.h"

class HandEvaluator
{
public:
	HandEvaluator();

	float DetermineOdds_MonteCarlo_Multi(std::array<Card, 2> _Hole, std::vector<Card> community, unsigned int _PlayerAmt, unsigned int _TrialsAmt);

	int DetermineValue_7Cards(const std::array<Card, 7>& _Hand);

	void RandomFill(std::vector<Card>& set, std::vector<Card>& dead, std::size_t target);

private:
	std::vector<int> HR;
	std::mt19937 MTGenerator;
	std::array <Card, 52> ReferenceDeck;
	void Initialize();
	std::array<int, 7> Get7CardsInt(const std::array<Card, 7>& _Hand);
};

