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

    mvprintw(0, 0, "Points: %d", packet.players[me].points);

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
        attron(COLOR_PAIR(i+1));
        for (int j = 0; j < packet.players[i].points; j++){
            mvaddch(game.players[i].snake.body[j].y + 2, game.players[i].snake.body[j].x + 1, '*');
            }
        attroff(COLOR_PAIR(i+1));
        }

    // Apple
    mvaddch(packet.apple->y + 2, packet.apple->x + 1, 'o');

    // Head
    for (int i=0; i<game.connections; i++){
        attron(COLOR_PAIR(i+1));
        mvaddch(game.players[i].snake.y + 2, game.players[i].snake.x + 1, '@');
        attroff(COLOR_PAIR(i+1));
    }
    
    if (packet.players[me].dead=='d'){
        mvprintw(20, 0, "DEAD");
    }


    mvprintw(23, 0, "Package size: %d", bytes);
    mvprintw(21, 0, "Package size: %zu", sizeof(Packets));
    refresh();
    return 0;
}

int recvAll(int sock, void *buffer, int size)
{
    int total = 0;

    while (total < size)
    {
        int n = recv(sock,
                     (char *)buffer + total,
                     size - total,
                     0);

        if (n <= 0)
            return n;

        total += n;
    }

    return total;
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
    while (connection_status ==-1) {
        int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
        usleep(100000);
        return 1;
    }

    recv(network_socket, &me, sizeof(int), 0);

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(4, COLOR_BLUE,    COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN,    COLOR_BLACK);
    init_pair(7, COLOR_WHITE,   COLOR_BLACK);


    while (1)
    {

        //update snake head
            for (int i=0; i<game.connections; i++){
                game.players[i].snake.oldX = game.players[i].snake.x;
                game.players[i].snake.oldY = game.players[i].snake.y;
            }
        //get res
        
        //printf("\nsend: ");
        
        int key = getch();
        

        switch (key)
        {
            case KEY_UP:
                if (game.players[me].snake.direction != DOWN)
                {
                    game.players[me].snake.direction = UP;
                }
                break;
            case KEY_DOWN:
                if (game.players[me].snake.direction != UP)
                {
                    game.players[me].snake.direction = DOWN;
                }
                break;
            case KEY_LEFT:
                if (game.players[me].snake.direction != RIGHT)
                {
                    game.players[me].snake.direction = LEFT;
                }
                break;
            case KEY_RIGHT:
                if (game.players[me].snake.direction != LEFT)
                {
                    game.players[me].snake.direction = RIGHT;
                }
                break;
        }

        if (key != ERR){
            send(network_socket, &game.players[me].snake.direction, sizeof(game.players[0].snake.direction), 0);
        }
        
        

        //recv(network_socket, &game, sizeof(game), 0);
        bytes = recvAll(network_socket, &packet, sizeof(packet));
        for (int i=0; i<packet.connections; i++){
                game.players[i].snake.x = packet.players[i].x;
                game.players[i].snake.y = packet.players[i].y;
            }
        game.connections=packet.connections;
        
        //bytes = recv(network_socket, &game, sizeof(game), 0);

        // update snake body
            for (int i=0; i<game.connections; i++){
                for (int j = packet.players[i].points; j > 0; j--)
                    game.players[i].snake.body[j] = game.players[i].snake.body[j-1];

                game.players[i].snake.body[0].x = game.players[i].snake.oldX;
                game.players[i].snake.body[0].y = game.players[i].snake.oldY;
            }

        displ();

        refresh();
        
        
        //printf("You entered: %s\n", line);
        //send(network_socket, &players[0].snake.direction, sizeof(players[0].snake.direction), 0);

    }
    endwin();
    return 0;
}
