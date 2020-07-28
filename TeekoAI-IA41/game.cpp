#include "game.h"
#include "Router.h"

// Classe qui contient la logique du jeu.

// CONSTRUCTOR
Game::Game(QWidget *parent) : player1(), player2(), current_player(), win(), board(), parent(parent), pause()
{
}


// -------------------------------------------
// -------------- GAME METHODS ---------------
// -------------------------------------------

// boucle du jeu
void Game::run()
{
    // choisi aléatoirement qui est le premier joueur
    this->chooseFirstPlayer();

    while ( !this->win )
    {
        // attend un choix de move du joueur courant
        while (!current_player->hasPlayed())
        {
            if (current_player->isIA())
            {
                QThread::msleep(200);
                int move = this->current_player->getIAMove(this->board);
                emit playerPlayed(move);
            }
        }


        // joueur courant a choisi son action
        if (current_player->hasPlayed())
        {

            // imprime le board dans la console
//            this->printBoard();

            // on vérifie si le dernier déplacement a permis une victoire
            this->win = this->checkIfWins(current_player->getID());

            // victoire !
            if (this->win)
            {
                // affiche le vainqueur
                emit disableBoard();
                emit theresAWinner(current_player->getID(), current_player->getName());
            }

            // pas de victoire
            else {
                // nouveau joueur courant
                this->changeCurrentPlayer();
                // prepare la vue pour le nouveau joueur courant
                this->prepareBoardForNextTurn();
            }
        }

        // le thread dort pendant que le jeu est en pause (sur la page d'accueil)
        while (this->isPaused())
        {
            QThread::msleep(1000);
        }
        QThread::msleep(500);
    }
}


// Reset le jeu pour une nouvelle partie.
// Méthode lancée en cas d'appuie sur Recommencer
void Game::restartGame()
{
    // reset des attributs de Game
    this->win = false;
    this->pause = false;
    for(int i=0; i<5; i++)
    {
        for(int j=0; j<5; j++)
        {
            this->board[i][j]=0;
        }
    }

    // reset les attributs des joueurs
    player1->setPlayed(false);
    player1->setChosePionToMove(false);
    player1->setPionOnBoard(0);
    player1->setPreviousIndex(-1);

    player2->setPlayed(false);
    player2->setChosePionToMove(false);
    player2->setPionOnBoard(0);
    player2->setPreviousIndex(-1);
}



// Appelée par Board lorsque le joueur courant a appuyé sur un pion ou placé un pion.
void Game::playerPlayed(int index)
{
    int line = (int) index / 5;
    int col = index % 5;

    // moins de 4 pions joués donc le joueur place son jeton
    // il va où il veut
    if (current_player->pionOnBoard() < 4)
    {
        // sauvegarde l'état du plateau
        this->board[line][col] = this->current_player->getID();

        // mise à jour de la vue
        if (!this->current_player->hasPlayed())
        {
            emit updateView(current_player->getID(), line, col, false, false, false);
            this->current_player->setPlayed(true);
            this->current_player->incrementPionOnBoard();
        }

    // quand tous les joueurs ont joué leur 4 pions, ils doivent déplacer un pion par tour
    } else {

        // si le joueur n'a pas encore sélectionné de pion à déplacer
        // seulement utilisé quand le joueur courant n'est pas une IA
        if (!current_player->chosePionToMove())
        {
            this->current_player->setChosePionToMove(true);

            this->board[line][col] = 0;

            // sauvegarde l'ancienne position du pion
            this->current_player->setPreviousIndex(index);

            // mise à jour de la vue
            emit updateView(0, line, col, true, true, false);

            emit displayPossibleMoves(line, col);

        // si le joueur a déjà choisi son pion à déplacer
        } else {            
            // cas où joueur déselectionne son pion pour en choisir un autre
            if (index == this->current_player->getPreviousIndex())
            {
                this->current_player->setChosePionToMove(false);
                emit updateView(current_player->getID(), line, col, false, true, true);

                this->board[line][col] = current_player->getID();
                this->prepareBoardForNextTurn();

            // cas où il déplace son pion
            } else {
                if (this->current_player->isIA())
                {
                    int previous_line = (int) this->current_player->getPreviousIndex() / 5;
                    int previous_col = this->current_player->getPreviousIndex() % 5;

                    // enlève le pion de sa place précédente
                    this->board[previous_line][previous_col] = 0;
                    // mise à jour de la vue
                    emit updateView(0, previous_line, previous_col, true, true, false);
                    emit displayPossibleMoves(previous_line, previous_col);
                }
                this->board[line][col] = this->current_player->getID();
                emit updateView(current_player->getID(), line, col, false, false, false);
                this->current_player->setPlayed(true);
                emit unselectPawn(current_player->getPreviousIndex());
            }
        }
    }

//    static_cast<Router*>(this->parent)->getBoard().repaint();
}



// Demande au board de préparer la vue pour le joueur transmis en param.
void Game::prepareBoardForNextTurn()
{
    static_cast<Router*>(this->parent)->getBoard().setUpdated(false);
    emit prepareBoardForNextTurn(current_player->getID(), current_player->pionOnBoard());
    QThread::msleep(200);
}






// -------------------------------------------
// -------------- BOARD METHODS --------------
// -------------------------------------------
void Game::printBoard()
{
    for(int i=0; i<5; i++)
    {
            for(int j=0; j<5; j++)
            {
                    std::cout<<" "<<board[i][j]<<" ";
            }
            std::cout<<"\n";
    }
    std::cout<<"\n";
}



