#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    char name[20];
    int skill;
    int energy;
    double value;
} Player;

typedef struct {
    char name[30];
    int points;
    int goalsFor;
    int goalsAgainst;
    int gamesPlayed;
} Team;

// Global variables
int managerLevel = 1;
double clubMoney = 500.0;
Team league[10];
char myTeamName[30];
int currentMatchday = 0;
int season = 1;

// Array of predefined player names for random selection
char playerNames[][20] = {
    "Messi", "Ronaldo", "Mbappe", "Neymar", "Haaland", "Benzema", "Salah", "Kane",
    "Lewandowski", "De Bruyne", "Modric", "Vinicius", "Son", "Griezmann", "Foden", "Pedri",
    "Bellingham", "Saka", "Gavi", "Sancho", "Sterling", "Mane", "Vlahovic", "Odegaard",
    "Kimmich", "Alisson", "Courtois", "Ramos", "Hakimi", "Varane", "Silva", "Casemiro"
};

// Function Declarations
void listPlayers(Player *players, int *numPlayers);
void printStats();
void sellPlayer(Player *players, int *numPlayers, int playerIndex);
void matchDay(Player *players, int *numPlayers);
void continueMatch(Player *players, int *numPlayers);
void buyRandomPlayer(Player *players, int *numPlayers, double *clubMoney);
int playerNameExists(Player *players, int numPlayers, const char *name);
void initLeague(void);
void playLeagueMatch(Player *players, int *numPlayers);
void showLeagueTable(void);
void endSeason(Player *players, int *numPlayers);
void simulateOtherMatches(int currentMatchday);

int main() {
    printf("Enter your team name: ");
    if (fgets(myTeamName, sizeof(myTeamName), stdin)) {
        myTeamName[strcspn(myTeamName, "\n")] = 0; // Remove newline
    }

    
    // Initialize player data
    int numPlayers = 8;
    Player *players = (Player *)malloc(numPlayers * sizeof(Player));
    for (int i = 0; i < numPlayers; ++i) {
        int nameIndex, tries = 0;
        do {
            nameIndex = rand() % (sizeof(playerNames) / sizeof(playerNames[0]));
            strncpy(players[i].name, playerNames[nameIndex], sizeof(players[i].name) - 1);
            players[i].name[sizeof(players[i].name) - 1] = '\0';
            tries++;
        } while (playerNameExists(players, i, players[i].name) && tries < 100);
        players[i].skill = rand() % 5 + 1;
        players[i].energy = rand() % 10 + 1;
        players[i].value = players[i].skill * 100.0 + players[i].energy * 10.0;
    }
    
    // Initialize league
    initLeague();
    
    // Seed the random number generator
    srand(time(NULL));

    int choice;
    char input[100];
    do {
        static int lastPosition = 0;
        static int lastReward = 0;
        static int lastManagerLevel = 1;
        if (currentMatchday < 9) {
            printf("\nMatchday #%d\n\n", currentMatchday + 1);
            printf("Manager Level: %d\n", managerLevel);
            printf("Club Money: %.2f€\n\n", clubMoney);
        } else {
            printf("\nSeason over.\n");
            showLeagueTable();
            printf("\n");

            // Find my position and reward for display
            int myPosition = 0;
            Team sortedLeague[10];
            memcpy(sortedLeague, league, sizeof(league));
            for (int i = 0; i < 10; i++) {
                for (int j = i + 1; j < 10; j++) {
                    if (sortedLeague[j].points > sortedLeague[i].points) {
                        Team temp = sortedLeague[i];
                        sortedLeague[i] = sortedLeague[j];
                        sortedLeague[j] = temp;
                    }
                }
            }
            for (int i = 0; i < 10; i++) {
                if (strcmp(sortedLeague[i].name, myTeamName) == 0) {
                    myPosition = i + 1;
                    break;
                }
            }
            int seasonReward = 500 - (myPosition - 1) * 10;
            if (seasonReward < 0) seasonReward = 0;
            if (lastPosition != myPosition || lastReward != seasonReward || lastManagerLevel != managerLevel) {
                printf("You finished in position %d.\n", myPosition);
                printf("You received %d€ for your final position.\n", seasonReward);
                printf("Your manager level is now %d.\n\n", managerLevel);
                lastPosition = myPosition;
                lastReward = seasonReward;
                lastManagerLevel = managerLevel;
            }
            printf("Manager Level: %d\n", managerLevel);
            printf("Club Money: %.2f€\n\n", clubMoney);
        }

        // Main menu options
        if (currentMatchday < 9) {
            printf("1. Play Match\n");
            printf("2. Sell Players\n");
            printf("3. Show Table\n");
            printf("0. Quit\n\n");
        } else {
            printf("2. Sell Players\n");
            printf("3. Show Table\n");
            printf("0. Quit\n\n");
        }
        printf("Enter your choice: ");

        if (fgets(input, sizeof(input), stdin)) {
            choice = atoi(input);
        } else {
            choice = -1;
        }

        // Menu logic
        if (currentMatchday < 9) {
            switch (choice) {
                case 1:
                    playLeagueMatch(players, &numPlayers);
                    break;
                case 2:
                    listPlayers(players, &numPlayers);
                    break;
                case 3:
                    showLeagueTable();
                    break;
                case 0:
                    printf("Exiting the game. Goodbye!\n");
                    break;
                default:
                    if (choice >= 0) {
                        printf("Invalid choice. Please try again.\n");
                    }
            }
        } else {
            switch (choice) {
                case 2:
                    listPlayers(players, &numPlayers);
                    break;
                case 3:
                    showLeagueTable();
                    break;
                case 0:
                    printf("Exiting the game. Goodbye!\n");
                    break;
                default:
                    if (choice >= 0) {
                        printf("Invalid choice. Please try again.\n");
                    }
            }
        }
    } while (choice != 0);

    // Free allocated memory
    free(players);
    return 0;
}

