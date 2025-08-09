#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <math.h>
#endif

// Struct Definitions
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
    int skillRating;
    int fitness;
} Team;

// --- Function Declarations (Prototypes) ---
void listPlayers(Player **players, int *numPlayers);
void sellPlayer(Player **players, int *numPlayers, int playerIndex);
void continueMatch(Player **players, int *numPlayers);
void buyRandomPlayer(Player **players, int *numPlayers);
int playerNameExists(Player *players, int numPlayers, const char *name);
void initLeague(void);
void evolveLeague(void);
void playLeagueMatch(Player **players, int *numPlayers);
void showLeagueTable(void);
void endSeason(Player **players, int *numPlayers);
void simulateOtherMatches(int matchday);
void sortPlayers(Player *players, int numPlayers);
void clearScreen(void);
void cross_platform_sleep(int seconds);


// --- Global Variables ---
int managerLevel = 1;
double clubMoney = 500.0;
Team league[10];
char myTeamName[30];
int currentMatchday = 0;
int season = 1;
int negotiationsLeftThisSeason = 10;

char playerNames[][20] = {
    "Messi", "Ronaldo", "Mbappe", "Neymar", "Benzema", "Salah", "Kane",
    "Lewandowski", "De Bruyne", "Modric", "Vinicius", "Son", "Griezmann", "Foden", "Pedri",
    "Bellingham", "Saka", "Gavi", "Sancho", "Sterling", "Mane", "Vlahovic", "Odegaard",
    "Kimmich", "Alisson", "Courtois", "Ramos", "Hakimi", "Varane", "Silva", "Casemiro"
};


// --- Main Function ---
int main() {
    srand(time(NULL));

    printf("Enter your team name: ");
    if (fgets(myTeamName, sizeof(myTeamName), stdin)) {
        myTeamName[strcspn(myTeamName, "\n")] = 0;
    }

    int numPlayers = 8;
    Player *players = (Player *)malloc(numPlayers * sizeof(Player));
    if (players == NULL) {
        printf("Failed to allocate memory!\n");
        return 1;
    }
    
    int skillDistribution[] = {5, 4, 4, 3, 3, 3, 2, 2};
    
    for (int i = 0; i < numPlayers; ++i) {
        int nameIndex, tries = 0;
        do {
            nameIndex = rand() % (sizeof(playerNames) / sizeof(playerNames[0]));
            strncpy(players[i].name, playerNames[nameIndex], sizeof(players[i].name) - 1);
            players[i].name[sizeof(players[i].name) - 1] = '\0';
            tries++;
        } while (playerNameExists(players, i, players[i].name) && tries < 100);
        
        // Set specific skill based on user's distribution
        players[i].skill = skillDistribution[i];
        players[i].energy = 10; // Start all players with 100% fitness
        players[i].value = players[i].skill * 100.0 + players[i].energy * 10.0;
    }
    
    initLeague();
    
    int choice = -1;
    char input[100];
    int endOfSeasonProcessed = 0;

    while (1) {
        clearScreen(); 

        if (currentMatchday < 9) {
            printf("================ Futsal Manager =================\n");
            printf("Season: %d  |  Matchday: %d/9\n", season, currentMatchday + 1);
            printf("Manager Level: %d | Club Money: %.2f€\n", managerLevel, clubMoney);
            printf("=================================================\n\n");
            printf("1. Play Match\n");
            printf("2. Manage Team\n");
            printf("3. Show League Table\n\n");
            printf("Enter your choice: ");

            if (fgets(input, sizeof(input), stdin)) {
                choice = atoi(input);
            } else {
                break; 
            }

            switch (choice) {
                case 1: playLeagueMatch(&players, &numPlayers); break;
                case 2: listPlayers(&players, &numPlayers); break;
                case 3: clearScreen(); showLeagueTable(); printf("\nPress Enter to continue..."); getchar(); break;
                default: break;
            }
        } else { // Season Over
            if (!endOfSeasonProcessed) {
                 endSeason(&players, &numPlayers);
                 endOfSeasonProcessed = 1;
            }
            
            season++;
            currentMatchday = 0;
            evolveLeague();
            endOfSeasonProcessed = 0;
            printf("\nNew season started! Good luck!\nPress Enter to continue...");
            getchar();
        }
    }

    free(players);
    return 0;
}