// Méthode qui regroupe les méthodes de vérification de victoire.
bool Game::checkIfWins(int id)
{
    bool win;
    win = this->checkRows(id);
    if (win)
        return win;
    win = this->checkCol(id);
    if (win)
        return win;
    win = this->checkSquare(id);
    if (win)
        return win;
    win = this->checkDiag(id);
    if (win)
        return win;

    return false;
}


// Cherche un alignement de 4 pions en LIGNE
bool Game::checkRows(int id)
{
    for (int j = 0 ; j < 2; j++ )
    {
        for (int i = 0 ; i < 5; i++ )
        {
            if(board[i][j]     ==  id &&
               board[i][j+1]   ==  id &&
               board[i][j+2]   ==  id &&
               board[i][j+3]   ==  id)
            {
                return true;
            }
        }
    }
    return false;
}


// Cherche un alignement de 4 pions en COLONNE
bool Game::checkCol(int id)
{
    for (int i = 0 ; i < 2; i++ )
    {
        for (int j = 0 ; j < 5; j++ )
        {
            if(board[i][j]       ==  id &&
               board[i+1][j]   ==  id &&
               board[i+2][j]   ==  id &&
               board[i+3][j]   ==  id)
            {
                return true;
            }
        }
    }
    return false;
}


// Cherche une formation de 4 pions en CARRE
bool Game::checkSquare(int id)
{
    for(int i=0;i<4;i++)
    {

        for(int j=0;j<4;j++)
        {
            if(board[i][j]       ==  id &&
               board[i][j+1]     ==  id &&
               board[i+1][j]     ==  id &&
               board[i+1][j+1]   ==  id)
            {
                return true;
            }
        }
    }
    return false;
}


// cherche un alignement de 4 pions en diagonale
bool Game::checkDiag(int id)
{
    for (int i = 0 ; i < 2; i++ )
    {
        for (int j = 0 ; j < 2; j++ )
        {
            if(board[i][j]       ==  id &&
               board[i+1][j+1]   ==  id &&
               board[i+2][j+2]   ==  id &&
               board[i+3][j+3]   ==  id)
            {
                return true;
            }
        }
    }

    for (int i = 0 ; i < 2; i++ )
    {
        for (int j = 3 ; j < 5; j++ )
        {
            if(board[i][j]       ==  id &&
               board[i+1][j-1]   ==  id &&
               board[i+2][j-2]   ==  id &&
               board[i+3][j-3]   ==  id)
            {
                return true;
            }
        }
    }

    return false;
}




// -------------------------------------------
// ------------- PLAYERS METHODS -------------
// -------------------------------------------

// Choix aléatoire du premier joueur à jouer.
void Game::chooseFirstPlayer()
{
    int MAX = 100, MIN = 1;

    // Génération du nombre aléatoire
    srand(time(NULL));
    int random = (rand() % (MAX - MIN + 1)) + MIN;

    // Joueur 1 commence
    if (random > 50 )
    {
        current_player = player1;
        emit displayCurrentPlayer(1);

    // Joueur 2 commence
    } else {
        current_player = player2;
        emit displayCurrentPlayer(2);
    }

    if (!current_player->isIA())
    {
        emit enableBoard();
    } else {
        emit disableBoard();
    }
}


// Switche le joueur courant.
void Game::changeCurrentPlayer()
{
    if (current_player == player1)
    {
        player1->setPlayed(false);
        player1->setChosePionToMove(false);
        current_player = player2;
        emit displayCurrentPlayer(2);
    }else
    {
        player2->setPlayed(false);
        player2->setChosePionToMove(false);
        current_player = player1;
        emit displayCurrentPlayer(1);
    }
    if (!current_player->isIA())
    {
        emit enableBoard();
    } else {
        emit disableBoard();
    }
}



// -------------------------------------------
// ----------------- SETTERS -----------------
// -------------------------------------------

void Game::setPlayer1(Player pl)
{
    this->player1 = &pl;
    this->player1->isIA();
}

void Game::setPlayer2(Player pl)
{
    this->player2 = &pl;
    this->player2->isIA();
}


// Gestion de l'IA
// Crée les IA si nécessaire en fonction des choix efefctués sur la page d'accueil
void Game::setMode(int mode, int diff)
{
    if (mode == 1)
    {
        player1 = new AIPlayer("Joueur 1", 1, diff);
        player2 = new AIPlayer("Joueur 2", 2, diff);
    } else if (mode == 2) {
        player1 = new Player("Joueur 1", false, 1);
        player2 = new AIPlayer("Joueur 2", 2, diff);
    } else if (mode == 3) {
        player1 = new Player("Joueur 1", false, 1);
        player2 = new Player("Joueur 2", false, 2);
    }
}


// Jeu mis en pause  par retour à la page d'accueil
void Game::setPause(bool b)
{
    this->pause = b;
    if (current_player != NULL)
    {
        if (this->current_player->isIA()){
            emit enableBoard();
        } else {
            emit disableBoard();
        }
    }
}




// -------------------------------------------
// ----------------- GETTERS -----------------
// -------------------------------------------

pointer_to_arrays Game::getBoard()
{
    return this->board;
}


bool Game::isPaused()
{
    return this->pause;
}
