#define MAX_PLAYERS 32

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
    Segment body[1000];
} Snake;



typedef struct {
    int socket;
    Snake snake;
    int connected;
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