void listPlayers(Player *players, int *numPlayers) {
    printf("\n%-5s%-15s%-15s%-15s%-15s\n", "No.", "Name", "Skill", "Energy", "Value");
    printf("--------------------------------------------\n");
    int totalSkill = 0;
    int totalEnergy = 0;
    for (int i = 0; i < *numPlayers; ++i) {
        printf("%-5d%-15s%-15d%-15d%.0f€\n", i + 1, players[i].name, players[i].skill, players[i].energy, players[i].value);
        totalSkill += players[i].skill;
        totalEnergy += players[i].energy;
    }

    if (*numPlayers > 0) {
        double avgSkill = (double)totalSkill / *numPlayers;
        double avgEnergy = (double)totalEnergy / *numPlayers;
        printf("\nAverage Skill Level: %.2f\n", avgSkill);
        printf("Average Energy Level: %.2f\n", avgEnergy);
    } else {
        printf("\nNo players available.\n");
    }

    if (*numPlayers > 5) {
        printf("\nSell a player (Press 1-%d, 0 to go back): ", *numPlayers);
        int sellChoice;
        scanf("%d", &sellChoice);
        if (sellChoice >= 1 && sellChoice <= *numPlayers) {
            sellPlayer(players, numPlayers, sellChoice - 1);
        } else if (sellChoice != 0) {
            printf("Invalid choice. Returning to the main menu.\n");
        }
        // Clear input buffer
        while (getchar() != '\n');
    } else {
        printf("\nYou must have at least 5 players. Selling is disabled.\n");
        // Clear input buffer
        while (getchar() != '\n');
    }
}

void printStats() {
    printf("\nManager Level: %d\n", managerLevel);
    printf("Club Money: %.2f€\n", clubMoney);
}

void sellPlayer(Player *players, int *numPlayers, int playerIndex) {
    if (*numPlayers <= 5) {
        printf("You must have at least 5 players on your team. Cannot sell more players.\n");
        return;
    }
    if (playerIndex >= 0 && playerIndex < *numPlayers) {
        // Add player's value to clubMoney
        clubMoney += players[playerIndex].value;
        // Remove the player from the list
        for (int i = playerIndex; i < *numPlayers - 1; ++i) {
            players[i] = players[i + 1];
        }
        (*numPlayers)--;
        printf("Player has been sold. Club Money updated to %.2f€.\n", clubMoney);
    } else {
        printf("Invalid player index.\n");
    }
}

