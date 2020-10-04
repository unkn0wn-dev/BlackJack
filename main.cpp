#include <iostream>
#include <string>
#include <array>
#include <chrono>
#include <thread>
#include <random>
#include <limits>
#include <ctime>

constexpr int maximumScore{ 21 };
constexpr int minimumDealerScore{ 17 };
constexpr __int64 stream_limit{ (std::numeric_limits<std::streamsize>::max)() };

enum CardSuit
{
    SUIT_CLUB,
    SUIT_DIAMOND,
    SUIT_HEART,
    SUIT_SPADE,

    MAX_SUITS
};

enum CardRank
{
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_9,
    RANK_10,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING,
    RANK_ACE,

    MAX_RANKS
};

enum class BlackJackResult
{
    loss,
    win,
    tie
};

struct Card
{
    CardRank rank;
    CardSuit suit;
};

struct Player
{
    int score;
    bool bust;
    short aces;
};

void printCard(const Card& card)
{
    switch (card.rank)
    {
    case CardRank::RANK_2:
        std::cout << '2';
        break;
    case CardRank::RANK_3:
        std::cout << '3';
        break;
    case CardRank::RANK_4:
        std::cout << '4';
        break;
    case CardRank::RANK_5:
        std::cout << '5';
        break;
    case CardRank::RANK_6:
        std::cout << '6';
        break;
    case CardRank::RANK_7:
        std::cout << '7';
        break;
    case CardRank::RANK_8:
        std::cout << '8';
        break;
    case CardRank::RANK_9:
        std::cout << '9';
        break;
    case CardRank::RANK_10:
        std::cout << 'T';
        break;
    case CardRank::RANK_JACK:
        std::cout << 'J';
        break;
    case CardRank::RANK_QUEEN:
        std::cout << 'Q';
        break;
    case CardRank::RANK_KING:
        std::cout << 'K';
        break;
    case CardRank::RANK_ACE:
        std::cout << 'A';
        break;
    default:
        std::cout << '?';
        break;
    }

    switch (card.suit)
    {
    case CardSuit::SUIT_CLUB:
        std::cout << 'C';
        break;
    case CardSuit::SUIT_DIAMOND:
        std::cout << 'D';
        break;
    case CardSuit::SUIT_HEART:
        std::cout << 'H';
        break;
    case CardSuit::SUIT_SPADE:
        std::cout << 'S';
        break;
    default:
        std::cout << '?';
        break;
    }
    std::cout << '\n';
}

using deck_type = std::array<Card, 52>;

deck_type createDeck()
{
    deck_type deck;

    int index{ 0 };
    for (int i{ 0 }; i < MAX_SUITS; ++i)
    {
        for (int e{ 0 }; e < MAX_RANKS; ++e)
        {
            deck[index].rank = static_cast<CardRank>(e);
            deck[index].suit = static_cast<CardSuit>(i);
            ++index;
        }
    }
    return deck;
}

void printDeck(const deck_type& deck)
{
    for (auto& card : deck)
        printCard(card);
}

void shuffleDeck(deck_type& deck)
{
    static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
    std::shuffle(deck.begin(), deck.end(), mt);
}

int getCardValue(const Card& input)
{
    if (input.rank < 10)
        return (static_cast<int>(input.rank) + 2);
    switch (input.rank)
    {
    //Fallthrough Intended
    case RANK_JACK:
    case RANK_QUEEN:
    case RANK_KING:
        return 10;
        break;
    case RANK_ACE:
        return 11;
        break;
    default:
        return 0;
    }
}

bool playerTurn(Player& player, const deck_type& deck, int cardIndex)
{
    int& score{ player.score };
    using namespace std;
    cout << "You have a score of " << score << '\n';
    while (score <= maximumScore)
    {
        std::string choice;
        while (true)
        {
            cout << "Would you like to (h)it or (s)tand? -> ";
            getline(cin, choice);
            if (choice != "h" && choice != "s" )
            {
                continue;
            }
            break;
        }
        while (score <= maximumScore)
        {
            if (choice == "h")
            {
                int card_score = getCardValue(deck[cardIndex++]);
                if (card_score == 11)
                    ++player.aces;
                score += card_score;
                std::cout << "\nYou have a score of " << score << '\n';
                break;
            }
            else if (choice == "s")
            {
                cout << "\nYou stand with a score of " << score << '\n';
                return false;
            }
        }
    }
    if (player.aces > 0)
    {
        cout << "You have an ace.\n";
        player.aces -= 1;
        player.score -= 10;
        return playerTurn(player, deck, cardIndex);
    }
    cout << "Bust!\n";
    return true;
}

bool dealerTurn(Player& dealer, const deck_type& deck, int cardIndex)
{
    int& score{ dealer.score };
    std::cout << "Dealer score is " << score << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    while (score < minimumDealerScore)
    {
        int card_score{ getCardValue(deck[cardIndex++]) };
        if (card_score == 11)
            ++dealer.aces;
        std::cout << "Dealer hits\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        score += card_score;
        std::cout << "Dealer score is " << score << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    if (score <= maximumScore)
    {
        std::cout << "Dealer stands with score of " << score << '\n';
        return false;
    }
    if (dealer.aces > 0)
    {
        std::cout << "Dealer has an ace.\n";
        dealer.aces -= 1;
        dealer.score -= 10;
        return dealerTurn(dealer, deck, cardIndex);
    }
    std::cout << "Dealer busts with score of " << score << '\n';
    return true;
}

BlackJackResult playBlackJack(deck_type& deck)
{
    int cardIndex{ 0 };
    Player player{ getCardValue(deck[cardIndex]) + getCardValue(deck[cardIndex + 1]) };
    cardIndex += 2;
    Player dealer{ getCardValue(deck[cardIndex++]) };
    
    player.bust = playerTurn(player, deck, cardIndex);
    dealer.bust = dealerTurn(dealer, deck, cardIndex);
    if (player.bust && dealer.bust || player.score == dealer.score)
        return BlackJackResult::tie;
    else if (player.bust && !dealer.bust)
        return BlackJackResult::loss;
    else if (!player.bust && dealer.bust)
        return BlackJackResult::win;
    else
        return (static_cast<BlackJackResult>(player.score > dealer.score));
}

int main()
{
    deck_type deck = createDeck();
    shuffleDeck(deck);
    BlackJackResult result{ playBlackJack(deck) };
    if (result == BlackJackResult::win)
        std::cout << "Win!\n";
    else if (result == BlackJackResult::loss)
        std::cout << "Loss!\n";
    else if (result == BlackJackResult::tie)
        std::cout << "Tie!\n";


}