#pragma once
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>
#include <memory>

class Card;

class HandEvaluator
{
public:
	HandEvaluator();

	float DetermineOdds_MonteCarlo_Multi(std::array<Card, 2> _Hole, std::vector<Card> community, unsigned int _PlayerAmt, unsigned int _TrialsAmt);

	int DetermineValue_7Cards(const std::array<std::shared_ptr<Card>, 7>& _Hand);

	void RandomFill(std::vector<Card>& set, std::vector<Card>& dead, std::size_t target);

private:
	int HR[32487834] = {0};

	std::mt19937 MTGenerator;

	std::array <Card, 52> ReferenceDeck;

	void Initialize();

	int GetCardInt(std::string _CardTxt);
	int GetCardInt(const std::shared_ptr<Card>& _Card);
	void Get7CardsInt(const std::array<std::shared_ptr<Card>, 7>& _Hand, std::array<int, 7>& _CardInts);
};

