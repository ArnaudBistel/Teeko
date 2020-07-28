#ifndef ROUTER_H
#define ROUTER_H

#include <QtWidgets>
#include "board.h"
#include "home.h"
#include "player.h"
#include "game.h"
#include <iostream>
using namespace std;


// Classe qui hérite de QStackedWidget et permet donc de transiter entre
// les différentes pages de l'application
class Router : public QStackedWidget
{
    Q_OBJECT

public:
    Router();
    ~Router();

    // ------------- GETTERS -------------
    Board& getBoard();


    // ------------- SLOTS -------------
public slots:
    void changeOnglet(QString name);
    void restartGame();
    void pauseGame();

private:
    // ------------- ATTRIBUTES -------------
    Home *home;
    Board *board;
    Player *player1;
    Player *player2;
    Game *game;


    // ------------- METHODS -------------
    void setGameParameters();

    // ------------- FRIENDS -------------
    friend class Board;
    friend class Game;
};


#endif // ROUTER_H