// --- Function Definitions ---

void cross_platform_sleep(int seconds) {
    #ifdef _WIN32
        Sleep(seconds * 1000);
    #else
        sleep(seconds);
    #endif
}

void clearScreen() {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else
        system("clear");
    #endif
}

void listPlayers(Player **players, int *numPlayers) {
    while (1) {
        clearScreen();
        sortPlayers(*players, *numPlayers);
        
        printf("\n--- Manage Team ---\n");
        printf("Club Money: %.2f€\n\n", clubMoney);
        printf("%-5s %-20s %-10s %-10s %-10s\n", "No.", "Name", "Skill", "Energy", "Value");
        printf("----------------------------------------------------------\n");
        
        if (*numPlayers == 0) {
            printf("You have no players on your team.\n");
        } else {
            for (int i = 0; i < *numPlayers; ++i) {
                printf("%-5d %-20s %-10d %-10d %.0f€\n", i + 1, (*players)[i].name, (*players)[i].skill, (*players)[i].energy, (*players)[i].value);
            }
        }

        printf("\nEnter a player number to sell, or 0 to return to the main menu: ");
        
        char input[10];
        int choice = 0;
        if (fgets(input, sizeof(input), stdin)) {
            choice = atoi(input);
        }

        if (choice == 0) {
            break;
        } else if (choice > 0 && choice <= *numPlayers) {
            if (*numPlayers > 5) {
                sellPlayer(players, numPlayers, choice - 1);
            } else {
                printf("\nCannot sell! You must have at least 5 players.\n");
                printf("Press Enter to continue...");
                getchar();
            }
        } else {
            printf("\nInvalid choice.\n");
            printf("Press Enter to continue...");
            getchar();
        }
    }
}

void sellPlayer(Player **players_ptr, int *numPlayers, int playerIndex) {
    if (*numPlayers <= 5 || playerIndex < 0 || playerIndex >= *numPlayers) return;

    Player *players = *players_ptr;
    clubMoney += players[playerIndex].value;
    printf("\nSold %s for %.0f€.\n", players[playerIndex].name, players[playerIndex].value);
    cross_platform_sleep(0.5);

    for (int i = playerIndex; i < *numPlayers - 1; ++i) {
        players[i] = players[i + 1];
    }
    (*numPlayers)--;
    
    Player* new_players;
    if (*numPlayers > 0) {
        new_players = realloc(players, (*numPlayers) * sizeof(Player));
        if (new_players == NULL) {
            *players_ptr = players;
            return;
        }
    } else {
        free(players);
        new_players = NULL;
    }
    *players_ptr = new_players;
}

