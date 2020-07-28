#ifndef PAWN_H
#define PAWN_H

#include <QtWidgets>
#include "iostream"

class Pawn : public QPushButton
{
    Q_OBJECT

public:
    Pawn();
    Pawn(Pawn const &);
    Pawn & operator = (Pawn const &);
    Pawn(const QString &text, QWidget *parent);

    // ------------- GETTERS -------------
    int getIndex();
    bool isSelectable();
    bool getSelected();

    // ------------- SLOTS -------------
public slots:

    // ------------- SETTERS -------------
    void isSelected(bool, bool);
    void setSelectable(bool);
    void setColor(int);
    void setPlayer(int);
    void setIndex(int);

private:

    // ------------- ATTRIBUTES -------------
    int index;
    int color;
    bool selected;
    int player;
    bool selectable;

    // ------------- METHODS -------------
    void setShadow(int, int);
    void initStyle();

};


#endif // PAWN_H
