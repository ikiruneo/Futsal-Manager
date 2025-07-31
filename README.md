
# Football Manager Game

Welcome to the Football Manager Game, a text-based football management simulation written in C. Take control of your own club, manage your squad, compete in a league, and lead your team to glory!

---

## Features

- **Player Management:**
  - List all players in your squad with detailed stats (name, skill, energy, value).
  - Sell players to increase your club's funds (must keep at least 5 players).
  - Buy new random players after each match (if you have enough money).

- **Financial Management:**
  - Track your club's money and manager level.
  - Earn money for match results and end-of-season position.

- **League Competition:**
  - Compete in a 10-team league, playing 9 matches per season.
  - Each matchday, select your starting 5 players from your squad.
  - Opponents have varying skill and energy each match.
  - League table updates after every match.
  - At season's end, receive rewards based on your final position and progress to the next season.

- **Progression:**
  - Manager level increases if you win the league.
  - Players' skills decrease and energy increases at the end of each season.

---

## Controls & Gameplay

1. **Start the Game:**
   - Enter your team name when prompted.
   - Your initial squad is randomly generated.

2. **Main Menu Options:**
   - `1. List Players` — View your squad and optionally sell players.
   - `2. Print Stats` — View your manager level and club money.
   - `3. Play League Match` — (if season is ongoing) Play the next match.
   - `4. Show League Table` — View the current league standings.
   - `0. Quit` — Exit the game.

3. **Match Day:**
   - See your opponent's average skill and energy.
   - Select 5 players by their numbers to form your starting lineup.
   - After the match, see the result, awarded money, and updated league table.
   - Buy a new random player if you wish (and can afford it).

4. **Season End:**
   - After 9 matches, the season ends.
   - See your final position, receive a reward, and start a new season with updated player stats.

---

## How to Play

1. Clone or download this repository to your local machine.
2. Compile the source code using a C compiler:
   ```sh
   gcc fm.c -o football_manager
   ```
3. Run the game:
   ```sh
   ./football_manager
   ```
4. Follow the on-screen prompts to manage your team and progress through the league.

---

## Tips

- Keep at least 5 players in your squad to avoid being unable to play matches.
- Balance your squad's skill and energy for best results.
- Winning matches and finishing higher in the league earns more money.
- Use your funds wisely to strengthen your team.

---

**Disclaimer:** This game and all associated content are for demonstration and entertainment purposes only.
