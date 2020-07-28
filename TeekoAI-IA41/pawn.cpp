#include "pawn.h"
#include <iostream>

// Les pions sont en fait des QPushButton dont la couleur évolue en focntion
// de si ils sont au joueur 1 (bleu)
// au joueur 2 (rouge)
// ou non sélectionné (blanc)


// -----------------------------------
// ---------- CONSTRUCTORS -----------
// -----------------------------------
Pawn::Pawn () : QPushButton(), index(), color(), selected(), player(), selectable()
{
    this->initStyle();
}

Pawn::Pawn(const QString &text, QWidget *parent): QPushButton(text, parent), index(), color(), selected(), player(), selectable()
{
    this->initStyle();
}

// constructor de copie
Pawn::Pawn(Pawn const & p)
{
    this->index = p.index;
    this->color = p.color;
    this->selected = p .selected;
    this->player = p.player;
    this->selectable = p.selectable;
    this->initStyle();
}


// -----------------------------------
// ---------- OPERATORS -----------
// -----------------------------------
Pawn& Pawn::operator= (Pawn const & p)
{
    this->index = p.index;
    this->color = p.color;
    this->selected = p .selected;
    this->player = p.player;
    this->selectable = p.selectable;
    this->initStyle();
    return *this;
}


// méthode d'initialisation du style d'un bouton
// blanc (donc représente une case vide)
// forme arrondie
void Pawn::initStyle()
{
    // de base le pion est blanc (transparent)
    QString str = "background-color: white;";
    str+= "border-radius:50px;";
    str+="max-width:100px;";
    str+="max-height:100px;";
    str+="min-width:100px;";
    str+="min-height:100px;";
    this->setStyleSheet(str);
}


// -----------------------------------
// ------------- SETTERS -------------
// -----------------------------------

// modifie la vue du pion si il est sélecitonné ou non
// Le pion "monte" et son ombre devient diffuse quand il est sélectionné
void Pawn::isSelected(bool b, bool reset)
{
    this->selected = b;

    // sélectionné
    if (b)
    {
        // il monte
        this->move(this->x(), this->y() - 10);

        // son ombre devient plus diffuse et étendue
        this->setShadow(10, 10);

    // il est déselectionné
    } else {

        // il redevient blanc (car déplacé)
        QString str = "background-color: white;";
        str+= "border-radius:50px;";
        str+="max-width:100px;";
        str+="max-height:100px;";
        str+="min-width:100px;";
        str+="min-height:100px;";
        this->setStyleSheet(str);

        // il redescend sur le plateau
        if (!reset)
        {
            this->move(this->x(), this->y() + 10);
        }

        // son ombre redevient courte
        this->setShadow(0, 0);
    }
}


// Bouton sélectionnable dans le cas d'un déplacement de pion
// Les boutons sélectionnable sont les boutons où un pion qui
// est sélectionné peut être déplacé
// On représente cet état par un cercle de pointillés
void Pawn::setSelectable(bool b)
{
    // est sélectionnable
    // on l'entoure de dot
    if (b)
    {
        this->selectable = true;
        QString str = "background-color: white;";
        str += "border-style: dotted;";
        str+= "border-width:3px;";
        str+= "border-radius:50px;";
        str += "border-color: black;";
        str+="max-width:100px;";
        str+="max-height:100px;";
        str+="min-width:100px;";
        str+="min-height:100px;";
        this->setStyleSheet(str);

    // n'est pas sélectionnable
    // il reste blanc
    }else{
        this->selectable = false;
        QString str = "background-color: white;";
        str+= "border-radius:50px;";
        str+="max-width:100px;";
        str+="max-height:100px;";
        str+="min-width:100px;";
        str+="min-height:100px;";
        this->setStyleSheet(str);
    }
}


// modifie la couleur selon qu'il a été choisi par un joueur ou bien n'appartient à personne
// joueur 1 = bleu
// joueur 2 = rouge
// pas de joueur = blanc
void Pawn::setColor(int c)
{
    // n'appartient à personne (blanc)
    if (c == 0)
    {
        QString str = "background-color: white;";
        str+= "border-radius:50px;";
        str+="max-width:100px;";
        str+="max-height:100px;";
        str+="min-width:100px;";
        str+="min-height:100px;";
        this->setStyleSheet(str);
    }

    // appartient au joueur 1 (bleu)
    else if (c == 1)
    {
        QString str = "background-color: blue;";
        str += "border-style: solid;";
        str+= "border-width:1px;";
        str+= "border-radius:50px;";
        str += "border-color: grey;";
        str+="max-width:100px;";
        str+="max-height:100px;";
        str+="min-width:100px;";
        str+="min-height:100px;";
        this->setStyleSheet(str);

        // on active son ombre car il est visible
        this->setShadow(3, 3);
    }

    // appartient au joueur 2 (rouge)
    else if (c == 2)
    {
        QString str = "background-color: red;";
        str += "border-style: solid;";
        str+= "border-width:1px;";
        str+= "border-radius:50px;";
        str += "border-color: grey;";
        str+="max-width:100px;";
        str+="max-height:100px;";
        str+="min-width:100px;";
        str+="min-height:100px;";
        this->setStyleSheet(str);

        // on active son ombre car il est visible
        this->setShadow(3, 3);
    }
}



void Pawn::setPlayer(int p)
{
    this->player = p;
    this->setColor(p);
}


void Pawn::setIndex(int i)
{
    this->index = i;
}


// définit l'apparence de l'ombre du pion
void Pawn::setShadow(int a, int b)
{
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(a);
    effect->setOffset(b,b);
    this->setGraphicsEffect(effect);
}



// ------------------------------------
// ------------- GETTERS --------------
// ------------------------------------

int Pawn::getIndex()
{
    return this->index;
}


bool Pawn::isSelectable()
{
    return this->selectable;
}

bool Pawn::getSelected()
{
    return this->selected;
}