void playLeagueMatch(Player **players, int *numPlayers) {
    if (currentMatchday >= 9) {
        clearScreen();
        printf("\nSeason is over! Start a new season from the main menu.\n\nPress Enter...");
        getchar();
        return;
    }
    if (*numPlayers < 5) {
        clearScreen();
        printf("\nYou need at least 5 players to play a match.\n\nPress Enter...");
        getchar();
        return;
    }

    int opponent_idx = currentMatchday + 1;
    sortPlayers(*players, *numPlayers);

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
    int myRank = 0, opponentRank = 0;
    for (int i = 0; i < 10; i++) {
        if (strcmp(sortedLeague[i].name, myTeamName) == 0) myRank = i + 1;
        if (strcmp(sortedLeague[i].name, league[opponent_idx].name) == 0) opponentRank = i + 1;
    }
    
    int isSelected[100] = {0};
    for (int i = 0; i < 5; i++) { isSelected[i] = 1; }

    double difficultyMultiplier = 1.0;
    if (managerLevel == 1)      difficultyMultiplier = 0.75;
    else if (managerLevel == 2) difficultyMultiplier = 0.90;
    else if (managerLevel == 3) difficultyMultiplier = 1.00;
    else difficultyMultiplier = 1.00 + ((managerLevel - 3) * 0.05);

    while (1) {
        clearScreen();
        
        int myTotalSkill = 0;
        double myTotalEnergy = 0;
        int selectionCount = 0;
        for(int i=0; i < *numPlayers; i++) {
            if(isSelected[i]) {
                myTotalSkill += (*players)[i].skill;
                myTotalEnergy += (*players)[i].energy;
                selectionCount++;
            }
        }
        double myAvgFitnessPercent = 0;
        if(selectionCount > 0) {
            myAvgFitnessPercent = (myTotalEnergy / selectionCount) * 10.0;
        }
        
        double opponent_base_rating = league[opponent_idx].skillRating;
        int min_est_skill = 10, max_est_skill = 22;
        double rating_range = 50.0 - 30.0;
        double skill_range = max_est_skill - min_est_skill;
        double strength_percentage = (opponent_base_rating - 30.0) / rating_range;
        if (strength_percentage < 0.0) strength_percentage = 0.0;
        if (strength_percentage > 1.0) strength_percentage = 1.0;
        int opponentDisplaySkill = min_est_skill + (int)(strength_percentage * skill_range);
        
        char myFitnessStr[10], oppFitnessStr[10];
        sprintf(myFitnessStr, "%.0f%%", myAvgFitnessPercent);
        sprintf(oppFitnessStr, "%d%%", league[opponent_idx].fitness);
        
        printf("====================== MATCH OVERVIEW ======================\n");
        printf(" Matchday %d/9, Season %d\n", currentMatchday + 1, season);
        printf("----------------------------------------------------------\n");
        printf("            %-20s vs %-20s\n", myTeamName, league[opponent_idx].name);
        printf("----------------------------------------------------------\n");
        printf(" Rank:      %-20d    %-20d\n", myRank, opponentRank);
        printf(" Total Skill: %-17d    (Est: %d)\n", myTotalSkill, opponentDisplaySkill);
        printf(" Avg Fitness: %-17s    (Est: %s)\n", myFitnessStr, oppFitnessStr);
        printf("==========================================================\n\n");
        printf("1. Simulate Match\n");
        printf("2. Change Player Selection\n");
        printf("0. Back to Main Menu\n\n");
        printf("Enter your choice: ");

        char overviewChoiceInput[10];
        fgets(overviewChoiceInput, sizeof(overviewChoiceInput), stdin);
        int overviewChoice = atoi(overviewChoiceInput);

        if (overviewChoice == 1) {
            if (selectionCount != 5) {
                printf("\nInvalid selection! You must select exactly 5 players.\nPress Enter...");
                getchar();
                continue;
            }
            break; 
        } else if (overviewChoice == 2) {
            int selectionDone = 0;
            while(!selectionDone) {
                clearScreen();
                printf("--- Select Your Starting 5 (Toggle with numbers) ---\n");
                printf("   %-20s %-7s %-7s %-10s\n", "Player", "Skill", "Energy", "Selected");
                printf("------------------------------------------------------\n");
                for (int i = 0; i < *numPlayers; ++i) {
                    printf("%2d. %-20s %-7d %-7d %s\n", i + 1, (*players)[i].name, (*players)[i].skill, (*players)[i].energy, isSelected[i] ? "*" : "");
                }
                int numSelected = 0;
                for (int i = 0; i < *numPlayers; i++) { if (isSelected[i]) numSelected++; }
                
                printf("\nSelected: %d/5. Enter player number, or 0 to confirm: ", numSelected);
                
                char selectionInput[10];
                fgets(selectionInput, sizeof(selectionInput), stdin);
                int playerNum = atoi(selectionInput);

                if (playerNum == 0) {
                    selectionDone = 1;
                } else if (playerNum > 0 && playerNum <= *numPlayers) {
                    isSelected[playerNum - 1] = !isSelected[playerNum - 1];
                }
            }
        } else if (overviewChoice == 0) {
            return;
        }
    }

    clearScreen();
    printf("The match is about to begin...\n");
    cross_platform_sleep(1);

    double myFinalBasePower = 0;
    for (int i = 0; i < *numPlayers; ++i) {
        if (isSelected[i]) {
            // More balanced weight between skill and fitness
            // Skill: 45%, Fitness: 45%, Random factor: 10%
            myFinalBasePower += ((*players)[i].skill * 11.0) + ((*players)[i].energy * 4.0);
        }
    }
    myFinalBasePower /= 5.0;

    // MODIFIED: Opponent power now depends on their real fitness
    double fitnessMultiplier = 0.6 + (league[opponent_idx].fitness / 200.0);
    double opponentFinalBasePower = league[opponent_idx].skillRating * fitnessMultiplier * difficultyMultiplier;

    double myTotalPower = myFinalBasePower + (rand() % 16);
    double opponentTotalPower = opponentFinalBasePower + (rand() % 16);

    int myGoals = 0, oppGoals = 0;

    for (int minute = 1; minute <= 9; minute++) {
        clearScreen();
        printf("==================== MATCH LIVE ====================\n\n");
        
        if (myTotalPower > (rand() % 250)) {
            myGoals++;
        }
        if (opponentTotalPower > (rand() % 250)) {
            oppGoals++;
        }

        printf("                    %d' MINUTE\n", minute * 10);
        printf("           %s %d - %d %s\n", myTeamName, myGoals, oppGoals, league[opponent_idx].name);
        printf("\n====================================================\n");
        
        cross_platform_sleep(1.5);
    }

    clearScreen();
    printf("\n==================== FULL TIME ======================\n");
    printf("            %s %d - %d %s\n", myTeamName, myGoals, oppGoals, league[opponent_idx].name);
    printf("=====================================================\n\n");
    
    for (int i = 0; i < *numPlayers; ++i) {
        if (isSelected[i]) {
            (*players)[i].energy = ((*players)[i].energy > 1) ? (*players)[i].energy - 1 : 1;
        } else {
            (*players)[i].energy = ((*players)[i].energy < 9) ? (*players)[i].energy + 2 : 10;
        }
        (*players)[i].value = (*players)[i].skill * 100.0 + (*players)[i].energy * 10.0;
    }

    int awardedMoney = 0;
    if (myGoals > oppGoals) { league[0].points += 3; printf("You WON! +3 points.\n"); awardedMoney = 100; } 
    else if (myGoals == oppGoals) { league[0].points += 1; league[opponent_idx].points += 1; printf("It's a DRAW. +1 point.\n"); awardedMoney = 50; } 
    else { league[opponent_idx].points += 3; printf("You LOST.\n"); awardedMoney = 20; }
    
    clubMoney += awardedMoney;
    printf("You received %d€ for the match. New balance: %.2f€.\n", awardedMoney, clubMoney);
    
    league[0].goalsFor += myGoals; league[0].goalsAgainst += oppGoals;
    league[opponent_idx].goalsFor += oppGoals; league[opponent_idx].goalsAgainst += myGoals;
    league[0].gamesPlayed++; league[opponent_idx].gamesPlayed++;
    
    currentMatchday++;
    simulateOtherMatches(currentMatchday);
    
    printf("\n--- Updated League Table ---\n");
    showLeagueTable();
    
    continueMatch(players, numPlayers);
}

