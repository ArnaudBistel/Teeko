#include "aiplayer.h"


AIPlayer::AIPlayer(QString name, int id, int difficulty )
    : Player(name, true, id), move(), difficulty(difficulty), pawn_to_move(-1), where_to_move_pawn(-1)
{
}

AIPlayer& AIPlayer::operator=(const AIPlayer& pl)
{
    if ( this == &pl)	return *this;
    this->id = pl.id;
    this->name 	= pl.name;
    this->ia    = pl.ia;
    this->move = pl.move;
    this->difficulty = pl.difficulty;

    return *this;
}


bool AIPlayer::operator== (AIPlayer &rhs)
{
    if (rhs.isIA() == this->isIA() &&
            rhs.getID() == this->getID() &&
            rhs.getName() == this->getName() &&
            rhs.move == this->move &&
            rhs.difficulty == this->difficulty
            )
        return true;
    return false;
}


//Fonction appelée dans game
int AIPlayer::getIAMove(int  board [5][5])
{
    this->findMove(board);
    return this->move;
}


int AIPlayer::getDifficulty()
{
    return this->difficulty;
}

//Fontion principale qui appelle toutes les autres pour trouver le meilleur coup à chaque tour
void AIPlayer::findMove(int  board [5][5])
{
    // AI's pawns
    std::vector< int > pawns;
    int move(-1);

    // récupère la liste des index des pions de l'IA
    for(int i=0; i<5; i++)
    {
        for(int j=0; j<5; j++)
        {
            if (board[i][j] == this->id)
            {
                pawns.push_back(i*5 + j);
            }
        }
    }

    // Difficulté facile
    if (this->difficulty == 1 )
    {
        // si l'IA a déjà placé ses 4 pions et qu'elle n'a pas encore choisi
        // quel pion elle va déplacer
        if (this->pionOnBoard() >= 4 && !this->chosePionToMove()) {
            int MAX = 3, MIN = 0;

            // choisi au hasard un pion à déplacer parmis ses 4 pions
            srand(time(NULL));
            move = (rand() % (MAX - MIN + 1)) + MIN;
            move = pawns[move];

        // si l'IA a déjà placé ses 4 pions et qu'elle a choisi
        // quel pion elle va déplacer
        } else if (this->pionOnBoard() >= 4 && this->chosePionToMove())
        {
            // cherche les déplacements possibles pour le pion qu'elle souhaite déplacer
            // qui se trouve dans previous_index
            pawns = this->computePossibleMoves(this->previous_index, board);
            int MAX = pawns.size() - 1;
            int MIN = 0;
            // choisi au hasard un déplacement parmis les déplacements possibles
            srand(time(NULL));
            move = (rand() % (MAX - MIN + 1)) + MIN;
            move = pawns[move];

        // si l'IA n'a pas encore placé ses 4 pions
        } else if (this->pionOnBoard() < 4) {
            int MAX = 24, MIN = 0;

            // Génération du nombre aléatoire
            srand(time(NULL));
            // choisi une case au hasard sur le plateau
            move = (rand() % (MAX - MIN + 1)) + MIN;

            // si la case est déjà occupée on en cherche une autre
            while (board[(int) move / 5][move % 5] != 0)
            {
                move = (rand() % (MAX - MIN + 1)) + MIN;
            }

//            std::cout << "move : " << move << std::endl;
        }
    }
    //Si difficulté moyenne ou difficile
    else{
        int bestValue, moveValue, bestIndex;
        bestValue = INT32_MIN;
        
        //Si on a déjà placé les 4 pions
        if (this->pionOnBoard()>=4){
            int bestPrevious; //Correspond à la position du pion qu'on déplace pour la fonction setPreviousIndex
            for(int i = 0 ; i<5 ; i++){
                for(int j =0; j < 5 ; j++){
                    //Pour chaque pion de l'IA on cherche les coups possibles
                    if (board[i][j] == this->getID()){
                        //Pour chaque coup possible on crée un nouveau plateau avec ce mouvement et on appelle MinMax pour trouver le meilleur de ces placements
                        for (int index : computePossibleMoves(i*5 + j, board)){
                            int new_board[5][5];
                            this->copyBoard(board,new_board);
                            new_board[i][j] = 0;
                            new_board[index/5][index%5] = this->getID();
                            //On appelle minMax en tant que Minimizer puisque le coup d'après est le coup de l'adversaire, comme on appelle minMax pour la première fois on initialise alpha et beta eu minimum et au maximum
                            moveValue = minMax(new_board,this->getDifficulty(), false,INT32_MIN,INT32_MAX);
                            //Si on trouve un meilleure coup on met à jour bestValue, bestIndex et bestPrevious
                            if(moveValue > bestValue){
                                bestValue = moveValue;
                                bestIndex = index;
                                bestPrevious = i*5 + j;
                            }
                        }
                    }
                }
            }
            //Nécessaire puisque le joueur humain doit d'abord sélectionner un pion avant de jouer mais l'IA fait les deux en même temps
            this->setChosePionToMove(true);
            this->setPreviousIndex(bestPrevious);
        }
        //Si moins de 4 pions posés
        else{
            for(int i = 0 ; i<5 ; i++){
                for(int j =0; j < 5 ; j++){
                    //On teste toutes les cases libres et on évalue avec MinMax comme précedemment
                    if (board[i][j] == 0){
                        int new_board[5][5];
                        this->copyBoard(board,new_board);
                        new_board[i][j] = this->getID();
                        moveValue = minMax(new_board,this->getDifficulty(), false,INT32_MIN,INT32_MAX);

                        if(moveValue > bestValue){
                            bestValue = moveValue;
                            bestIndex = i*5 + j;
                        }
                    }
                }
            }
        }
        move = bestIndex;
    }
    //On renvoie le déplacement à faire
    this->move = move;
}

