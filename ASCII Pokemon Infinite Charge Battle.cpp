#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cmath>

//define types of boosts of Pokemon
enum class BoostType { NONE, STATUS_PARALYZE, ACCURACY_DOWN };
//define status effects of Pokemon
enum class StatusEffect { NORMAL, PARALYZED };

//move structures
struct Move {
    std::string name;          //move name
    int baseDamage;            //base damage
    int chargeNeeded;          //charge turns needed: 0 = normal, 1 = basic boost, 2 = max boost
    BoostType boostType;       //boost type
    double accuracy;           //accuracy
};

//pokemon structures
struct Pokemon {
    std::string name;          //name
    int maxHP;                 //max HP
    int currentHP;             //current HP
    StatusEffect status;       //status effect variables
    std::vector<Move> moves;   //move list
    int level;                 //level, for Rattata only
};

//game constants
const int MAX_CHARGE_TURNS = 2;
const double BASE_BOOST_DAMAGE = 1.5;
const double MAX_BOOST_DAMAGE = 2.0;
const double BASE_BOOST_ACC = 0.9;
const double MAX_BOOST_ACC = 0.7;
const int PARALYZE_CHANCE = 30;  //paralysis chance%
const double PLAYER_HP_BUFF_RATE = 1.2;  //Pikachu HP boost rate
const double PLAYER_DAMAGE_BUFF_RATE = 1.15;  //Pikachu damage boost rate
const double WILD_HP_BUFF_RATE = 1.3;  //Wild Rattata HP boost rate
const double WILD_DAMAGE_BUFF_RATE = 1.5;  //Wild Rattata damage boost rate

//modular design function declarations
void initPlayerPokemon(Pokemon& playerPokemon);
void initWildPokemon(Pokemon& wildPokemon, int wave);
void displayBattleStatus(const Pokemon& player, const Pokemon& wild, int wave);
void displayASCIIArt(const std::string& pokemonName, int level = 1);
void displayBattleSceneASCII();
void displayGameOverASCII();
void displayLevelUpASCII();
int getPlayerChoice(const Pokemon& player);
bool isMoveChargeable(const Move& move);
void handleCharge(Pokemon& user, int& currentCharge, int selectedMoveIdx);
int calculateDamage(const Move& move, int chargeTurns);
bool checkHit(const Move& move, int chargeTurns);
void applyStatusEffect(Pokemon& target, BoostType boostType);
bool isFainted(const Pokemon& pokemon);
void resetCharge(int& currentCharge);
void buffPlayerPokemon(Pokemon& playerPokemon);
void battleLoop(Pokemon& playerPokemon, int& wave);