void continueMatch(Player **players, int *numPlayers) {
    printf("\nPress Enter to continue to the transfer market...");
    getchar();
    buyRandomPlayer(players, numPlayers);
}

void buyRandomPlayer(Player **players_ptr, int *numPlayers) {
    clearScreen();
    
    Player randomPlayer;
    int nameIndex, tries = 0;
    do {
        nameIndex = rand() % (sizeof(playerNames) / sizeof(playerNames[0]));
        strncpy(randomPlayer.name, playerNames[nameIndex], sizeof(randomPlayer.name) - 1);
        randomPlayer.name[sizeof(randomPlayer.name) - 1] = '\0';
        tries++;
    } while (playerNameExists(*players_ptr, *numPlayers, randomPlayer.name) && tries < 100);
    
    randomPlayer.skill = 2 + (rand() % 4);
    randomPlayer.energy = rand() % 10 + 1;
    randomPlayer.value = randomPlayer.skill * 100.0 + randomPlayer.energy * 10.0;
    
    // Store original value for negotiations
    double originalValue = randomPlayer.value;
    double currentValue = originalValue;
    int negotiationFailed = 0; // Flag to track if negotiation has failed for this player
    
    // Constant failure chance for all negotiations
    const int FAILURE_CHANCE = 25;

    while (1) {
        clearScreen();
        printf("\n================ Transfer Market ================\n");
        printf("A player is available for purchase! (Club Money: %.2f€)\n", clubMoney);
        printf("Name: %-15s Skill: %-5d Energy: %-5d Value: %.0f€\n", randomPlayer.name, randomPlayer.skill, randomPlayer.energy, currentValue);
        
        if (negotiationsLeftThisSeason > 0 && !negotiationFailed) {
            printf("\nNegotiations left this season: %d\n", negotiationsLeftThisSeason);
        } else if (negotiationFailed) {
            printf("\n❌ Negotiation failed for this player!\n");
        }
        printf("\n=================================================\n");
        
        printf("\nTransfer Actions:\n");
        if (clubMoney >= currentValue) {
            printf("  1. Buy Player\n");
            if (negotiationsLeftThisSeason > 0 && !negotiationFailed) {
                printf("  2. Negotiate Price\n");
            }
            printf("  0. Skip\n");
            printf("\nEnter your choice: ");
        } else {
            printf("  You don't have enough money to buy this player.\n");
            printf("  Press Enter to continue...\n");
        }
        
        char input[10];
        if (clubMoney >= currentValue) {
            fgets(input, sizeof(input), stdin);
        } else {
            getchar();
            printf("\nReturning to main menu...\n");
            cross_platform_sleep(1);
            return;
        }

        // Check if user just pressed Enter (skip)
        int choice = -1;
        if (input[0] == '\n' || input[0] == '\0') {
            choice = 0; // Treat Enter as skip
        } else {
            choice = atoi(input);
        }
        
        if (choice == 1) {
            // Buy player
            if (clubMoney >= currentValue) {
                clubMoney -= currentValue;
                Player *new_players = realloc(*players_ptr, (*numPlayers + 1) * sizeof(Player));
                if (new_players == NULL) {
                    printf("FATAL: Memory allocation failed!\n");
                    clubMoney += currentValue;
                    return;
                }
                *players_ptr = new_players;
                (*players_ptr)[*numPlayers] = randomPlayer;
                (*numPlayers)++;
                printf("\n✅ Congratulations! You have purchased %s for %.0f€!\n", randomPlayer.name, currentValue);
                if (currentValue < originalValue) {
                    printf("You saved %.0f€ through negotiation!\n", originalValue - currentValue);
                }
                printf("New club balance: %.2f€\n", clubMoney);
            } else {
                printf("Sorry, you don't have enough money.\n");
            }
            break;
        } else if (choice == 2 && negotiationsLeftThisSeason > 0 && !negotiationFailed) {
            // Negotiation attempt
            negotiationsLeftThisSeason--;
            
            // Check if negotiation fails
            int randValue = rand() % 100;
            if (randValue < FAILURE_CHANCE) {
                // Negotiation failed
                printf("\n❌ Negotiation failed! The player is now demanding a 25%% premium.\n");
                currentValue = originalValue * 1.25;
                printf("New price: %.0f€\n", currentValue);
                negotiationFailed = 1; // Set flag to prevent further negotiations with this player
                printf("Press Enter to continue...");
                getchar();
            } else {
                // Negotiation succeeded
                printf("\n✅ Negotiation successful! The player has agreed to a 10%% reduction.\n");
                currentValue = currentValue * 0.9;
                printf("New price: %.0f€\n", currentValue);
                printf("Press Enter to continue...");
                getchar();
            }
        } else if (choice == 0) {
            // Skip player (also handles Enter key)
            printf("\nYou decided not to buy the player.\n");
            break;
        } else {
            printf("\nInvalid choice. Please try again.\n");
            printf("Press Enter to continue...");
            getchar();
        }
    }
    
    printf("\nPress Enter to return to the main menu...");
    getchar();
    printf("\nReturning to main menu...\n");
    cross_platform_sleep(1);
}