//Fonction récursive principale de l'IA qui renvoie la valeur max d'un coup en prenant en compte que l'adversaire cherchera à minimiser notre score
//Paramètres : int **board est le plateau de jeu 5*5, int depth représente la profondeur à laquelle on va
//bool is_maximising : true si c'est un coup de l'IA false si c'est un coup de l'adversaire,int alpha et int beta pour l'élagage alpha beta
int AIPlayer::minMax(int (*board)[5], int depth, bool is_maximizing, int alpha, int beta){

    vector <int> ai, player;
    //On récupère la position des pions de chaque joueur
    for(int i = 0 ; i<5 ; i++){
        for (int j = 0 ; j<5 ; j++){
            if(board[i][j] == this->id){
                ai.push_back(i*5  + j);
            }
            else if (board[i][j] != 0){
                player.push_back(i*5+j);
            }
        }
    }
    //Si noeud terminal on renvoie la fonction d'évaluation
    if(depth == 0 || checkWin(ai) == 1 || checkWin(player) == 1){
        return evaluateBoard(board);
    }
    
    //Si c'est un coup de l'IA à tester on cherche à maximiser
    if(is_maximizing){
        int bestValue = INT32_MIN;
        //Si on a pas 4 pions posés on teste toutes les case libres et on récupère la meilleure valeur donnée par minMax
        if(ai.size() < 4){
            for(int i = 0 ; i<5 ; i++){
                for(int j =0; j < 5 ; j++){
                    if (board[i][j] == 0){
                        int new_board[5][5];
                        this->copyBoard(board,new_board);
                        new_board[i][j] = this->getID();
                        //On appelle à nouveau minMax en changeant is_maximizing pour false avec alpha qui peut varier à chaque appel et beta inchangé
                        bestValue = max(bestValue,minMax(new_board,depth-1, false,alpha,beta));
                        //On met à jour beta
                        alpha = max(alpha,bestValue);
                        //Elagage alpha beta
                        if(alpha >= beta)
                            return bestValue;

                    }
                }
            }
        }
        //Si 4 pions déjà posés on cherche le meilleur pion à bouger
        else{
            for(int i = 0 ; i<5 ; i++){
                for(int j =0; j < 5 ; j++){
                    if (board[i][j] == this->getID()){
                        for (int index : computePossibleMoves(i*5 + j, board)){
                            int new_board[5][5];
                            this->copyBoard(board,new_board);
                            new_board[i][j] = 0;
                            new_board[index/5][index%5] = this->getID();
                            //Comme au dessus on appelle minMax avec is_maximizing = false et alpha mis à jour béta inchangé
                            bestValue = max(bestValue,minMax(new_board,depth-1, false,alpha,beta));
                            alpha = max(alpha,bestValue);
                            //Elagage alpha beta
                            if(alpha >= beta)
                                return bestValue;
                        }
                    }
                }
            }
        }
        return bestValue;
    }
    //On est dans le cas de l'adversaire qui va chercher la valeur minimale possible selon le coup précédent de l'IA
    else{
        int bestValue = INT32_MAX;
        //Permet d'obtenir la bonne id puisque id variable
        int playerID = ((this->getID() == 1) ? 2 : 1);
        //Si moins de 4 pions posés par l'adversaire
        if(player.size() < 4){
            for(int i = 0 ; i<5 ; i++){
                for(int j =0; j < 5 ; j++){
                    //On teste chaque case vide et on trouve la valeur donnée par minMax si on pose ce pion 
                    if (board[i][j] == 0){
                        int new_board[5][5];
                        this->copyBoard(board,new_board);
                        new_board[i][j] = playerID;
                        //Cette fois on appelle minMax avec is_maximizing = true puisque le coup d'après est un coup de l'IA
                        //Ici alpha reste fixe par rapport à l'appel initial mais beta est mis à jour
                        bestValue = min(bestValue,minMax(new_board,depth-1, true,alpha,beta));
                        beta = min(beta,bestValue);
                        //Elagage alpha beta
                        if(alpha >= beta)
                            return bestValue;

                    }
                }
            }
        }
        //Comme précedemment pour le maximizer si 4 pions sont déjà posés
        else{
            for(int i = 0 ; i<5 ; i++){
                for(int j =0; j < 5 ; j++){
                    if (board[i][j] == playerID){
                        for (int index : computePossibleMoves(i*5 + j, board)){
                            int new_board[5][5];
                            this->copyBoard(board,new_board);
                            new_board[i][j] = 0;
                            new_board[index/5][index%5] = playerID;
                            bestValue = min(bestValue,minMax(new_board,depth-1, true,alpha,beta));
                            beta = min(beta,bestValue);
                            if(alpha >= beta)
                                return bestValue;
                        }
                    }
                }
            }
        }
        return bestValue;
    }
}

