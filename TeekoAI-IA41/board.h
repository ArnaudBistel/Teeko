#ifndef BOARD_H
#define BOARD_H

#include <QtWidgets>
#include "iostream"
#include "pawn.h"

typedef int (*pointer_to_arrays)[5];

class Board : public QWidget
{
  Q_OBJECT

public:
    Board();
    Board(QWidget *parent,QString name);

    // ------------- GETTERS -------------
    pointer_to_arrays getBoard();
    void printBoard();
    bool isOnlyIA();
    bool isUpdated();

    // ------------- SETTERS -------------
    void setOnlyIA(bool);
    void setUpdated(bool);

    // ------------- SLOTS -------------
public slots:
    void tileChosen();
    void resetGame();
    void goBack();
    void announceWinner(int, QString);
    void placePion(int, int, int, bool, bool, bool);
    void displayPossibleMoves(int,int);
    void unselectPawn(int);
    void prepareBoardForCurrentPlayer(int, int);
    void setBoardLabelEnabled(bool);
    void disableBoard();
    void enableBoard();
    void displayCurrentPlayer(int);

    // ------------- SIGNALS -------------
signals:
    void changeInterface(QString name);
    void playerPlayed(int);
    void beginGame();
    void holdOn();

private:
    // ---------- PAWN & BOARD METHODS ----------

    void deletePossibleMoves();
    void initBoardVisible();
    void initBoardInvisible();
    void reinit();
    void enableAllPawns();

    // ------------- PLAYERS METHODS ------------
    void displayPlayers(QString, QString);
    void displayWinLabel(QString);


    // ------------- ATTRIBUTES -------------
    // représentation matricielle du board
    int  board [5][5];
    // synchronization de thread
    QMutex mutex;
    QWaitCondition sleepSimulator;
    bool onlyIA;
    bool updated;

    // ----------------------------------
    // Fenêtre du jeu
    // ----------------------------------
    QLabel board_label;
//    std::vector<std::vector<Pawn*>>array;

    Pawn* array[5][5];
    QGridLayout but_and_image_layout;


    // ----------------------------------
    // Panneau affichant les joueurs
    // ----------------------------------
    QVBoxLayout panel_layout;
    QVBoxLayout player1_layout;
    QGroupBox player1_groupbox;
    QLabel player1_name_label;
    QLabel player1_type_label;
    QLabel player1_color_label;
    QGroupBox player2_groupbox;
    QVBoxLayout player2_layout;
    QLabel player2_name_label;
    QLabel player2_type_label;
    QLabel player2_color_label;
    QLabel win_label;


    // ----------------------------------
    // retour et recommencer
    // ----------------------------------
    QHBoxLayout bottom_buttons_layout;
    QPushButton back_to_home_button;
    QPushButton reset_button;

    // -------------------------------------
    // vertical  et horizontal layout qui contiennent tous les widgets
    // -------------------------------------
    QVBoxLayout v_layout;
    QHBoxLayout h_layout;


    // ------------- FRIENDS -------------
    friend class Game;
    friend class Router;
//    friend class IAPlayer;
};

#endif // BOARD_H