int main()
{
    srand(static_cast<unsigned int>(time(0)));  //set random seed
    
    std::cout << "============================================" << std::endl;
    std::cout << "        ASCII Pokemon: Infinite Charge Battle        " << std::endl;
    std::cout << "============================================" << std::endl;
    //display opening ASCII scene
    displayBattleSceneASCII();
    std::cout << "Game Rules:" << std::endl;
    std::cout << " - Defeat Wave N Rattata to get Pikachu +20% HP & +15% damage boost!" << std::endl;
    std::cout << " - 1-turn charge: ×1.5 damage, ×0.9 accuracy" << std::endl;
    std::cout << " - 2-turn charge: ×2.0 damage, ×0.7 accuracy, 30% paralysis chance" << std::endl;
    std::cout << " - Rattata gets stronger with each wave, damage increases significantly! Be careful!" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Press Enter to start adventure..." << std::endl;
    std::cin.ignore();

    Pokemon playerPokemon;
    initPlayerPokemon(playerPokemon);  //initialize player's Pikachu
    int wave = 1;  //current wave

    //infinite battle loop
    while (true) {
        Pokemon wildPokemon;
        initWildPokemon(wildPokemon, wave);  //initialize current wave Rattata
        std::cout << "============================================" << std::endl;
        std::cout << "🎉 Wave " << wave << " Challenge: Lv." << wave << " Furious Rattata!" << std::endl;
        std::cout << "============================================" << std::endl;
        displayASCIIArt(wildPokemon.name, wave);
        std::cout << "Press Enter to start battle..." << std::endl;
        std::cin.ignore();

        battleLoop(playerPokemon, wave);  //single wave battle loop

        //check if player is defeated
        if (isFainted(playerPokemon)) {
            displayGameOverASCII();  //display Game Over ASCII
            std::cout << "============================================" << std::endl;
            std::cout << "Game Over! You defeated " << wave - 1 << " Rattata total!" << std::endl;
            std::cout << "Final Wave: Wave " << wave << " (Lv." << wave << " Rattata)" << std::endl;
            std::cout << "============================================" << std::endl;
            break;
        }

        //boost Pikachu after victory
        buffPlayerPokemon(playerPokemon);
        displayLevelUpASCII();  //display level-up ASCII
        std::cout << "============================================" << std::endl;
        std::cout << "🏆 Congratulations! Defeated Wave " << wave << " Rattata!" << std::endl;
        std::cout << "⚡ Pikachu Boosted:" << std::endl;
        std::cout << " - Max HP increased to " << playerPokemon.maxHP << " (+20%)" << std::endl;
        std::cout << " - All moves +15% damage!" << std::endl;
        std::cout << "============================================" << std::endl;
        std::cout << "Next challenge: Lv." << wave + 1 << " Rattata (Higher damage!)" << std::endl;
        std::cout << "Press Enter to continue..." << std::endl;
        std::cin.ignore();

        wave++;  //increase wave number
    }

    return 0;
}

//initialize player's Pikachu
void initPlayerPokemon(Pokemon& playerPokemon) {
    playerPokemon.name = "Pikachu";
    playerPokemon.maxHP = 120;
    playerPokemon.currentHP = 120;
    playerPokemon.status = StatusEffect::NORMAL;
    //initial move list with base damage)
    playerPokemon.moves = {
        {"Quick Attack", 25, 0, BoostType::NONE, 0.95},  //normal move
        {"Thunder Shock", 40, 1, BoostType::STATUS_PARALYZE, 0.90},  //charge move
        {"Thunderbolt", 60, 0, BoostType::ACCURACY_DOWN, 0.75}   //power move
    };
}

//initialize wild Rattata boosted damage
void initWildPokemon(Pokemon& wildPokemon, int wave) {
    wildPokemon.name = "Furious Rattata";
    wildPokemon.level = wave;
    
    //base stats × wave boost rate
    int baseHP = 90;
    int baseMove1 = 30;  //Tackle base damage
    int baseMove2 = 45;  //Scratch base damage
    wildPokemon.maxHP = static_cast<int>(baseHP * std::pow(WILD_HP_BUFF_RATE, wave - 1));
    wildPokemon.currentHP = wildPokemon.maxHP;
    wildPokemon.status = StatusEffect::NORMAL;
    //Rattata move boost 1.5 damage rate improved base damage
    wildPokemon.moves = {
        {"Tackle", static_cast<int>(baseMove1 * std::pow(WILD_DAMAGE_BUFF_RATE, wave - 1)), 0, BoostType::NONE, 0.98},
        {"Scratch", static_cast<int>(baseMove2 * std::pow(WILD_DAMAGE_BUFF_RATE, wave - 1)), 0, BoostType::NONE, 0.85}
    };
}

