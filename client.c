#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include "common.h"


int me;
int bytes;

int displ()
{
    clear();

    mvprintw(0, 0, "Points: %d", game.players[me].snake.points);

    for (int i = 0; i < 47; i++)
        mvaddch(1, i, '#');

    for (int i = 0; i < 17; i++)
    {
        mvaddch(i + 2, 0, '#');
        mvaddch(i + 2, 46, '#');
    }

    for (int i = 0; i < 47; i++)
        mvaddch(19, i, '#');

    // Draw body
    for (int i=0; i<game.connections; i++){
        for (int j = 0; j < game.players[i].snake.points; j++)
            mvaddch(game.players[i].snake.body[j].y + 2, game.players[i].snake.body[j].x + 1, '*');
            }
    

    // Apple
    mvaddch(game.apple->y + 2, game.apple->x + 1, 'o');

    // Head
    for (int i=0; i<game.connections; i++){
        mvaddch(game.players[i].snake.y + 2, game.players[i].snake.x + 1, '@');
    }
    

    mvprintw(23, 0, "Package size: %d", bytes);
    mvprintw(21, 0, "Package size: %zu", sizeof(Package));
    refresh();
    return 0;
}


int main() {
    char line[1024];

    //create
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //specify
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(6969);
    //server_address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "144.24.205.234", &server_address.sin_addr);

    //create connect
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status ==-1) {
        printf("ERROR");
        return 1;
    }

    recv(network_socket, &me, sizeof(int), 0);

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);


    while (1)
    {

        //get res
        
        //printf("\nsend: ");
        
        int key = getch();
        

        switch (key)
        {
            case KEY_UP:
                game.players[me].snake.direction = UP;
                break;
            case KEY_DOWN:
                game.players[me].snake.direction = DOWN;
                break;
            case KEY_LEFT:
                game.players[me].snake.direction = LEFT;
                break;
            case KEY_RIGHT:
                game.players[me].snake.direction = RIGHT;
                break;
        }

        if (key != ERR){
            send(network_socket, &game.players[me].snake.direction, sizeof(game.players[0].snake.direction), 0);
        }
        

        //recv(network_socket, &game, sizeof(game), 0);
        bytes = recv(network_socket, &game, sizeof(game), 0);

        displ();

        refresh();
        
        
        //printf("You entered: %s\n", line);
        //send(network_socket, &players[0].snake.direction, sizeof(players[0].snake.direction), 0);

    }
    endwin();
    return 0;
}