void matchDay(Player *players, int *numPlayers) {
    // Generate a random opponent
    Player opponent;
    sprintf(opponent.name, "Opponent");
    opponent.skill = rand() % 5 + 1; // Random skill level from 1 to 5
    opponent.energy = rand() % 10 + 1; // Random energy level from 1 to 10

    printf("\nOpponent's Statistics:\n");
    printf("Average Skill Level: %d\n", opponent.skill);
    printf("Average Energy Level: %d\n", opponent.energy);

    // User selects their team
    printf("\nYour Team:\n");
    for (int i = 0; i < *numPlayers; ++i) {
        printf("%d. %-15sSkill: %-15dEnergy: %-15d\n", i + 1, players[i].name, players[i].skill, players[i].energy);
    }

    int selectedPlayers[5];
    printf("\nSelect 5 players by typing their numbers (1-%d): ", *numPlayers);
    for (int i = 0; i < 5; ++i) {
        scanf("%d", &selectedPlayers[i]);
        // Validate input
        if (selectedPlayers[i] < 1 || selectedPlayers[i] > *numPlayers) {
            printf("Invalid player number! Please select again.\n");
            i--;
            continue;
        }
    }

    // Adjust energy levels after the match
    for (int i = 0; i < *numPlayers; ++i) {
        int isSelected = 0;
        for (int j = 0; j < 5; ++j) {
            if (i + 1 == selectedPlayers[j]) {
                isSelected = 1;
                break;
            }
        }

        if (isSelected) {
            players[i].energy = (players[i].energy > 1) ? players[i].energy - 1 : 1;
        } else {
            players[i].energy = (players[i].energy < 10) ? players[i].energy + 2 : 10;
        }

        // Update value after energy change
        players[i].value = players[i].skill * 100.0 + players[i].energy * 10.0;
    }

    // Calculate average skill and energy levels of selected players
    double avgUserSkill = 0.0;
    double avgUserEnergy = 0.0;
    for (int i = 0; i < 5; ++i) {
        int index = selectedPlayers[i] - 1;
        avgUserSkill += players[index].skill;
        avgUserEnergy += players[index].energy;
    }
    avgUserSkill /= 5;
    avgUserEnergy /= 5;

    printf("\nYour Team Statistics:\n");
    printf("Average Skill Level: %.2f\n", avgUserSkill);
    printf("Average Energy Level: %.2f\n", avgUserEnergy);

    // Determine the outcome of the match
    if (avgUserSkill >= opponent.skill || avgUserEnergy >= opponent.energy) {
        printf("\nCongratulations! You win the match!\n");
        clubMoney += 100.0;  // Award 100€ for winning
    } else {
        printf("\nSorry, you lose the match. Better luck next time!\n");
        clubMoney += 20.0;  // Award 20€ for losing
    }

    // Print updated stats
    printStats();

    // Continue with buying a new player
    continueMatch(players, numPlayers);
}

void continueMatch(Player *players, int *numPlayers) {
    printf("\nPress Enter to continue...");
    getchar(); // Wait for Enter key

    // Buy a new player
    buyRandomPlayer(players, numPlayers, &clubMoney);
}

void buyRandomPlayer(Player *players, int *numPlayers, double *clubMoney) {
    // Generate a random player available for purchase
    Player randomPlayer;
    int nameIndex, tries = 0;
    do {
        nameIndex = rand() % (sizeof(playerNames) / sizeof(playerNames[0]));
        strncpy(randomPlayer.name, playerNames[nameIndex], sizeof(randomPlayer.name) - 1);
        randomPlayer.name[sizeof(randomPlayer.name) - 1] = '\0';
        tries++;
    } while (playerNameExists(players, *numPlayers, randomPlayer.name) && tries < 100);
    randomPlayer.skill = rand() % 5 + 1; // Random skill level from 1 to 5
    randomPlayer.energy = rand() % 10 + 1; // Random energy level from 1 to 10
    randomPlayer.value = randomPlayer.skill * 100.0 + randomPlayer.energy * 10.0; // Value based on skill and energy

    printf("\nRandom Player Available for Purchase (Club total: %.2f€):\n", *clubMoney);
    printf("Name: %-15sSkill: %-15dEnergy: %-15dValue (€): %.2f\n",
           randomPlayer.name, randomPlayer.skill, randomPlayer.energy, randomPlayer.value);

    printf("\nPress 1 to buy the player, or press Enter to continue: ");
    char input[10];
    int buyChoice = 0;
    
    if (fgets(input, sizeof(input), stdin)) {
        buyChoice = atoi(input);
    }

    if (buyChoice == 1) {
        if (*clubMoney >= randomPlayer.value) {
            *clubMoney -= randomPlayer.value;
            printf("\nCongratulations! You have successfully purchased %s!\n", randomPlayer.name);

            // Add the player to the team
            if (*numPlayers < 100) { // Prevent overflow
                players[*numPlayers] = randomPlayer;
                (*numPlayers)++;
            } else {
                printf("Team is full! Cannot buy more players.\n");
            }

            printStats(); // Show updated stats
        } else {
            printf("\nSorry, you do not have enough money to purchase %s.\n", randomPlayer.name);
        }
    } else {
        printf("Returning to the main menu.\n");
    }
}

