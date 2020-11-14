#include <iostream>
#include <array>
#include <random>
#include <ctime>
#include <string>
#include <chrono>
#include <thread>

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

enum BlackJackResult
{
    win,
    loss,
    tie
};
class Card
{
private:
    CardRank m_rank;
    CardSuit m_suit;
public:
    Card(CardRank rank, CardSuit suit) : m_rank{ rank }, m_suit{ suit }
    {};
    Card() = default;
    int value() const
    {
        if (m_rank < 10)
            return (static_cast<int>(m_rank) + 2);
        switch (m_rank)
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
    friend std::ostream& operator<<(std::ostream& out, const Card& card);
};

class Deck
{
private:
    std::array<Card, 52> m_deck;
    int m_cardIndex{ 0 };
public:
    Deck()
    {
        int index{ 0 };
        for (int suit{ 0 }; suit < MAX_SUITS; ++suit)
        {
            for (int rank{ 0 }; rank < MAX_RANKS; ++rank)
            {
                m_deck[index] = { static_cast<CardRank>(rank), static_cast<CardSuit>(suit) };
                ++index;
            }
        }
    }
    Deck& shuffle()
    {
        static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
        std::shuffle(m_deck.begin(), m_deck.end(), mt);
        m_cardIndex = 0;
        return *this;
    }
    Deck& print()
    {
        for (auto& card : m_deck)
        {
            std::cout << card << '\n';
        }
        return *this;
    }
    const Card& dealCard() { return m_deck[m_cardIndex++]; }
    friend std::ostream& operator<<(std::ostream& out, const Deck& deck);
};

class Player
{
private:
    int m_score{ 0 };
    int m_aces{ 0 };

    void drawCard(Deck& deck)
    {
        int card_val{ deck.dealCard().value() };
        if (card_val == 11)
            ++m_aces;
        m_score += card_val;
    }
public:
    int score() const
    {
        return m_score;
    }
    bool isBust()
    {
        return m_score > maximumScore ? true : false;
    }
    bool playerTurn(Deck& deck)
    {
        using namespace std;
        cout << "You have a score of " << m_score << '\n';
        while (!isBust())
        {
            string choice;
            while (true)
            {
                cout << "Would you like to (h)it or (s)tand? -> ";
                getline(cin, choice);
                if (choice != "h" && choice != "s")
                {
                    continue;
                }
                break;
            }
            while (!isBust())
            {
                if (choice == "h")
                {
                    drawCard(deck);
                    cout << "\nYou have a score of " << m_score << '\n';
                    break;
                }
                else if (choice == "s")
                {
                    cout << "\nYou stand with a score of " << m_score << '\n';
                    return false;
                }
            }
        }
        if (m_aces > 0)
        {
            cout << "You have an ace.\n";
            m_aces -= 1;
            m_score -= 10;
            return playerTurn(deck);
        }
        cout << "Bust!\n";
        return true;
    }
    bool dealerTurn(Deck& deck)
    {
        std::cout << "Dealer score is " << m_score << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        while (m_score < minimumDealerScore)
        {
            std::cout << "Dealer hits\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            drawCard(deck);
            std::cout << "Dealer score is " << m_score << '\n';
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        std::cout << "Dealer's final score is " << m_score << '\n';
        return isBust();
    }
    friend void playBlackJack(Deck& deck);
};

std::ostream& operator<<(std::ostream& out, const Card& card)
{
    switch (card.m_rank)
    {
    case CardRank::RANK_2:
        out << '2';
        break;
    case CardRank::RANK_3:
        out << '3';
        break;
    case CardRank::RANK_4:
        out << '4';
        break;
    case CardRank::RANK_5:
        out << '5';
        break;
    case CardRank::RANK_6:
        out << '6';
        break;
    case CardRank::RANK_7:
        out << '7';
        break;
    case CardRank::RANK_8:
        out << '8';
        break;
    case CardRank::RANK_9:
        out << '9';
        break;
    case CardRank::RANK_10:
        out << '10';
        break;
    case CardRank::RANK_JACK:
        out << 'J';
        break;
    case CardRank::RANK_QUEEN:
        out << 'Q';
        break;
    case CardRank::RANK_KING:
        out << 'K';
        break;
    case CardRank::RANK_ACE:
        out << 'A';
        break;
    default:
        out << '?';
        break;
    }

    switch (card.m_suit)
    {
    case CardSuit::SUIT_CLUB:
        out << 'C';
        break;
    case CardSuit::SUIT_DIAMOND:
        out << 'D';
        break;
    case CardSuit::SUIT_HEART:
        out << 'H';
        break;
    case CardSuit::SUIT_SPADE:
        out << 'S';
        break;
    default:
        out << '?';
        break;
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const Deck& deck)
{
    for (auto& card : deck.m_deck)
        out << card << '\n';
    return out;
}

void playBlackJack(Deck& deck)
{
    int cardIndex{ 0 };
    Player player{};
    player.drawCard(deck);
    player.drawCard(deck);
    Player dealer{};
    dealer.drawCard(deck);

    bool player_bust{ player.playerTurn(deck) };
    bool dealer_bust{ dealer.dealerTurn(deck) };
    if (player_bust && dealer_bust || player.m_score == dealer.m_score)
        std::cout << "Tie!\n";
    else if (player_bust && !dealer_bust)
        std::cout << "Loss!\n";
    else if (!player_bust && dealer_bust)
        std::cout << "Win!\n";
    else
        player.score() > dealer.score() ? std::cout << "Win!\n" : std::cout << "Loss!\n";
}

int main()
{
    Deck deck{};
    deck.shuffle();
    playBlackJack(deck);
}