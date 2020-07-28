#ifndef HOME_H
#define HOME_H

#include <QtWidgets>
#include <iostream>
#include "player.h"

// classe de la page d'accueil du jeu qui permet de configurer le jeu
// Le mode : IA vs IA, IA vs Humain, Humain vs Humain
// Ainsi que la difficulté si une IA est impliquée
class Home : public QWidget
{
    Q_OBJECT

public:
    Home(QWidget *parent,QString name);
    ~Home();


    // ------------- SLOTS -------------
public slots:
    void launch_game();
    void enableDifficultyChoice();
    void disableDifficultyChoice();

    bool isIAVsIA();
    bool isHumanVsIA();
    bool isHumanVsHuman();

    bool isEasy();
    bool isMedium();
    bool isHard();
    // ------------- SIGNALS -------------
signals:
    void changeInterface(QString name);


private:

    // ------------- ATTRIBUTES -------------

    // -------------------------------------
    // top layout, contient le titre de l'app
    // -------------------------------------
    QLabel title_label;
    QHBoxLayout title_layout;


    // -------------------------------------
    // Box de congfiguration du mode de jeu
    // -------------------------------------
    QGroupBox configuration_box;
    QFormLayout configuration_form;
    QRadioButton button_ia_vs_ia;
    QRadioButton button_human_vs_ia;
    QRadioButton button_human_vs_human;


    // -------------------------------------
    // Box de configuration de la difficulté
    // -------------------------------------
    QGroupBox difficulty_box;
    QFormLayout difficulty_form;
    QRadioButton button_easy;
    QRadioButton button_medium;
    QRadioButton button_hard;


    // -------------------------------------
    // quitter / commencer / recommencer
    // -------------------------------------
    QHBoxLayout buttons_layout;
    QPushButton quit_button;
    QPushButton start_button;
    QPushButton restart_button;


    // -------------------------------------
    // vertical et horizontal layouts qui contiennent tous les widgets de Home
    // -------------------------------------
    QVBoxLayout vertical_layout;
    QVBoxLayout params_layout;
//    QHBoxLayout horizontal_layout;

};

#endif // HOME_H
