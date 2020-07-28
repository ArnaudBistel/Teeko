#include "Router.h"


// Classe qui hérite de QStackedWidget et permet donc de transiter entre
// les différentes pages de l'application

// CONSTRUCTOR
Router::Router()
{
    game = new Game(this);

    home = new Home(this, "home");
    this->addWidget(static_cast<QWidget*>(home));

    board = new Board(this, "board");
    this->addWidget(static_cast<QWidget*>(board));

    // home est la page d'accueil de l'app
    this->setCurrentWidget(home);
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
}


// recommencer une partie après appuie d'un des boutons Recommencer (de l'accueil ou de
// la page de jeu
void Router::restartGame()
{
    // stop thread du jeu
    game->terminate();

    while(!game->isFinished()){}

    if(!game->isRunning())
    {
        // réinitialise le jeu
        board->reinit();
        game->restartGame();
        this->setGameParameters();

        // affiche la page de jeu
        this->setCurrentWidget(board);

        // démarre le thread du jeu
        game->start();
    }
}


// On met en pause l'IA quand le joueur retourne à la page d'accueil
void Router::pauseGame()
{
    this->game->setPause(true);
}


//Controlleur : slot pour changer de page
// Méthode de routage
void Router::changeOnglet(QString name)
{
    // Vers Home, page d'accueil
    if((name == home->objectName()) && home)
    {
        this->setCurrentWidget(home);
        return;
    }

    // Vers le Board, page du jeu
    else if((name == board->objectName()) && board)
    {
        // si le jeu était on pause on enlève la pause
        if (game->isPaused())
        {
            game->setPause(false);
        } else
        {
            this->setGameParameters();

            // lance le thread de jeu
            game->start();
        }
        // affiche la page de jeu
        this->setCurrentWidget(board);
        return;
    }

    else
    {
        QMessageBox::information(0,tr("Erreur changement onglet"),tr("Impossible de changer l'interface pour l'onglet ")+name+tr(".\nCet onglet n'existe plus ou n'a pas été créer."));
    }
}


// Définie les paramètres du jeu suivant les choix fait dans la page d'accueil
void Router::setGameParameters()
{
    // on récupère les paramètres de configuration et les transmets à
    // Game et au board
    int mode = 0;

    // IA vs IA
    if (home->isIAVsIA())
    {
        mode = 1;
        board->displayPlayers("IA", "IA");
        board->setOnlyIA(true);
    }

    // Humain vs IA
    else if (home->isHumanVsIA())
    {
        mode = 2;
        board->displayPlayers("HUMAN", "IA");
        board->setOnlyIA(false);
    }

    // Humain vs Humain
    else if (home->isHumanVsHuman())
    {
        mode = 3;
        board->displayPlayers("HUMAN", "HUMAN");
        board->setOnlyIA(false);
    }

    // difficulté de l'ia
    int difficulty = 0;
    if (home->isEasy())
    {
        difficulty = 1;
    }
    else if (home->isMedium())
    {
        difficulty = 2;
    }

    else if (home->isHard())
    {
        difficulty = 3;
    }

    game->setMode(mode, difficulty);

}




// -----------------------------------
// ------------- GETTERS -------------
// -----------------------------------

Board& Router::getBoard()
{
    return *board;
}


Router::~Router()
{}