//Fonction d'évaluation qui compare la valeur des pions de l'IA et celle des pions de l'adversaire
int AIPlayer::evaluateBoard(int board[5][5]){
    int value;
    
    vector <int> ai, player;
    //On récupère les positions des pions de l'IA et de son adversaire
    for(int i = 0 ; i<5 ; i++){
        for (int j = 0 ; j<5 ; j++){
            if(board[i][j] == this->id){
                ai.push_back(i*5  + j);
            }
            else if (board[i][j] != 0){
                player.push_back(i*5+j);
            }
        }
    }
    //Selon la difficulté la fonction d'évaluation est plus précise (une fonction en plus et des coefficients différents)
    switch(this->getDifficulty()){
        case 2:
            value = 30*(checkWin(ai) - checkWin(player));
            value += 0.6*(checkPawnAlign(ai,board) - checkPawnAlign(player,board));
            value += checkPawnPotential(ai) - checkPawnPotential(player);
            break;
        case 3:
            value = 30*(checkWin(ai) - checkWin(player));
            value += 0.4*(checkPawnAlign(ai,board) - checkPawnAlign(player,board));
            value += 1.5*(checkPawnDistance(ai) - checkPawnDistance(player));
            value += 0.8*(checkPawnPotential(ai) - checkPawnPotential(player));

    }
    return value;
}

//Fonction qui calcule la distance d'un pion avec le pion précédent et renvoie une valeur plus élevée si les pions sont proches les un des autres
int AIPlayer::checkPawnDistance(vector<int> pawns){
    int value = 0;
        if(pawns.size() > 1){
            for(int i = 1 ; i < (int) pawns.size(); i++){
                value += (int) sqrt(std::pow((pawns[i] / 5) - (pawns[i-1] / 5),2 ) + std::pow((pawns[i] % 5) - (pawns[i-1] % 5),2));
            }
            //Le but est d'avoir une valeur élevée si value est faible
            value = (pawns.size() *5)/value;
        }

        return value;
}