void initLeague(void) {
    char *teamNames[] = {"Red Dragons", "Blue Eagles", "Green Lions", "Yellow Tigers", "Black Panthers", "White Knights", "Purple Ravens", "Orange Bears", "Silver Wolves"};
    
    for (int i = 0; i < 10; i++) {
        if(i == 0) {
            strncpy(league[i].name, myTeamName, sizeof(league[i].name) - 1);
            league[i].name[sizeof(league[i].name) - 1] = '\0';
        } else {
            strncpy(league[i].name, teamNames[i-1], sizeof(league[i].name) - 1);
            league[i].name[sizeof(league[i].name) - 1] = '\0';
        }
        
        league[i].points = 0; 
        league[i].goalsFor = 0; 
        league[i].goalsAgainst = 0; 
        league[i].gamesPlayed = 0;
        league[i].skillRating = 26 + (rand() % 21);
        league[i].fitness = 100;
    }
}

void evolveLeague() {
    for (int i = 0; i < 10; i++) {
        if(i > 0) {
            int change = (rand() % 11) - 5;
            league[i].skillRating += change;
            if (league[i].skillRating > 50) league[i].skillRating = 50;
            if (league[i].skillRating < 30) league[i].skillRating = 30;
        }
        league[i].points = 0; 
        league[i].goalsFor = 0; 
        league[i].goalsAgainst = 0; 
        league[i].gamesPlayed = 0;
        league[i].fitness = 100;
    }
}