//display battle status with ASCII and move damage label
void displayBattleStatus(const Pokemon& player, const Pokemon& wild, int wave) {
    std::cout << "============================================" << std::endl;
    std::cout << "📌 Wave " << wave << " | Lv." << wild.level << " " << wild.name << std::endl;
    std::cout << "============================================" << std::endl;
    
    //Pikachu ASCII and status
    std::cout << "[Your Pokemon]" << std::endl;
    displayASCIIArt(player.name);
    std::cout << std::left << std::setw(15) << player.name << " | HP: " 
              << player.currentHP << "/" << player.maxHP << " | Status: ";
    if (player.status == StatusEffect::PARALYZED) std::cout << "Paralyzed (Cannot act)";
    else std::cout << "Normal";
    std::cout << "\n" << std::endl;

    //wild Rattata ASCII and status
    std::cout << "[Opponent Pokemon]" << std::endl;
    displayASCIIArt(wild.name, wild.level);
    std::cout << std::left << std::setw(15) << wild.name << " | HP: " 
              << wild.currentHP << "/" << wild.maxHP << " | Status: ";
    if (wild.status == StatusEffect::PARALYZED) std::cout << "Paralyzed (Cannot act)";
    else std::cout << "Normal";
    std::cout << "\n============================================" << std::endl;

    //display player's moves with current damage
    std::cout << "🔧 Your Moves (Current Damage):" << std::endl;
    std::cout << "1. " << player.moves[0].name << " | Damage: " << player.moves[0].baseDamage << " | Accuracy: " << player.moves[0].accuracy * 100 << "%" << std::endl;
    std::cout << "2. " << player.moves[1].name << " | Base Damage: " << player.moves[1].baseDamage 
              << " | 1-turn charge: " << static_cast<int>(player.moves[1].baseDamage * BASE_BOOST_DAMAGE)
              << " | 2-turn charge: " << static_cast<int>(player.moves[1].baseDamage * MAX_BOOST_DAMAGE)
              << " | Accuracy: " << player.moves[1].accuracy * 100 << "%" << std::endl;
    std::cout << "3. " << player.moves[2].name << " | Damage: " << player.moves[2].baseDamage << " | Accuracy: " << player.moves[2].accuracy * 100 << "%" << std::endl;
    std::cout << "============================================" << std::endl;
}

//display Pokemon ASCII
void displayASCIIArt(const std::string& pokemonName, int level) {
    if (pokemonName == "Pikachu") {
        //Pikachu ASCII
        std::cout << "  /\\_/\\  ⚡" << std::endl;
        std::cout << " ( o.o ) 💛" << std::endl;
        std::cout << "  > ^ <  ⚡" << std::endl;
    } else if (pokemonName == "Furious Rattata") {
        //Rattata evolves with level
        if (level <= 2) {
            std::cout << "  /\\___/\\ 🐀" << std::endl;
            std::cout << " ( o   o )" << std::endl;
            std::cout << "   > ^ <" << std::endl;
        } else if (level <= 4) {
            std::cout << "  /\\___/\\ 🐀" << std::endl;
            std::cout << " ( 🔥   🔥 )" << std::endl;                                             
            std::cout << "   > v <" << std::endl;
        } else {
            std::cout << "  /\\___/\\ 🐀" << std::endl;
            std::cout << " ( 💥   💥 )" << std::endl;
            std::cout << "   > v <" << std::endl;
            std::cout << "  [Wings Deployed]  " << std::endl;
        }
    }
}

//display battle scene ASCII
void displayBattleSceneASCII() {
    std::cout << "============================================" << std::endl;
    std::cout << "        Viridian Forest Outskirts · Furious Grassland        " << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "  /|\\    /|\\    /|\\    /|\\" << std::endl;
    std::cout << " / | \\  / | \\  / | \\  / | \\" << std::endl;
    std::cout << "/__|__\\/__|__\\/__|__\\/__|__\\" << std::endl;
    std::cout << "  ||     ||     ||     || " << std::endl;
    std::cout << "  ||     ||     ||     ||    " << std::endl;
    std::cout << "  ||     ||     ||     ||    " << std::endl;
    std::cout << "============================================" << std::endl;
}

//display Game Over ASCII
void displayGameOverASCII(){
    std::cout << "┌-----------------------------------┐" << std::endl;
    std::cout << "                                     " << std::endl;
    std::cout << "          G A M E  O V E R           " << std::endl;
    std::cout << "                                     " << std::endl;
    std::cout << "          /\\_/\\    /\\___/\\       " << std::endl;
    std::cout << "         ( x.x )  ( o   o )          " << std::endl;
    std::cout << "          > ^ <     > v <            " << std::endl;
    std::cout << "└-----------------------------------┘" << std::endl;

}