//Fonction qui calcule la moyenne des potentiels de chaque case
int AIPlayer::checkPawnPotential(vector<int> pawns){
   
    vector <int> corner = {0,4,20,24}, sideMid = {2,10,14,22}, sideCorner = {1,3,5,9,15,19,21,23}, middleCorner = {6,7,8,11,13,16,17,18}; //Number of possible combinaisons : corner (4) sideMid (5) sideCorner(6) middleCorner(10) middle (12)

    int value = 0;
    if(pawns.size()>1){
        for(int i = 0 ; i< (int) pawns.size() ; i++){
            //Si le pion est dans un coin il ne peut faire que 4 combinaisons possible
            if (std::find(corner.begin(),corner.end(), pawns[i]) != corner.end()){
                value += 2;
            }
            //Si le pion est au milieu d'un côté il ne peut faire que 5 combinaisons possible
            else if (std::find(sideMid.begin(),sideMid.end(), pawns[i]) != sideMid.end()){
                value += 4;
            }
            //Si le pion est entre le coin et le milieu sur le côté il ne peut faire que 6 combinaisons possible
            else if (std::find(sideCorner.begin(),sideCorner.end(), pawns[i]) != sideCorner.end()){
                value += 7;
            }
            //Si le pion est dans le carré central sans la case centrale il peut faire 10 combinaisons possibles
            else if (std::find(middleCorner.begin(),middleCorner.end(), pawns[i]) != middleCorner.end()){
                value += 11;
            }
            //La case du milieu permet de faire 12 combinaisons possibles
            else{
                value += 16;
            }
        }
        value /= pawns.size();
    }
    return value;
}

//Fonction qui détecte si une séquence de pions peut déboucher sur une combinaison gagnante
//La fonction vérifie pour chaque combinaison gagnante (ligne,colonne,carré,diagonale) et le score et la somme des carrés de la taille de la meilleure séquence pour chaque combinaison
int AIPlayer::checkPawnAlign(vector<int> pawns, int board[5][5]){
    int value = 0;
    if(pawns.size() > 0){
        vector <vector <int>> sub;
        //On appelle cette fonction pour découper notre liste de pions en sous-séquences pour tester chaque sous-séquences
        createSubSequences(sub, pawns);
        int maxSquare = 0, maxLine = 0, maxCol = 0, maxLDiag = 0, maxRDiag = 0;
        for (int i = 0 ; i < (int) sub.size() ; i++){
            //Si la séquence peut devenir un carré ET si les positions pour faire un carré sont libres
            if(isValidSquare(sub[i]) && canMakeSquare(sub[i],board)){
                maxSquare = max(maxSquare,(int) pow(sub[i].size(),2));
            }
            //Si la séquence peut devenir une ligne ET si les positions pour faire une ligne sont libres
            if(isValidLine(sub[i]) && canMakeLine(sub[i],board)){
                maxLine = max(maxLine,(int) pow(sub[i].size(),2));
            }
            //Si la séquence peut devenir une colonne ET si les positions pour faire une colonne sont libres
            if(isValidColumn(sub[i]) && canMakeCol(sub[i],board)){
                maxCol = max(maxCol,(int) pow(sub[i].size(),2));
            }
            //Les deux conditions suivantes vérifient la même chose pour des diagonales mais pour un soucis de clarté on a divisé
            //les possibilités en 2 soit une diagonale descend à gauche soit à droite
            if(isValidDiag(sub[i],true) && canMakeRDiag(sub[i],board)){
                maxRDiag = max(maxRDiag,(int) pow(sub[i].size(),2));
            }
            if(isValidDiag(sub[i], false) && canMakeLDiag(sub[i],board)){
                maxLDiag = max(maxLDiag,(int) pow(sub[i].size(),2));
            }
        }
        value = maxSquare + maxCol + maxLDiag + maxLine + maxRDiag;
    }
    return value;
}

