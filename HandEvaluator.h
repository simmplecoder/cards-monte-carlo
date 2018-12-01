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

	float DetermineOdds_MonteCarlo_Multi(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Community, unsigned int _PlayerAmt, unsigned int _TrialsAmt);

	int DetermineValue_7Cards(const std::array<std::shared_ptr<Card>, 7>& _Hand);

	void RandomFill(std::vector<std::shared_ptr<Card>>& _Set, std::vector<std::shared_ptr<Card>>& _Dead, unsigned int _Target);

private:
	int HR[32487834] = {0};

	std::mt19937 MTGenerator;

	std::array <std::shared_ptr<Card>, 52> ReferenceDeck;

	void Initialize();

	int GetCardInt(std::string _CardTxt);
	int GetCardInt(const std::shared_ptr<Card>& _Card);
	void Get7CardsInt(const std::array<std::shared_ptr<Card>, 7>& _Hand, std::array<int, 7>& _CardInts);
};

