 cmake_minimum_required(VERSION 3.2)
 project(monte_carlo)

 add_executable(monte_carlo Main.cpp HandEvaluator.cpp Deck.cpp Card.cpp)
 configure_file(HandRanks.dat ${CMAKE_CURRENT_BINARY_DIR}/HandRanks.dat)