//Fonction qui vérifie si la combinaison peut faire un carré sans prendre en compte les pions adverses
bool AIPlayer::isValidSquare(vector<int> pawns){
    bool valid = false;
    //Peu importe où est placé le pion, tout pion peut faire un carré
    if(pawns.size() == 1){
        valid = true;
    }
    else if (pawns.size() == 2){ //Toute ligne, colonne ou diagonale collée est un carré potentiel
        if (pawns[0] + 1 == pawns[1] && pawns[1] %5 != 0){
            valid = true;
        }
        else if (pawns[0] + 5 == pawns[1]){
            valid = true;
        }
        else if (pawns[0] + 6 == pawns[1] && pawns[1] %5 != 0){
            valid = true;
        }
        else if (pawns[0] + 4 == pawns[1] && pawns[1] % 5 != 4){
            valid = true;
        }
    }
    else if (pawns.size() == 3){ //Un carré potentiel à 3 pions est un L
        if (pawns[0] + 1 == pawns[1] && pawns[1] %5 != 0){
            if(pawns[2] == pawns[0] + 5 || pawns[2] == pawns[0] + 6){
                valid = true;
            }
        }
        else if (pawns[1] + 1 == pawns[2] && pawns[2] %5 != 0){
            if(pawns[0] == pawns[1] - 5 || pawns[0] == pawns[1] - 4){
                valid = true;
            }
        }
    }
    //On vérifie que c'est bien un carré formé
    else{
        if (pawns[0] / 5 != 4 && pawns[0] % 5 != 4 && pawns[1] == pawns[0] + 1 && pawns[2] == pawns[0] + 5 && pawns[3] == pawns[0] + 6){
            valid = true;
        }
    }
    return valid;
}

//Fonction qui vérifie si la combinaison peut faire une colonne sans prendre en compte les pions adverses
bool AIPlayer::isValidColumn(vector<int> pawns){
    bool valid = false;
    if(pawns.size() == 1){
        valid = true;
    }
    else{
        //On vérifie que chaque pion est sur la même colonne et que les pions ne sont pas à 5 cases d'écart
        for(int i = 1 ; i< (int) pawns.size() ; i++){
            if(pawns[i] % 5 == pawns[i-1] % 5 && pawns[pawns.size() -1 ] - pawns[0] < 16 ){
                valid = true;
            }
            else{
                valid = false;
                i=4;
            }
        }
    }
    return valid;
}

//Fonction qui vérifie si la combinaison peut faire une ligne sans prendre en compte les pions adverses
bool AIPlayer::isValidLine(vector<int> pawns){
    bool valid = false;

    if(pawns.size() == 1){
        valid = true;
    }
    else{
        //On vérifie que chaque pion est sur la même ligne et qu'il ne sont pas à 5 cases d'écart
        for(int i = 1 ; i< (int) pawns.size() ; i++){
            if(pawns[i] / 5 == pawns[i-1] / 5 && pawns[pawns.size() -1 ] - pawns[0] < 4 ){
                valid = true;
            }
            else{
                valid = false;
                i=4;
            }
        }
    }
    return valid;
}

//Fonction qui vérifie si la combinaison peut faire une diagonale sans prendre en compte les pions adverses
//Le parametre right permet de dissocier une diagonale à droite ou à gauche
bool AIPlayer::isValidDiag(vector<int> pawns, bool right){

    bool valid = false;
    //Seules certaines cases permettent une diagonale
    vector <int> possible_r = {0,1,5,6,7,11,12,13,17,18,19,23,24}, possible_l = {3,4,7,8,9,11,12,13,15,16,17,20,21};

    if(right){
        //Si le premier pion n'est pas à une case possible c'est faux
        if(std::find(possible_r.begin(),possible_r.end(),pawns[0]) != possible_r.end()){
            if(pawns.size() == 1){
                valid = true;
            }
            //On vérifie que chaque pion est sur la diagonale 
            else{
                for(int i = 1; i<(int)pawns.size();i++){
                    if ((pawns[i] - pawns[i-1]) % 6 == 0 && pawns[i]%5 >= i){
                        valid = true;
                    }
                    else{
                        valid = false;
                        i=4;
                    }
                }
            }
        }

    }
    else{
        //Si le premier pion n'est pas à une case possible c'est faux
        if(std::find(possible_l.begin(),possible_l.end(),pawns[0]) != possible_l.end()){
            if(pawns.size() == 1){
                valid = true;
            }
            else{
                //On vérifie que chaque pion est sur la diagonale 
                for(int i = 1; i<(int)pawns.size();i++){
                    if ((pawns[i] - pawns[i-1]) % 4 == 0 && pawns[i]%5 <= (4-i)){
                        valid = true;
                    }
                    else{
                        valid = false;
                        i = 4;
                    }
                }
            }
        }

    }
    return valid;
}