void showLeagueTable(void) {
    printf("%-4s %-20s %-5s %-5s %-5s %-5s %-5s\n", "Pos", "Team", "MP", "Pts", "GF", "GA", "GD");
    printf("----------------------------------------------------------\n");
    Team sortedLeague[10];
    memcpy(sortedLeague, league, sizeof(league));
    for (int i = 0; i < 10; i++) {
        for (int j = i + 1; j < 10; j++) {
            int diff_i = sortedLeague[i].goalsFor - sortedLeague[i].goalsAgainst;
            int diff_j = sortedLeague[j].goalsFor - sortedLeague[j].goalsAgainst;
            if (sortedLeague[j].points > sortedLeague[i].points || (sortedLeague[j].points == sortedLeague[i].points && diff_j > diff_i)) {
                Team temp = sortedLeague[i];
                sortedLeague[i] = sortedLeague[j];
                sortedLeague[j] = temp;
            }
        }
    }
    for (int i = 0; i < 10; i++) {
        printf("%-4d %-20s %-5d %-5d %-5d %-5d %+d\n", i + 1, sortedLeague[i].name, sortedLeague[i].gamesPlayed, sortedLeague[i].points, sortedLeague[i].goalsFor, sortedLeague[i].goalsAgainst, sortedLeague[i].goalsFor - sortedLeague[i].goalsAgainst);
    }
}

void endSeason(Player **players_ptr, int *numPlayers) {
    clearScreen();
    printf("\n================ FINAL LEAGUE STANDINGS ================\n");
    showLeagueTable();
    
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

    int myPosition = 0;
    for (int i = 0; i < 10; i++) {
        if (strcmp(sortedLeague[i].name, myTeamName) == 0) {
            myPosition = i + 1;
            break;
        }
    }

    printf("\nYou finished in position %d.\n", myPosition);
    int seasonReward = 500 - (myPosition - 1) * 25;
    if (seasonReward < 0) seasonReward = 0;
    clubMoney += seasonReward;
    printf("You received %d€ for your final position. New balance: %.2f€\n", seasonReward, clubMoney);
    
    if (myPosition == 1) {
        managerLevel++;
        printf("You are the champion! Manager level has increased to %d!\n", managerLevel);
    } else if (myPosition > 7) {
        if (managerLevel > 1) {
            managerLevel--;
            printf("A poor season finish. Your manager level has decreased to %d.\n", managerLevel);
        } else {
            printf("You finished in the bottom 3, but your level is already at the minimum.\n");
        }
    } else {
        printf("Your manager level remains at %d.\n", managerLevel);
    }

    printf("\nUpdating player stats for new season...\n");
    if(*numPlayers > 0) {
        for (int i = 0; i < *numPlayers; i++) {
            if ((*players_ptr)[i].skill > 1) { (*players_ptr)[i].skill--; }
            (*players_ptr)[i].energy = ((*players_ptr)[i].energy + 5 > 10) ? 10 : (*players_ptr)[i].energy + 5;
            (*players_ptr)[i].value = (*players_ptr)[i].skill * 100.0 + (*players_ptr)[i].energy * 10.0;
        }
        printf("All players' skills decreased by 1 and energy is restored.\n");
    }
    
    // Reset negotiations for the new season
    negotiationsLeftThisSeason = 10;
    printf("Negotiation attempts reset to 10 for the new season.\n");
    
    printf("\nPress Enter to continue to the off-season menu...");
    getchar();
}

