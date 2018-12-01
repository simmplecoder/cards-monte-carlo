#include <iostream>
#include <memory>

#include "Card.h"
#include "HandEvaluator.h"

int main()
{
	unsigned int PlayerAmt = 8;
	unsigned int Trials = 15000;
	unsigned int SimulationsLimit = 100;

	std::unique_ptr<HandEvaluator> Evaluator = std::make_unique<HandEvaluator>();
	
	std::array<std::shared_ptr<Card>, 2> PlayerHole = { std::make_shared<Card>(Suit::Spade, Rank::Three), 
														std::make_shared<Card>(Suit::Club, Rank::King) };

	std::array<std::shared_ptr<Card>, 5> Communal = { std::make_shared<Card>(Suit::Heart, Rank::King), 
													  nullptr, nullptr,nullptr,nullptr };

	for (unsigned int Count = 0; Count < SimulationsLimit; Count++)
	{
		std::cout << "Simulation " << Count << " - Chance of winning with " << PlayerHole[0]->GetInfo() << "," << PlayerHole[1]->GetInfo() << ": ";
		std::cout << Evaluator->DetermineOdds_MonteCarlo_Multi(PlayerHole, Communal, PlayerAmt, Trials) << "\n";
	}

    return 0;
}