//Fonction qui vérifie que le carré est bien faisable en prenannt en compte les pions adverses, utilisés avec isValidSquare donc on sait déjà que le carré peut être fait
bool AIPlayer::canMakeSquare(vector<int> pawns, int board[5][5]){
    bool result = false;
    int x,row,col,thisId,otherId;
    x = pawns[0];
    row = x / 5;
    col = x%5;
    thisId = board[row][col];
    otherId = (thisId == 1) ? 2 : 1;
    if(pawns.size() == 1){
        //à partir d'un pion on a 4 carrés potentiels donc on vérifie qu'au moins 1 de ces carrés est possible
        if(row > 0 && col > 0){
            if(board[row-1][col-1] != otherId && board[row-1][col] != otherId && board[row][col-1] != otherId)
                result = true;
        }
        if(row > 0 && col < 4){
            if(board[row-1][col+1] != otherId && board[row-1][col] != otherId && board[row][col+1] != otherId)
                result = true;
        }
        if(row < 4 && col < 4){
            if(board[row+1][col+1] != otherId && board[row+1][col] != otherId && board[row][col+1] != otherId)
                result = true;
        }
        if(row < 4 && col > 0){
            if(board[row+1][col-1] != otherId && board[row+1][col] != otherId && board[row][col-1] != otherId)
                result = true;
        }
    }
    else if (pawns.size() == 2){
        //Avec 2 pions on a 6 carrés possibles 
        //Si les deux pions sont sur la même ligne
        if(pawns[1] == x +1){
            if(row > 0 && board[row-1][col] != otherId && board[row-1][col+1] != otherId)
                result = true;
            if(row < 4 && board[row+1][col] != otherId && board[row+1][col+1] != otherId)
                result = true;
        }
        //Si les deux pions sont en diagonale gauche
        else if(pawns[1] == x +4){
            if(board[row+1][col] != otherId && board[row][col-1] != otherId)
                result = true;
        }
        //Si les deux pions forment une colonne
        else if(pawns[1] == x +5){
            if(col > 0 && board[row][col-1] != otherId && board[row+1][col-1] != otherId)
                result = true;
            if(col < 4 && board[row][col+1] != otherId && board[row+1][col+1] != otherId)
                result = true;
        }
        //Si les deux pions sont en diagonale droite
        else if(pawns[1] == x +6){
            if(board[row+1][col] != otherId && board[row][col+1] != otherId)
                result = true;
        }
    }
    else if (pawns.size() == 3){
        //Si on a 3 pions on a 4 possibilités
        //Si le troisième pion est en colonne avec le premier on a 2 possibilités
        if(pawns[2] == x +5){
            if(pawns[1] == x+1 && board[row+1][col+1] != otherId)
                result = true;
            if(pawns[1] == x+4 && board[row][col-1] != otherId)
                result = true;
        }
        //Si le troisième pion n'est pas en colonne avec le deuxième on a 2 possibilités
        else{
            if(pawns[1] == x+1 && board[row+1][col] != otherId)
                result = true;
            if(pawns[1] == x+5 && board[row][col+1] != otherId)
                result = true;
        }
    }
    //Vraie puisqu'on a déjà vérifié que c'était un carré possible
    else{
        result = true;
    }
    return result;
}

//Fonction qui vérifie que la ligne est bien faisable en prenannt en compte les pions adverses, utilisés avec isValidLine donc on sait déjà que la ligne peut être faite
bool AIPlayer::canMakeLine(vector<int> pawns, int board[5][5]){
    bool result = false;
    int x,row,col,thisId,otherId;
    x = pawns[0];
    row = x / 5;
    col = x%5;
    thisId = board[row][col];
    otherId = (thisId == 1) ? 2 : 1;
    //Les 3 cases centrales de la ligne doivent être libre ou possédée par le joueur 
    if(board[row][1] != otherId && board[row][2] != otherId && board[row][3] != otherId){
        //Si les 3 centrales sont posssibles il faut vérifier les deux sur les bords
        if(board[row][0] != otherId || board[row][4] != otherId)
            result = true;
    }
    return result;
}

