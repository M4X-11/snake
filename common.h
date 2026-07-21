#define MAX_PLAYERS 6

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct {

    int x;
    int y;
} Segment;

typedef struct {

    int x;
    int y;
    int oldX;
    int oldY;
    int points;
    int direction;
    Segment body[50];
} Snake;



typedef struct {
    int socket;
    Snake snake;
    int connected;
    int dead;
} Player;

//Player players[MAX_PLAYERS];

typedef struct {

    int x;
    int y;
} AppleS;

//AppleS Apple[MAX_PLAYERS];

typedef struct {

    AppleS apple[MAX_PLAYERS];
    Player players[MAX_PLAYERS];
    int connections;
} Package;

Package game;

////////

typedef struct {
    
    int x;
    int y;
    
} PlayerP;

typedef struct {
    AppleS apple[MAX_PLAYERS];
    PlayerP players[MAX_PLAYERS];
} Packets;
Packets packet;