//display level-up boost ASCII
void displayLevelUpASCII() {
    std::cout << "============================================" << std::endl << std::endl;
    std::cout << "⚡      Pikachu Boosted Successfully!      ⚡" << std::endl << std::endl;
    std::cout << "                 /\\_/\\ " << std::endl;    //
    std::cout << "                (🌟 .🌟 )  " << std::endl;
    std::cout << "                 > ^ <   " << std::endl;
    std::cout << "⚡      Pikachu Boosted Successfully!      ⚡" << std::endl << std::endl;
}

//get player's choice
int getPlayerChoice(const Pokemon& player) {
    int choice;
    std::cout << "Choose Action:" << std::endl;
    std::cout << "1. Attack (" << player.moves[0].name << " | Damage: " << player.moves[0].baseDamage << ")" << std::endl;
    std::cout << "2. Charge (" << player.moves[1].name << " | Double damage after charge)" << std::endl;
    std::cout << "3. Power Attack (" << player.moves[2].name << " | Damage: " << player.moves[2].baseDamage << ")" << std::endl;
    std::cout << "4. Run (Abandon current wave, game over)" << std::endl;
    std::cout << "Enter choice (1-4): ";
    std::cin >> choice;

    //input validation
    while (std::cin.fail() || choice < 1 || choice > 4) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "XXX Invalid input! Enter a number between 1-4: ";
        std::cin >> choice;
    }
    return choice;
}

//check if move is chargeable
bool isMoveChargeable(const Move& move) {
    return move.chargeNeeded > 0;
}
//charge move logic
void handleCharge(Pokemon& user, int& currentCharge, int selectedMoveIdx) {
    Move& selectedMove = user.moves[selectedMoveIdx];
    if (!isMoveChargeable(selectedMove)) {
        std::cout << user.name << " can't charge this move!" << std::endl;
        return;
    }
    if (currentCharge >= MAX_CHARGE_TURNS) {
        std::cout << user.name << " already has max charge! No need to charge again!" << std::endl;
        return;
    }
    currentCharge++;
    std::cout << "⚡⚡⚡  " << user.name << " starts charging \"" << selectedMove.name << "\"!  ⚡⚡⚡" << std::endl;
    std::cout << "📊 Current charge progress: " << currentCharge << "/" << MAX_CHARGE_TURNS << " turns" << std::endl;
    for (int i = 0; i < currentCharge; i++) {
        std::cout << "🔥🔥🔥  Energy accumulating...  🔥🔥🔥" << std::endl;
    }
}

//calculate move damage
int calculateDamage(const Move& move, int chargeTurns) {
    if (chargeTurns == 0) {
        return move.baseDamage;
    } else if (chargeTurns == 1) {
        return static_cast<int>(move.baseDamage * BASE_BOOST_DAMAGE);
    } else {  // chargeTurns == 2
        return static_cast<int>(move.baseDamage * MAX_BOOST_DAMAGE);
    }
}

//check if move hits
bool checkHit(const Move& move, int chargeTurns) {
    double finalAcc;
    if (chargeTurns == 0) {
        finalAcc = move.accuracy;
    } else if (chargeTurns == 1) {
        finalAcc = move.accuracy * BASE_BOOST_ACC;
    } else {  // chargeTurns == 2
        finalAcc = move.accuracy * MAX_BOOST_ACC;
    }
    //random number from 0-99
    int randomNum = rand() % 100;
    return randomNum < (finalAcc * 100);  
}

//apply status effect
void applyStatusEffect(Pokemon& target, BoostType boostType) {
    if (boostType != BoostType::STATUS_PARALYZE) return;
    std::cout << "!!! Paralysis effect triggering..." << std::endl;
    //random number from 0-99
    int randomChance = rand() % 100;
    if (randomChance < PARALYZE_CHANCE) {
        target.status = StatusEffect::PARALYZED;
        std::cout << "⚡⚡⚡ " << target.name << " is paralyzed! Cannot act next turn! ⚡⚡⚡" << std::endl;
    } else {
        std::cout << target.name << " resisted the paralysis effect!" << std::endl;
    }
}

