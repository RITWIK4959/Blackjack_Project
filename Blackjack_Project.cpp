#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <string>
#include <stdexcept> // For exception handling

using namespace std;

// Structure to represent a card
struct Card {
    string value;
    string suit;
};

// Class to represent the deck of cards
class Deck {
private:
    Card cards[52];
    int deckIndex;

public:
    Deck() : deckIndex(0) {
        string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
        string values[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};

        int index = 0;
        for (int s = 0; s < 4; ++s) {
            for (int v = 0; v < 13; ++v) {
                cards[index++] = {values[v], suits[s]};
            }
        }
        random_shuffle(cards, cards + 52);
    }

    Card dealCard() {
        if (deckIndex >= 52) {
            throw runtime_error("Deck is out of cards!");
        }
        return cards[deckIndex++];
    }
};

// Base class for players
class PlayerBase {
protected:
    string name;
    Card hand[10];
    int handSize;
    int score;

    int calculateScore() {
        score = 0;
        int aces = 0;

        for (int i = 0; i < handSize; ++i) {
            if (hand[i].value == "Jack" || hand[i].value == "Queen" || hand[i].value == "King") {
                score += 10;
            } else if (hand[i].value == "Ace") {
                score += 11;
                aces++;
            } else {
                score += stoi(hand[i].value);
            }
        }

        // Adjust for Aces if score exceeds 21
        while (score > 21 && aces > 0) {
            score -= 10;
            aces--;
        }

        return score;
    }

    void displayHand() {
        cout << name << "'s Hand: ";
        for (int i = 0; i < handSize; ++i) {
            cout << hand[i].value << " of " << hand[i].suit << ", ";
        }
        cout << endl;
    }

public:
    PlayerBase(string playerName) : name(playerName), handSize(0), score(0) {}

    virtual void playTurn(Deck &deck) = 0; // Pure virtual function

    int getScore() const {
        return score;
    }

    string getName() const {
        return name;
    }
};

// Derived class for human players
class HumanPlayer : public PlayerBase {
public:
    HumanPlayer(string playerName) : PlayerBase(playerName) {}

    void playTurn(Deck &deck) override {
        try {
            hand[handSize++] = deck.dealCard();
            hand[handSize++] = deck.dealCard();

            while (true) {
                displayHand();
                calculateScore();
                cout << name << "'s Score: " << score << endl;

                if (score > 21) {
                    cout << name << " busts!" << endl;
                    return;
                }

                string choice;
                while (true) { // Start a loop to validate input
                    cout << name << ", do you want to 'hit' or 'stand'? ";
                    cin >> choice;

                    if (choice == "stand") {
                        return;
                    } else if (choice == "hit") {
                        hand[handSize++] = deck.dealCard();
                        break; // Valid input, exit the loop and continue
                    } else {
                        cout << "Invalid choice. Please enter 'hit' or 'stand'." << endl;
                    }
                }
            }
        } catch (const runtime_error &e) {
            cout << "Error: " << e.what() << endl;
            exit(1);
        }
    }
};

// Derived class for computer players
class ComputerPlayer : public PlayerBase {
public:
    ComputerPlayer() : PlayerBase("Computer") {}

    void playTurn(Deck &deck) override {
        try {
            hand[handSize++] = deck.dealCard();
            hand[handSize++] = deck.dealCard();

            while (calculateScore() < 17) {
                hand[handSize++] = deck.dealCard();
            }

            displayHand();
            cout << name << "'s Score: " << score << endl;

            if (score > 21) {
                cout << name << " busts!" << endl;
            }
        } catch (const runtime_error &e) {
            cout << "Error: " << e.what() << endl;
            exit(1);
        }
    }
};

// Function to play the game
void playBlackjack() {
    cout << "Welcome to Blackjack!" << endl;

    // Create and shuffle deck
    Deck deck;

    cout << "Choose game mode: 'PvP' (Player vs Player) or 'PvC' (Player vs Computer): ";
    string mode;
    cin >> mode;

    try {
        if (mode == "PvP") {
            int numPlayers;
            cout << "Enter the number of players (2 or more): ";
            cin >> numPlayers;

            if (numPlayers < 2) {
                throw invalid_argument("Number of players must be at least 2.");
            }

            PlayerBase *players[numPlayers];
            for (int i = 0; i < numPlayers; ++i) {
                players[i] = new HumanPlayer("Player " + to_string(i + 1));
            }

            for (int i = 0; i < numPlayers; ++i) {
                cout << "\n" << players[i]->getName() << "'s turn:" << endl;
                players[i]->playTurn(deck);
            }

            // Determine the winner
            cout << "\nFinal Scores:" << endl;
            int maxScore = 0, winner = -1;
            for (int i = 0; i < numPlayers; ++i) {
                cout << players[i]->getName() << ": " << players[i]->getScore() << endl;
                if (players[i]->getScore() > maxScore && players[i]->getScore() <= 21) {
                    maxScore = players[i]->getScore();
                    winner = i;
                }
            }

            if (winner == -1) {
                cout << "No one wins! All players busted!" << endl;
            } else {
                cout << players[winner]->getName() << " wins with a score of " << maxScore << "!" << endl;
            }

            for (int i = 0; i < numPlayers; ++i) {
                delete players[i];
            }

        } else if (mode == "PvC") {
            HumanPlayer player("Player");
            ComputerPlayer computer;

            cout << "\nPlayer's turn:" << endl;
            player.playTurn(deck);

            cout << "\nComputer's turn:" << endl;
            computer.playTurn(deck);

            cout << "\nFinal Scores:" << endl;
            cout << player.getName() << ": " << player.getScore() << endl;
            cout << computer.getName() << ": " << computer.getScore() << endl;

            if (player.getScore() > computer.getScore() && player.getScore() <= 21) {
                cout << "Player wins!" << endl;
            } else if (computer.getScore() > player.getScore() && computer.getScore() <= 21) {
                cout << "Computer wins!" << endl;
            } else if (player.getScore() == computer.getScore()) {
                cout << "It's a tie!" << endl;
            } else {
                cout << "No one wins!" << endl;
            }
        } else {
            throw invalid_argument("Invalid mode selected. Please choose 'PvP' or 'PvC'.");
        }
    } catch (const invalid_argument &e) {
        cout << "Error: " << e.what() << endl;
        exit(1);
    }
}

// Function to ask if the player wants to play again
bool askToPlayAgain() {
    string response;
    cout << "Do you want to play again? (yes/no): ";
    cin >> response;
    return (response == "yes" || response == "Yes");
}

int main() {
    srand(time(0)); // Seed random number generator
    try {
        do {
            playBlackjack();
        } while (askToPlayAgain());  // Keep playing until user chooses to exit
    } catch (const exception &e) {
        cout << "Unexpected error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
