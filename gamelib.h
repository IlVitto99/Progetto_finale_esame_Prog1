#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//Color codes for UNIX
#define COL(x) "\033[" #x ";1m" 
#define COL_RED COL(31)
#define COL_GREEN COL(32)
#define COL_YELLOW COL(33)
#define COL_BLUE COL(34)
#define COL_WHITE COL(37)
#define COL_GRAY "\033[0m"

//to ease the screen cleaning
#define clr system("clear")

//define a random number from 0 to 100
#define prob rand()%101


//to ease player's state management
enum Player_state { 
    vulnerable,
    life_shield,
    only_shield,
    only_life,
    dead
};

//to ease danger's type management
enum Type_of_danger { 
    alien,
    trap,
    no_danger,
};

//to ease Object's type management
enum Objects{ 
    no_objects,
    medikit,
    potion,
    materials,
    rocket_launcher_bullet,
};

//semplification of new types's name to ease a future use
typedef enum Type_of_danger Tod_t;
typedef enum Objects Obj_t;
typedef enum Player_state Pstate_t;

//fundamental unit of the chessboard
struct Cell{
    Tod_t dancell;
    Obj_t objcell;
    int pg; //flag to verify player's presence
};

//tower, built by player during the final fase of the game
struct tower{
    short num; //tower's floor
    struct tower *pnext;
};

//this struct contains player's attributes and positioning in the chessboard
struct Player{
    int x;
    int y;
    int kills; //number of killed aliens
    int zaino[4];
    int turn; //actual round
    char name[10];
    Pstate_t state; //healt state of the player
    struct tower *FTower; //pointer to the first element of player's tower
    struct tower *LTower; //pointer to the last element of player's tower
};

//semplification of the name of new structs to ease a future use
typedef struct tower Tw;
typedef struct Player Pg_t;
typedef struct Cell Cell_t;

void Menu(bool *check); //function for the main menu

void play(void); //the core of the game

void cleanplayer(void); //to clean player's status and to free heap from linked lists

void cleanchess(void); //to free chessboard's allocated memory in heap