//Fonction qui vérifie que la colonne est bien faisable en prenannt en compte les pions adverses, utilisés avec isValidCol donc on sait déjà que la colonne peut être faite
bool AIPlayer::canMakeCol(vector<int> pawns, int board[5][5]){
    bool result = false;
    int x,row,col,thisId,otherId;
    x = pawns[0];
    row = x / 5;
    col = x%5;
    thisId = board[row][col];
    otherId = (thisId == 1) ? 2 : 1;
    //Comme pour la ligne on vérifie que les 3 places centrales de la colonne sont possible
    if(board[1][col] != otherId && board[2][col] != otherId && board[3][col] != otherId){
        //Si les positions centrales sont possible on vérifie les deux aux extrémités
        if(board[0][col] != otherId || board[4][col] != otherId)
            result = true;
    }
    return result;
}

//Fonction qui vérifie que la diagonale droite est bien faisable en prenannt en compte les pions adverses, utilisés avec isValidDiag donc on sait déjà que la diagonale peut être faite
bool AIPlayer::canMakeRDiag(vector<int> pawns, int board[5][5]){
    bool result = false;
    int x,row,col,thisId,otherId;
    x = pawns[0];
    row = x / 5;
    col = x%5;
    thisId = board[row][col];
    otherId = (thisId == 1) ? 2 : 1;
    //Si on est sur la diagonale centrale on a deux possibilités puisqu'il y à 5 cases
    if(row == col){
        //On vérifie les 3 centrales
        if(board[1][1] != otherId && board[2][2] != otherId && board[3][3] != otherId){
            //Puis les extrémités
            if(board[0][0] != otherId || board[4][4] != otherId)
                result = true;
        }
    }
    //Sinon on a deux diagonales possibles
    else{
        //On remonte la diagonale pour ensuite redescendre
        while(row > 0 && col > 0){
            row--;
            col--;
        }
        //On redescend en vérifiant chaque position
        for(int i = 0;i<4;i++){
            if (board[row][col] != otherId){
                result = true;
            }
            else{
                result = false;
                i=4;
            }
            row++;
            col++;
        }
    }
    return result;
}

//Fonction qui vérifie que la diagonale gauche est bien faisable en prenannt en compte les pions adverses, utilisés avec isValidDiag donc on sait déjà que la diagonale peut être faite
//Cette fonction marche exactement comme la fonction ci-dessus 
bool AIPlayer::canMakeLDiag(vector<int> pawns, int board[5][5]){
    bool result = false;
    int x,row,col,thisId,otherId;
    x = pawns[0];
    row = x / 5;
    col = x%5;
    thisId = board[row][col];
    otherId = (thisId == 1) ? 2 : 1;
    if(row + col == 4){
        if(board[1][3] != otherId && board[2][2] != otherId && board[3][1] != otherId){
            if(board[0][4] != otherId || board[4][0] != otherId)
                result = true;
        }
    }
    else{
        while(row > 0 && col <4){
            row--;
            col++;
        }
        for(int i = 0;i<4;i++){
            if (board[row][col] != otherId){
                result = true;
            }
            else{
                result = false;
                i=4;
            }
            row++;
            col--;
        }
    }
    return result;
}


//On vérifie si les 4 pions sont une combinaison gagnante
int AIPlayer::checkWin(vector <int> pawns ){
    int won, lineCount = 1, columnCount = 1, diag_rCount = 1, diag_lCount = 1, squareCount = 1;
    if(pawns.size() < 4){
        won = 0;
    }
    else{
        int pawn = pawns[0];
        for(int i = 1 ; i<4 ; i++ ){
            //On vérifie que ça forme un carré
            if (pawns[i] == pawn + 1 && pawns[2] == pawns[0] + 5 && pawns[0] % 5 < 4 && pawns[0] < 20){
                if(i == 2){
                    pawn = pawns[i];
                }
                else{
                    pawn +=4;
                }
                squareCount++;
            }
            //On vérifie que chaque pion est à droite du précédent
            else if(pawns[i] == pawn +1 && pawns[0]%5 <2){
                pawn = pawns[i];
                lineCount++;
            }
            //On vérifie que chaque pion est sous le précédent
            else if (pawns[i] == pawn +5 && pawns[0] < 10){
                pawn = pawns[i];
                columnCount++;
            }
            //On vérifie que chaque pion est en diagonale à droite du précédent
            else if(pawns[i] == pawn + 6 && pawns[0] % 5 < 2 && pawns[0] <7){
                pawn = pawns[i];
                diag_rCount++;
            }
            //On vérifie que chaque pion est en diagonale à gauche du précédent
            else if (pawns[i] == pawn + 4 && pawns[0] % 5 > 2 && pawns[0] < 10){
                pawn = pawns[i];
                diag_lCount++;
            }

        }
        //Si une combinaison gagante trouvée on a gagné
        if(lineCount == 4 || columnCount == 4 || diag_lCount == 4 || diag_rCount == 4 || squareCount == 4)
            won = 1;
        else
            won = 0;
    }
    return won;
}

