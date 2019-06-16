/*
    Copyright (C) 2019 George Cave - gcave@stablecoder.ca

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <tuple>
#include <type_traits>
#include <vector>

constexpr bool LessRandom = false;

enum Suit {
  Hearts = 0,
  Spades,
  Diamonds,
  Clubs,
};
constexpr uint64_t NumSuits = 4;

enum Value {
  Ace = 0,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Jack,
  Queen,
  King,
};
constexpr uint64_t NumValues = 13;
constexpr uint64_t DeckSize = NumSuits * NumValues;

using Card = uint64_t;

constexpr uint64_t shiftSmaller(uint64_t value, int numBits) {
  if constexpr (std::endian::native == std::endian::little) {
    return value >> numBits;
  } else {
    return value << numBits;
  }
}

constexpr uint64_t shiftLarger(uint64_t value, int numBits) {
  if constexpr (std::endian::native == std::endian::little) {
    return value << numBits;
  } else {
    return value >> numBits;
  }
}

constexpr Suit getSuit(Card card) {
  if (card <
      shiftLarger(1,
                  NumValues +
                      NumValues * static_cast<std::underlying_type<Suit>::type>(
                                      Suit::Hearts))) {
    return Suit::Hearts;
  } else if (card <
             shiftLarger(1,
                         NumValues +
                             NumValues *
                                 static_cast<std::underlying_type<Suit>::type>(
                                     Suit::Spades))) {
    return Suit::Spades;
  } else if (card <
             shiftLarger(1,
                         NumValues +
                             NumValues *
                                 static_cast<std::underlying_type<Suit>::type>(
                                     Suit::Diamonds))) {
    return Suit::Diamonds;
  } else {
    return Suit::Clubs;
  }
}

constexpr Value getValue(Card card) {
  if (card == 0) { // @todo Convert to C++20 contract
    std::abort();
  }

  while (card >= shiftLarger(1, NumValues)) {
    card = shiftSmaller(card, NumValues);
  }

  int ret = 0;
  auto testCard = 1;
  while (testCard != card) {
    testCard = shiftLarger(testCard, 1);
    ++ret;
  }

  return static_cast<Value>(ret);
}

class Deck {
public:
  Deck() {
    if constexpr (std::endian::native == std::endian::big) {
      deck = 0xFFFFFFFFFFFFF000;
    }
  }

  std::size_t cardsLeft() const { return numCards + peekedCards.size(); }

  void shuffle() {
    for (auto it : peekedCards) {
      redeck(it);
    }
  }

  void redeck(Card card) {
    ++numCards;
    deck |= card;
  }

  Card drawCard() {
    if (cardsLeft() == 0)
      std::abort();

    if (!peekedCards.empty()) {
      auto card = peekedCards.front();
      peekedCards.erase(peekedCards.begin(), peekedCards.begin() + 1);
      return card;
    }

    return getRandomCard();
  }

  std::vector<Card> peekCards(int count) {
    if (numCards < count)
      std::abort();

    for (int i = peekedCards.size(); i < count; ++i) {
      peekedCards.emplace_back(getRandomCard());
    }

    return std::vector<Card>{peekedCards.begin(), peekedCards.begin() + count};
  }

private:
  uint64_t getRandomCard() {
    if constexpr (LessRandom)
      std::srand(std::time(nullptr));
    auto rand = std::rand();

    rand = (rand % numCards) + 1;

    Card card{1};
    for (; rand > 0; card = shiftLarger(card, 1)) {
      if ((card & deck) != 0)
        --rand;
    }

    card = shiftSmaller(card, 1);
    deck = (deck xor card);
    --numCards;
    return card;
  }

  Card deck{0x000FFFFFFFFFFFFF};
  std::size_t numCards{52};
  std::vector<Card> peekedCards{};
};

std::string suitStr(Card card) {
  auto suit = getSuit(card);
  switch (suit) {
  case Suit::Hearts:
    return "Hearts";
  case Suit::Spades:
    return "Spades";
  case Suit::Diamonds:
    return "Diamonds";
  case Suit::Clubs:
    return "Clubs";
  }
}

std::string valueStr(Card card) {
  auto value = getValue(card);
  switch (value) {
  case Value::Ace:
    return "Ace";
  case Value::Two:
    return "Two";
  case Value::Three:
    return "Three";
  case Value::Four:
    return "Four";
  case Value::Five:
    return "Five";
  case Value::Six:
    return "Six";
  case Value::Seven:
    return "Seven";
  case Value::Eight:
    return "Eight";
  case Value::Nine:
    return "Nine";
  case Value::Ten:
    return "Ten";
  case Value::Jack:
    return "Jack";
  case Value::Queen:
    return "Queen";
  case Value::King:
    return "King";
  }
}

#include <iostream>

void printHand(Card hand) {
  Card card{1};
  for (int i = 0; i < DeckSize; ++i) {
    if ((card & hand) != 0) {
      std::cout << "  " << suitStr(card) << ", " << valueStr(card) << '\n';
    }
    card = shiftLarger(card, 1);
  }
}

int main(int argc, char **argv) {
  Deck dealerDeck;

  Card playerHand{0};
  Card playerHand2{0};

  std::cout << "Peeking\n";
  for (auto it : dealerDeck.peekCards(5)) {
    std::cout << "  " << suitStr(it) << ", " << valueStr(it) << '\n';
  }

  std::cout << "Drawing Cards:\n";
  for (int i = 0; i < 26; ++i) {
    auto drawn = dealerDeck.drawCard();
    std::cout << "  " << suitStr(drawn) << ", " << valueStr(drawn) << '\n';
    playerHand |= drawn;
    drawn = dealerDeck.drawCard();
    std::cout << "  " << suitStr(drawn) << ", " << valueStr(drawn) << '\n';
    playerHand2 |= drawn;
  }

  std::cout << "Player 1 Hand:\n";
  printHand(playerHand);
  std::cout << "Player 2 Hand:\n";
  printHand(playerHand2);

  std::cout << std::endl;
}