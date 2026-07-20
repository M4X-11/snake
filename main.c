#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include "common.h"





void appleRand(int *x, int *y)
{
    int min = 0;
    int Xmax = 45;
    int Ymax = 17;

    *x = (rand() % Xmax) + min;
    *y = (rand() % Ymax) + min;
}

int isBodyAt(int x, int y, Segment body[], int bodyLength)
{
    for (int i = 0; i < bodyLength; i++) {
        if (body[i].x == x && body[i].y == y) {
            return 1;
        }
    }
    return 0;
}

int main()
{
    int mode;
    int server_socket;
    int connected = 0;
    int gameStarted = 0;
    
    game.apple[0].x = -1;
    game.apple[0].y = -1;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("socket");
        return 1;
    }

    // Address
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(6969);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(server_socket,
        (struct sockaddr*)&server_address,
        sizeof(server_address)) < 0)
    {
        perror("bind");
        return 1;
    }

    printf("select mode\n1: single apple\n2: individual apples\n[1/2]: ");
    scanf("%d", &mode);

    // Listen
    listen(server_socket, 5);

    printf("Server started\n");
    printf("Waiting for players...\n");
    printf("Type 'start' or press ENTER to begin\n");


    while (!gameStarted)
    {
        fd_set readfds;

        FD_ZERO(&readfds);

        // Watch keyboard
        FD_SET(STDIN_FILENO, &readfds);

        // Watch new connections
        FD_SET(server_socket, &readfds);


        int max_fd = server_socket;

        if (STDIN_FILENO > max_fd)
            max_fd = STDIN_FILENO;


        // Wait until something happens
        select(max_fd + 1, &readfds, NULL, NULL, NULL);


        // Someone typed something
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            char line[100];

            fgets(line, sizeof(line), stdin);

            if (strcmp(line, "\n") == 0 ||
                strcmp(line, "start\n") == 0)
            {
                gameStarted = 1;
                break;
            }
        }


        // A player connected
        if (FD_ISSET(server_socket, &readfds))
        {
            int client = accept(server_socket, NULL, NULL);


            if (client >= 0)
            {
                if (connected < MAX_PLAYERS)
                {
                    game.players[connected].socket = client;
                    game.players[connected].connected = 1;

                    connected++;

                    printf(
                        "Player connected (%d/%d)\n",
                        connected,
                        MAX_PLAYERS
                    );
                }
                else
                {
                    printf("Server full\n");
                    close(client);
                }
            }
        }


        // Automatically start if full
        if (connected == MAX_PLAYERS)
        {
            gameStarted = 1;
        }
    }


    printf("\nStarting game with %d players!\n", connected);

    for (int i=0; i<connected; i++){
        send(game.players[i].socket, &i, sizeof(int), 0);
    }

    for (int i = 0; i < connected; i++) {
        appleRand(&game.players[i].snake.x,
                &game.players[i].snake.y);

        game.players[i].snake.oldX = game.players[i].snake.x;
        game.players[i].snake.oldY = game.players[i].snake.y;
        game.players[i].snake.direction = RIGHT;
        game.players[i].snake.points = 0;
    }

    //init

    game.connections = connected;


    // GAME LOOP GOES HERE
    while (1)
    {
        // receive player input
        fd_set readfds;
        FD_ZERO(&readfds);

        int maxfd = -1;

        for (int i = 0; i < connected; i++) {
            FD_SET(game.players[i].socket, &readfds);

            if (game.players[i].socket > maxfd)
                maxfd = game.players[i].socket;
        }

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;      // 100 ms

        select(maxfd + 1, &readfds, NULL, NULL, &tv);

        for (int i = 0; i < connected; i++) {

            if (FD_ISSET(game.players[i].socket, &readfds)) {

                int dir;
                int n = recv(game.players[i].socket,
                            &dir,
                            sizeof(dir),
                            0);

                if (n > 0) {
                    game.players[i].snake.direction = dir;
                    printf("player[%d] direction: %d\n", i, dir);
                }
                else if (n == 0) {
                    printf("Player %d disconnected\n", i);
                    close(game.players[i].socket);
                    game.players[i].connected = 0;
                }
            }}

            //update snake head
            for (int i=0; i<connected; i++){
                game.players[i].snake.oldX = game.players[i].snake.x;
                game.players[i].snake.oldY = game.players[i].snake.y;
            }
            for (int i=0; i<connected; i++){
                switch (game.players[i].snake.direction)
                {
                    case UP:
                        game.players[i].snake.y--;
                        break;
                    case DOWN:
                        game.players[i].snake.y++;
                        break;
                    case LEFT:
                        game.players[i].snake.x--;
                        break;
                    case RIGHT:
                        game.players[i].snake.x++;
                        break;
                }
            }

            // update snake body
            for (int i=0; i<connected; i++){
                for (int j = game.players[i].snake.points; j > 0; j--)
                    game.players[i].snake.body[j] = game.players[i].snake.body[j-1];

                game.players[i].snake.body[0].x = game.players[i].snake.oldX;
                game.players[i].snake.body[0].y = game.players[i].snake.oldY;
            }

            //apple
            if (game.apple[0].x == -1) {
                appleRand(&game.apple[0].x, &game.apple[0].y);
            }

            for (int i=0; i<connected; i++){
                if (game.players[i].snake.x==game.apple[0].x && game.players[i].snake.y==game.apple[0].y){
                    game.players[i].snake.points++;
                    game.apple[0].y=-1;
                    game.apple[0].x=-1;
                }
            }
            

        
        // send game state
        for (int i=0; i<connected; i++){
            send(game.players[i].socket, &game, sizeof(game), 0);
        }
        usleep(100000);
    }


    close(server_socket);

    return 0;
}