// Initialize the league with default values
void initLeague(void) {
    // Initialize opponent teams
    char *teamNames[] = {
        "Red Dragons", "Blue Eagles", "Green Lions", "Yellow Tigers",
        "Black Panthers", "White Knights", "Purple Ravens", "Orange Bears",
        "Silver Wolves"
    };
    
    // Set my team as first team
    strncpy(league[0].name, myTeamName, sizeof(league[0].name) - 1);
    league[0].name[sizeof(league[0].name) - 1] = '\0';
    
    // Initialize other teams
    for (int i = 1; i < 10; i++) {
        strncpy(league[i].name, teamNames[i-1], sizeof(league[i].name) - 1);
        league[i].name[sizeof(league[i].name) - 1] = '\0';
        league[i].points = 0;
        league[i].goalsFor = 0;
        league[i].goalsAgainst = 0;
        league[i].gamesPlayed = 0;
    }
}

void playLeagueMatch(Player *players, int *numPlayers) {
    if (currentMatchday >= 9) {
        printf("\nSeason is over! Check the final standings.\n");
        endSeason(players, numPlayers);
        return;
    }

    int opponent = currentMatchday + 1;
    printf("\nMatchday %d: %s vs %s\n\n", currentMatchday + 1, myTeamName, league[opponent].name);

    // Opponent stats: skill is integer, energy is float
    double minSkill = 2.0 + (managerLevel - 1) * 0.5;
    double maxSkill = 4.0 + (managerLevel - 1) * 0.5;
    if (maxSkill > 9.0) maxSkill = 9.0;
    if (minSkill > maxSkill) minSkill = maxSkill - 0.5;
    int opponentSkill = (int)(minSkill + (rand() % ((int)(maxSkill - minSkill + 1))));
    if (opponentSkill < (int)minSkill) opponentSkill = (int)minSkill;
    if (opponentSkill > (int)maxSkill) opponentSkill = (int)maxSkill;

    double minEnergy = 3.0 + (managerLevel - 1) * 0.5;
    double maxEnergy = 9.0 + (managerLevel - 1) * 0.5;
    if (maxEnergy > 10.0) maxEnergy = 10.0;
    if (minEnergy > maxEnergy) minEnergy = maxEnergy - 0.5;
    double opponentEnergy = minEnergy + ((double)rand() / RAND_MAX) * (maxEnergy - minEnergy);

    // Show opponent stats before player selection
    printf("%s's Statistics:\n", league[opponent].name);
    printf("Average Skill Level: %.2f\n", (double)opponentSkill);
    printf("Average Energy Level: %.2f\n\n", opponentEnergy);

    int selectedPlayers[5];
    char input[10];
    int validSelection = 0;
    while (!validSelection) {
        // User selects their team
        printf("Your Team:\n");
        for (int i = 0; i < *numPlayers; ++i) {
            printf("%d. %-15sSkill: %-15dEnergy: %-15d\n", i + 1, players[i].name, players[i].skill, players[i].energy);
        }

        int duplicate = 0;
        printf("\nSelect 5 players by typing their numbers (1-%d): ", *numPlayers);
        for (int i = 0; i < 5; ++i) {
            scanf("%d", &selectedPlayers[i]);
            // Validate input
            if (selectedPlayers[i] < 1 || selectedPlayers[i] > *numPlayers) {
                printf("Invalid player number! Please select again.\n");
                i = -1; // restart selection
                duplicate = 1;
                break;
            }
            // Check for duplicates
            for (int j = 0; j < i; ++j) {
                if (selectedPlayers[i] == selectedPlayers[j]) {
                    printf("You cannot select the same player more than once! Please choose again.\n");
                    i = -1; // restart selection
                    duplicate = 1;
                    break;
                }
            }
            if (duplicate) break;
        }
        while (getchar() != '\n'); // Clear input buffer
        if (duplicate) continue;

        // Calculate and show your selected team's average stats
        double selAvgSkill = 0.0;
        double selAvgEnergy = 0.0;
        for (int i = 0; i < 5; i++) {
            int playerIndex = selectedPlayers[i] - 1;
            selAvgSkill += players[playerIndex].skill;
            selAvgEnergy += players[playerIndex].energy;
        }
        selAvgSkill /= 5;
        selAvgEnergy /= 5;

        printf("\n%s's Statistics:\n", myTeamName);
        printf("Average Skill Level: %.2f\n", selAvgSkill);
        printf("Average Energy Level: %.2f\n\n", selAvgEnergy);
        printf("%s's Statistics:\n", league[opponent].name);
        printf("Average Skill Level: %.2f\n", (double)opponentSkill);
        printf("Average Energy Level: %.2f\n", opponentEnergy);

        printf("\nPress Enter to continue with this team, or type 'c' to change selection: ");
        if (fgets(input, sizeof(input), stdin)) {
            if (input[0] == '\n') {
                validSelection = 1;
            }
        }
    }

    // Calculate match result based on selected players' strength
    double myTeamStrength = 0;
    for (int i = 0; i < 5; i++) {
        int playerIndex = selectedPlayers[i] - 1;
        myTeamStrength += players[playerIndex].skill * players[playerIndex].energy;
    }
    myTeamStrength /= 5;

    // Adjust energy levels after the match
    for (int i = 0; i < *numPlayers; ++i) {
        int isSelected = 0;
        for (int j = 0; j < 5; ++j) {
            if (i == selectedPlayers[j] - 1) {
                isSelected = 1;
                break;
            }
        }

        if (isSelected) {
            players[i].energy = (players[i].energy > 1) ? players[i].energy - 1 : 1;
        } else {
            players[i].energy = (players[i].energy < 10) ? players[i].energy + 2 : 10;
        }
        // Update value after energy change
        players[i].value = players[i].skill * 100.0 + players[i].energy * 10.0;
    }

    // Calculate goals based on team strengths
    int myGoals = (myTeamStrength / 10) + (rand() % 3);
    int oppGoals = (opponentSkill * opponentEnergy / 10) + (rand() % 3);

    printf("\nResult: %s %d - %d %s\n", myTeamName, myGoals, oppGoals, league[opponent].name);

    // Awarded money variable
    int awardedMoney = 0;

    // Update league table
    league[0].goalsFor += myGoals;
    league[0].goalsAgainst += oppGoals;
    league[opponent].goalsFor += oppGoals;
    league[opponent].goalsAgainst += myGoals;

    if (myGoals > oppGoals) {
        league[0].points += 3;
        printf("You won! +3 points\n");
        awardedMoney = 100;
        clubMoney += 100.0;
    } else if (myGoals == oppGoals) {
        league[0].points += 1;
        league[opponent].points += 1;
        printf("Draw! +1 point\n");
        awardedMoney = 50;
        clubMoney += 50.0;
    } else {
        league[opponent].points += 3;
        printf("You lost!\n");
        awardedMoney = 0;
        // No money for losing
    }

    printf("You received %d€ for this match.\n", awardedMoney);

    league[0].gamesPlayed++;
    league[opponent].gamesPlayed++;

    currentMatchday++;

    // Simulate other matches for this matchday
    simulateOtherMatches(currentMatchday);

    // Show current standings
    showLeagueTable();

    // Continue with player management
    continueMatch(players, numPlayers);
}