int playerNameExists(Player *players, int numPlayers, const char *name) {
    if (players == NULL) return 0;
    for (int i = 0; i < numPlayers; i++) {
        if (strcmp(players[i].name, name) == 0) return 1;
    }
    return 0;
}

void sortPlayers(Player *players, int numPlayers) {
    if (players == NULL) return;
    for (int i = 0; i < numPlayers - 1; i++) {
        for (int j = 0; j < numPlayers - i - 1; j++) {
            if (players[j].skill < players[j + 1].skill) {
                Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }
}

void simulateOtherMatches(int matchday) {
    // Create an array of teams that haven't played yet (excluding player team and current opponent)
    int availableTeams[10];
    int availableCount = 0;
    
    // Mark teams that have already played
    int hasPlayed[10] = {0};
    hasPlayed[0] = 1; // Player's team
    if (matchday > 0 && matchday < 10) {
        hasPlayed[matchday] = 1; // Current opponent
        league[matchday].fitness -= 25;
    }
    
    // Build list of available teams
    for (int i = 1; i < 10; i++) {
        if (!hasPlayed[i]) {
            availableTeams[availableCount++] = i;
        }
    }
    
    // Shuffle the available teams to create random pairings
    for (int i = 0; i < availableCount; i++) {
        int j = i + rand() / (RAND_MAX / (availableCount - i) + 1);
        int temp = availableTeams[j];
        availableTeams[j] = availableTeams[i];
        availableTeams[i] = temp;
    }
    
    double difficultyMultiplier = 1.0;
    if (managerLevel == 1) difficultyMultiplier = 0.85;
    else difficultyMultiplier = 1.0 + ( (managerLevel - 2) * 0.05 );

    printf("\n--- Other Match Results ---\n");
    
    // Play 4 matches with proper pairings
    int matchesPlayed = 0;
    for (int i = 0; i < availableCount - 1; i += 2) {
        if (matchesPlayed >= 4) break;
        
        int team1 = availableTeams[i];
        int team2 = availableTeams[i + 1];
        
        // Calculate base power with fitness factor
        double fitnessMultiplier1 = 0.6 + (league[team1].fitness / 250.0);
        double fitnessMultiplier2 = 0.6 + (league[team2].fitness / 250.0);
        
        double team1Power = league[team1].skillRating * fitnessMultiplier1;
        double team2Power = league[team2].skillRating * fitnessMultiplier2;
        
        // Apply difficulty multiplier
        if (team1Power > team2Power) team1Power *= difficultyMultiplier;
        else team2Power *= difficultyMultiplier;
        
        // Determine the favorite team (higher power)
        int favoriteTeam = (team1Power > team2Power) ? team1 : team2;
        double powerDifference = fabs(team1Power - team2Power);
        
        // Base goals - higher skilled teams should score more on average
        double baseGoals = powerDifference / 15.0; // Scale the difference to goals
        if (baseGoals > 3.0) baseGoals = 3.0; // Cap the advantage
        
        // Generate goals with more variance
        int team1Goals = 0;
        int team2Goals = 0;
        
        // Simulate the match with more realistic scoring
        for (int minute = 1; minute <= 9; minute++) {
            // Each team has a chance to score each "minute"
            double team1Chance = team1Power / 150.0; // Base chance
            double team2Chance = team2Power / 150.0; // Base chance
            
            // Add some randomness
            team1Chance += ((double)rand() / RAND_MAX) * 0.5;
            team2Chance += ((double)rand() / RAND_MAX) * 0.5;
            
            // Favorite team gets a slight boost
            if (favoriteTeam == team1) team1Chance *= 1.15;
            else team2Chance *= 1.15;
            
            // Convert to actual goals (0-2 per "minute" to allow for bursts)
            if (((double)rand() / RAND_MAX) < team1Chance * 0.15) {
                team1Goals += 1 + (rand() % 2); // 1-2 goals
            }
            if (((double)rand() / RAND_MAX) < team2Chance * 0.15) {
                team2Goals += 1 + (rand() % 2); // 1-2 goals
            }
        }
        
        // Apply the base advantage for the stronger team
        if (favoriteTeam == team1) {
            team1Goals += (int)(baseGoals);
        } else {
            team2Goals += (int)(baseGoals);
        }
        
        // Add excitement - chance for blowout games or goalless draws
        if (powerDifference > 15) {
            // Big difference in skill - chance for blowout
            if (rand() % 4 == 0) { // 25% chance
                if (favoriteTeam == team1) team1Goals += 2 + (rand() % 3); // 2-4 extra
                else team2Goals += 2 + (rand() % 3); // 2-4 extra
            }
        } else if (powerDifference < 5) {
            // Close match - chance for low scoring or draws
            if (rand() % 3 == 0) { // 33% chance
                // Reduce goals for a tight match
                team1Goals = (team1Goals > 1) ? team1Goals - (1 + rand() % 2) : team1Goals;
                team2Goals = (team2Goals > 1) ? team2Goals - (1 + rand() % 2) : team2Goals;
                if (team1Goals < 0) team1Goals = 0;
                if (team2Goals < 0) team2Goals = 0;
            }
        }
        
        // Rare events - goalless draws or high scoring games
        if (rand() % 20 == 0) { // 5% chance for goalless draw
            team1Goals = 0;
            team2Goals = 0;
        } else if (rand() % 25 == 0) { // 4% chance for high scoring
            team1Goals += 2 + (rand() % 4); // 2-5 extra goals
            team2Goals += 1 + (rand() % 3); // 1-3 extra goals
        }
        
        // Cap goals to prevent unrealistic scores
        if (team1Goals > 8) team1Goals = 6 + (rand() % 3); // 6-8
        if (team2Goals > 8) team2Goals = 6 + (rand() % 3); // 6-8
        
        league[team1].goalsFor += team1Goals; league[team1].goalsAgainst += team2Goals;
        league[team2].goalsFor += team2Goals; league[team2].goalsAgainst += team1Goals;
        
        if (team1Goals > team2Goals) { 
            league[team1].points += 3;
            printf("%s %d-%d %s\n", league[team1].name, team1Goals, team2Goals, league[team2].name);
        } 
        else if (team1Goals == team2Goals) { 
            league[team1].points += 1; league[team2].points += 1;
            printf("%s %d-%d %s\n", league[team1].name, team1Goals, team2Goals, league[team2].name);
        } 
        else { 
            league[team2].points += 3;
            printf("%s %d-%d %s\n", league[team1].name, team1Goals, team2Goals, league[team2].name);
        }
        
        league[team1].gamesPlayed++; league[team2].gamesPlayed++;
        league[team1].fitness -= 25;
        league[team2].fitness -= 25;
        
        matchesPlayed++;
    }
    
    const int MINIMUM_FITNESS = 35;
    for (int i = 1; i < 10; i++) {
        if (league[i].fitness < 100) {
            int missing_fitness = 100 - league[i].fitness;
            league[i].fitness += (int)(missing_fitness * 0.5);
        }
        if (league[i].fitness < MINIMUM_FITNESS) {
            league[i].fitness = MINIMUM_FITNESS;
        }
    }
}
