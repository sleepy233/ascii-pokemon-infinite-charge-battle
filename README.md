# ascii-pokemon-infinite-charge-battle
Turn‑based wave‑survival text C++ game with ASCII art, charge‑attack mechanics and status effects

Control Pikachu to fight endless waves of Furious Rattata in Viridian Forest Outskirts.
Use charge‑powered attacks, paralysis status effects, and permanent stat boosts to survive as many waves as you can.

## 📂 Project Files
- `ASCII Pokemon Infinite Charge Battle.cpp`: Full C++ source code

## 🎮 Game Features
- Turn‑based battle system with 3 unique moves for Pikachu
- **Charge mechanic**: Charge Thunder Shock up to 2 turns for higher damage, at cost of accuracy
- Paralysis status effect: paralyzed Pokémon skip their next turn
- Wave progression: Every win gives Pikachu permanent HP & damage buffs; Rattata grows exponentially stronger each wave
- Dynamic ASCII art that changes appearance based on Rattata level
- Input validation, random hit/miss probability, complete status‑effect logic

## 🛠 How to Run
```bash
g++ "ASCII Pokemon Infinite Charge Battle.cpp" -o pokemon_battle
./pokemon_battle
```
When you see `Press Enter`, press your Enter key to proceed. If the program freezes, hit Enter.

## 📖 Game Rules
- Defeat Wave‑N Rattata → Pikachu gains +20% max HP, +15% damage for all moves, and is fully healed.
- 1‑turn charge: ×1.5 damage, 0.9× accuracy
- 2‑turn max charge: ×2.0 damage, 0.7× accuracy, 30% chance to paralyze target
- Rattata’s HP and damage scale up every wave.
- If Pikachu faints or you choose to run away, the game ends. Try to beat your highest wave record!

## 📖 Game Rules
1. **Quick Attack**: Reliable high‑accuracy normal attack, resets charge progress
2. **Thunder Shock**: Charge‑able move. Build charge over multiple turns to unleash boosted damage and paralysis.
3. **Thunderbolt**: Heavy‑hitting power move with lower accuracy, resets charge progress
4. **Run**: Abandon current battle → Game Over

## Notes
This is an older project I stumbled across while looking for my Valorant gameplay clips lol >-< .