void showLeagueTable(void) {
    printf("\nLeague Table (Season %d):\n", season);
    printf("%-20s %-8s %-5s %-8s %-8s %-5s\n", "Team", "Matches", "Pts", "Scored", "Against", "Diff");
    printf("------------------------------------------------\n");
    
    // Sort teams by points, then by goal difference
    Team sortedLeague[10];
    memcpy(sortedLeague, league, sizeof(league));
    
    for (int i = 0; i < 10; i++) {
        for (int j = i + 1; j < 10; j++) {
            int diff_i = sortedLeague[i].goalsFor - sortedLeague[i].goalsAgainst;
            int diff_j = sortedLeague[j].goalsFor - sortedLeague[j].goalsAgainst;
            if (sortedLeague[j].points > sortedLeague[i].points ||
                (sortedLeague[j].points == sortedLeague[i].points && diff_j > diff_i)) {
                Team temp = sortedLeague[i];
                sortedLeague[i] = sortedLeague[j];
                sortedLeague[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < 10; i++) {
        printf("%-20s %-8d %-5d %-8d %-8d %-5d\n", 
            sortedLeague[i].name,
            sortedLeague[i].gamesPlayed,
            sortedLeague[i].points,
            sortedLeague[i].goalsFor,
            sortedLeague[i].goalsAgainst,
            sortedLeague[i].goalsFor - sortedLeague[i].goalsAgainst);
    }
}

void endSeason(Player *players, int *numPlayers) {
    // Find my position
    int myPosition = 0;
    Team sortedLeague[10];
    memcpy(sortedLeague, league, sizeof(league));
    
    for (int i = 0; i < 10; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (sortedLeague[j].points > sortedLeague[i].points) {
                Team temp = sortedLeague[i];
                sortedLeague[i] = sortedLeague[j];
                sortedLeague[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < 10; i++) {
        if (strcmp(sortedLeague[i].name, myTeamName) == 0) {
            myPosition = i + 1;
            break;
        }
    }
    
    printf("\nSeason %d finished!\n", season);
    printf("Your team finished in position %d\n", myPosition);
    
    // Award money based on position
    int seasonReward = 500 - (myPosition - 1) * 10;
    if (seasonReward < 0) seasonReward = 0;
    clubMoney += seasonReward;
    printf("You received %d€ for your final position!\n", seasonReward);

    // Decrease all players' skill by 1 (min 1), increase energy by 5 (max 10)
    for (int i = 0; i < *numPlayers; i++) {
        if (players[i].skill > 1) {
            players[i].skill--;
        }
        players[i].energy += 5;
        if (players[i].energy > 10) players[i].energy = 10;
        // Update player value
        players[i].value = players[i].skill * 100.0 + players[i].energy * 10.0;
    }
    printf("All players' skills decreased by 1 and energy increased by 5 (max 10).\n");

    if (myPosition == 1) {
        printf("Congratulations! You are the champion!\n");
        managerLevel++;
        printf("Manager level increased to %d!\n", managerLevel);
    } else if (myPosition > 3) {
        printf("You didn't finish in the top 3.\n");
    }

    // Print final table
    printf("\nFinal League Table:\n");
    showLeagueTable();

    // Reset for next season
    season++;
    currentMatchday = 0;
    for (int i = 0; i < 10; i++) {
        league[i].points = 0;
        league[i].goalsFor = 0;
        league[i].goalsAgainst = 0;
        league[i].gamesPlayed = 0;
    }
}

// Helper function to check if a player name already exists in the team
int playerNameExists(Player *players, int numPlayers, const char *name) {
    for (int i = 0; i < numPlayers; i++) {
        if (strcmp(players[i].name, name) == 0) {
            return 1;  // Name exists
        }
    }
    return 0;  // Name does not exist
}

void simulateOtherMatches(int currentMatchday) {
    // Array to track which teams have played this round
    int hasPlayed[10] = {0};  // 0 means hasn't played
    
    // Mark teams that just played (user's team and their opponent)
    hasPlayed[0] = 1;  // user's team
    hasPlayed[currentMatchday] = 1;  // user's opponent
    
    // Simulate matches between remaining teams
    for (int i = 1; i < 10; i++) {
        if (hasPlayed[i]) continue;  // Skip if team already played
        
        // Find an opponent who hasn't played yet
        for (int j = i + 1; j < 10; j++) {
            if (!hasPlayed[j]) {  // Found an opponent who hasn't played
                hasPlayed[i] = 1;
                hasPlayed[j] = 1;
                
                // Generate random scores
                int team1Goals = (rand() % 4); // 0-3 goals
                int team2Goals = (rand() % 4); // 0-3 goals
                
                // Update stats
                league[i].goalsFor += team1Goals;
                league[i].goalsAgainst += team2Goals;
                league[j].goalsFor += team2Goals;
                league[j].goalsAgainst += team1Goals;
                
                // Update points
                if (team1Goals > team2Goals) {
                    league[i].points += 3;
                } else if (team1Goals == team2Goals) {
                    league[i].points += 1;
                    league[j].points += 1;
                } else {
                    league[j].points += 3;
                }
                
                // Update games played
                league[i].gamesPlayed++;
                league[j].gamesPlayed++;
                break;  // Move to next team
            }
        }
    }
}