//check if Pokemon is fainted
bool isFainted(const Pokemon& pokemon) {
    return pokemon.currentHP <= 0;
}
//reset charge progress
void resetCharge(int& currentCharge) {
    currentCharge = 0;
}

//boost player's Pikachu
void buffPlayerPokemon(Pokemon& playerPokemon) {
    //HP boost
    playerPokemon.maxHP = static_cast<int>(playerPokemon.maxHP * PLAYER_HP_BUFF_RATE);
    playerPokemon.currentHP = playerPokemon.maxHP;  //fully healed
    //boost all moves' damage
    for (auto& move : playerPokemon.moves) { //(Search and Sorting Algorithms)
        move.baseDamage = static_cast<int>(move.baseDamage * PLAYER_DAMAGE_BUFF_RATE);
    }
}

//single wave battle main loop
void battleLoop(Pokemon& playerPokemon, int& wave) {
    Pokemon wildPokemon;
    initWildPokemon(wildPokemon, wave);  //reinitialize current wave Rattata to prevent boost residue
    int playerCharge = 0;  //player's current charge turns
    bool playerFled = false;

    //until faint or run
    while (!isFainted(playerPokemon) && !isFainted(wildPokemon) && !playerFled) {
        displayBattleStatus(playerPokemon, wildPokemon, wave);

        //player's turn
        if (playerPokemon.status == StatusEffect::PARALYZED) {
            std::cout << "⚡ Your " << playerPokemon.name << " is paralyzed! Cannot act! ⚡" << std::endl;
        } else {
            int choice = getPlayerChoice(playerPokemon);
            switch (choice) {
                case 1: {  //normal attack: Quick Attack
                    Move& move = playerPokemon.moves[0];
                    std::cout << "Your " << playerPokemon.name << " used \"" << move.name << "\"!" << std::endl;
                    std::cout << "💨💨💨 Quick Attack! Max speed! 💨💨💨" << std::endl;
                    if (checkHit(move, 0)) {
                        int damage = calculateDamage(move, 0);
                        wildPokemon.currentHP -= damage;
                        if (wildPokemon.currentHP < 0) wildPokemon.currentHP = 0;
                        std::cout << "💥 Dealt " << damage << " damage! 💥" << std::endl;
                        std::cout << wildPokemon.name << " Remaining HP: " << wildPokemon.currentHP << "/" << wildPokemon.maxHP << std::endl;
                    } else {
                        std::cout << "XXX The move missed! What a pity! XXX" << std::endl;
                    }
                    resetCharge(playerCharge);
                    break;
                }
                case 2: {  //charge: Thunder Shock
                    handleCharge(playerPokemon, playerCharge, 1);
                    break;
                }
                case 3: {  //Power attack: Thunderbolt
                    Move& move = playerPokemon.moves[2];
                    std::cout << "Your " << playerPokemon.name << " used \"" << move.name << "\"!" << std::endl;
                    std::cout << "⚡⚡⚡ Thunderbolt Outbreak！Electricity courses through the field! ⚡⚡⚡" << std::endl;
                    if (checkHit(move, 0)) {
                        int damage = calculateDamage(move, 0);
                        wildPokemon.currentHP -= damage;
                        if (wildPokemon.currentHP < 0) wildPokemon.currentHP = 0;
                        std::cout << "💥 Dealt " << damage << " high damage! 💥" << std::endl;
                        std::cout << wildPokemon.name << " Remaining HP: " << wildPokemon.currentHP << "/" << wildPokemon.maxHP << std::endl;
                    } else {
                        std::cout << "XXX The move missed! What a pity! XXX" << std::endl;
                    }
                    resetCharge(playerCharge);
                    break;
                }
                case 4: {  //chose to run
                    std::cout << "🏃 You chose to run! 🏃" << std::endl;
                    std::cout << "Game Over! You defeated " << wave - 1 << " Rattata total!" << std::endl;
                    playerFled = true;
                    playerPokemon.currentHP = 0;  //HP mark as defeated
                    break;
                }
            }
        }

        if (playerFled || isFainted(wildPokemon)) break;  //exit loop
        std::cin.ignore();
        std::cout << "\nPress Enter to continue battle..." << std::endl;
        std::cin.get(); //wait for user

        //wild Pokemon's turn
        displayBattleStatus(playerPokemon, wildPokemon, wave);
        if (wildPokemon.status == StatusEffect::PARALYZED) {
            std::cout << "⚡ " << wildPokemon.name << " is paralyzed! Cannot act! ⚡" << std::endl;
        } else {
            
            //randomly select move
            int randomMoveIdx = rand() % wildPokemon.moves.size();  
            Move& move = wildPokemon.moves[randomMoveIdx];
            std::cout << "🐀🐀🐀 " << wildPokemon.name << " used \"" << move.name << "\"! 🐀🐀🐀" << std::endl;
            if (move.name == "Tackle") std::cout << "💥💥💥 Powerful Tackle! Full power! 💥💥💥" << std::endl;
            else if (move.name == "Scratch") std::cout << "Claws claws claws Furious Scratch! Claws claws claws " << std::endl;
            if (checkHit(move, 0)) {
                int damage = calculateDamage(move, 0);
                playerPokemon.currentHP -= damage;
                if (playerPokemon.currentHP < 0) playerPokemon.currentHP = 0;
                std::cout << "💥 You took " << damage << " damage! 💥" << std::endl;
                std::cout << "Your " << playerPokemon.name << " Remaining HP: " << playerPokemon.currentHP << "/" << playerPokemon.maxHP << std::endl;
            } else {
                std::cout << "XXX " << wildPokemon.name << "'s move missed! XXX" << std::endl;
            }
        }

        //release charged move if fully charged
        if (playerCharge >= MAX_CHARGE_TURNS) {
            std::cin.ignore();
            std::cout << "\nPress Enter to release max charge move!" << std::endl;
            std::cin.get();   //wait for user
            displayBattleStatus(playerPokemon, wildPokemon, wave);
            
            Move& boostedMove = playerPokemon.moves[1];
            std::cout << "🔥🔥🔥 Your " << playerPokemon.name << " has max charge! 🔥🔥🔥" << std::endl;
            std::cout << "⚡⚡⚡⚡⚡ Unleash \"" << boostedMove.name << "\" with full power! ⚡⚡⚡⚡⚡" << std::endl;
            std::cout << "⚡💥⚡💥⚡ Thunder and lightning! Heaven and earth shake! ⚡💥⚡💥⚡" << std::endl;
            
            if (checkHit(boostedMove, playerCharge)) {  //charger move hits
                int damage = calculateDamage(boostedMove, playerCharge);
                wildPokemon.currentHP -= damage;
                if (wildPokemon.currentHP < 0) wildPokemon.currentHP = 0;
                std::cout << "💥💥💥 Dealt " << damage << " explosive damage! 💥💥💥" <<std::endl;
                std::cout << wildPokemon.name << " Remaining HP: " << wildPokemon.currentHP << "/" << wildPokemon.maxHP << std::endl;
                //apply paralysis
                applyStatusEffect(wildPokemon, boostedMove.boostType);
            } else {
                std::cout << "XXX The charged move missed! What a pity! XXX" << std::endl;
            }
            resetCharge(playerCharge);  //reset charge
        }
        if (isFainted(playerPokemon)) break;  //exit if Pikachu fainted
        std::cin.ignore();
        std::cout << "\nPress Enter to start next turn..." << std::endl;
        std::cin.get();  //wait for user
    }

    //single wave battle end settlement
    if (playerFled) return;
    if (isFainted(wildPokemon)) {
        std::cout << "============================================" << std::endl;
        std::cout << "🎉🎉🎉 Congratulations! Defeated Wave " << wave << " Lv." << wildPokemon.level << " "<< wildPokemon.name << "! 🎉🎉🎉" << std::endl;
        std::cout << "💥💥💥 Battle Victory! 💥💥💥" << std::endl;
        std::cout << "============================================" << std::endl;
        std::cin.ignore();
        std::cout << "Press Enter to claim reward..." << std::endl;
        std::cin.get();  //wait for user
    }
}