// retourne un vecteur contenant les index des déplacements possibles pour le
// pion dont l'index est passé en paramètre
// Donc l'ensemble des cases adjacentes qui ne sont pas déjà occupées
std::vector<int> AIPlayer::computePossibleMoves(int index, int  board [5][5])
{
//    std::cout << "AIPlayer::computePossibleMoves : " << std::endl;
    int line = (int) index / 5;
    int col = index % 5;
    std::vector< int > pawns;

    // évaluation des choix possibles
    int i, j = 0;

    if (line - 1 >= 0 && col - 1 >= 0 )
    {
        i = line - 1;
        j = col - 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    if (line - 1 >= 0)
    {
        i = line - 1;
        j = col;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    if (line - 1 >= 0 && col + 1 <= 4)
    {
        i = line - 1;
        j = col + 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    if (col + 1 <= 4)
    {
        i = line;
        j = col + 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    if (line + 1 <= 4 && col + 1 <= 4 )
    {
        i = line + 1;
        j = col + 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    if (line + 1 <= 4)
    {
        i = line + 1;
        j = col;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    if (line + 1 <= 4 && col - 1 >= 0)
    {
        i = line + 1;
        j = col - 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    if (col - 1 >= 0)
    {
        i = line;
        j = col - 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        }
    }

    return pawns;
}




// retourne un vecteur contenant les index des déplacements possibles pour le
// pion dont l'index est passé en paramètre
// Donc l'ensemble des cases adjacentes qui ne sont pas déjà occupées
std::vector<int> AIPlayer::computeSquaresAroundSquare(int index, int  board [5][5])
{
//    std::cout << "AIPlayer::computeSquaresAroundSquare : " << std::endl;
    int line = (int) index / 5;
    int col = index % 5;
    std::vector< int > pawns;

    // évaluation des choix possibles
    int i, j = 0;

    if (line - 1 >= 0 && col - 1 >= 0 )
    {
        i = line - 1;
        j = col - 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    if (line - 1 >= 0)
    {
        i = line - 1;
        j = col;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    if (line - 1 >= 0 && col + 1 <= 4)
    {
        i = line - 1;
        j = col + 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    if (col + 1 <= 4)
    {
        i = line;
        j = col + 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    if (line + 1 <= 4 && col + 1 <= 4 )
    {
        i = line + 1;
        j = col + 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    if (line + 1 <= 4)
    {
        i = line + 1;
        j = col;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    if (line + 1 <= 4 && col - 1 >= 0)
    {
        i = line + 1;
        j = col - 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    if (col - 1 >= 0)
    {
        i = line;
        j = col - 1;
        if (board[i][j] == 0 )
        {
            pawns.push_back(i*5 + j);
        } else {
            pawns.push_back(-1);
        }
    } else {
        pawns.push_back(-1);
    }

    return pawns;
}

//Découpe un vecteur en sous séquences 
void AIPlayer::createSubSequences(vector<vector<int>> &sub, vector<int> vec){
    int n = vec.size();
        int num = pow(2,n);
        for (int i = 0; i< num ; i++){
            vector <int> tmp;
            for (int j = 0;j<n;j++){
                if (((i>>j) & 1) == 1){
                   tmp.push_back(vec[j]);
                }
            }
            if (tmp.size() != 0)
                sub.push_back(tmp);
        }
}


void AIPlayer::copyBoard(int board [5][5], int new_board [5][5])
{
//    std::cout << "AIPlayer::copyBoard : " << std::endl;
    for(int i=0; i<5; i++)
    {
        for(int j=0; j<5; j++)
        {
            new_board[i][j] = board[i][j];
        }
    }
}

void AIPlayer::printBoard(int b [5][5])
{
    for(int i=0; i<5; i++)
    {
            for(int j=0; j<5; j++)
            {
                    std::cout<<" "<<b[i][j]<<" ";
            }
            std::cout<<"\n";
    }

}
