#include "home.h"
#include "Router.h"

// Page d'accueil de l'app qui permet de paramétrer le jeu
// IA contre IA, Humain contre IA, Humain contre Humain
// Difficulté de l'IA si elle est activée


// -----------------------------------
// ---------- CONSTRUCTOR -----------
// -----------------------------------
// met en place tous les éléments graphique qui constituent la page
Home::Home(QWidget *parent, QString name):
    QWidget(parent),

    // ATTRIBUTS

    // top layout, contient le titre de l'app
    title_label("Teeko"),
    title_layout(),

    // Box de congfiguration du mode de jeu
    configuration_box(tr("Mode de jeu"), this),
    configuration_form(),
    button_ia_vs_ia(this),
    button_human_vs_ia(this),
    button_human_vs_human(this),

    // Box de configuration de la difficulté
    difficulty_box(tr("Force de l'IA"), this),
    difficulty_form(),
    button_easy(this),
    button_medium(this),
    button_hard(this),

    // quitter / commencer / recommencer
    buttons_layout(),
    quit_button("Quitter"),
    start_button("Démarrer le jeu"),
    restart_button("Recommencer"),

    vertical_layout(),
    params_layout()
{

    // nom de la page pour switcher entre les pages via le Routeur
    this->setObjectName(name);

    // méthode qui permet la transition entre les différentes pages de l'app
    connect(this, SIGNAL(changeInterface(QString)), this->parent(),SLOT(changeOnglet(QString)));

    // obtenir la taille de l'écran de l'utilisateur
    QRect window = QGuiApplication::screens().first()->geometry();
    int screen_width = window.width();
    int screen_height = window.height();

    int window_width = parent->width();
    int window_height = parent->height();


    // -------------------------------------
    // top layout, contient le titre de l'app
    // -------------------------------------
    title_label.setFont(QFont("Roboto", 20, QFont::Bold, false));
    title_label.setAlignment(Qt::AlignCenter);
    title_layout.addWidget(&title_label);



    // -------------------------------------
    // Box de congfiguration du mode de jeu
    // -------------------------------------
    configuration_form.setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    connect(&button_ia_vs_ia, SIGNAL(clicked()), this, SLOT(enableDifficultyChoice()));
    // Humain contre IA est préselectionné
    button_human_vs_ia.setChecked(true);
    connect(&button_human_vs_ia, SIGNAL(clicked()), this, SLOT(enableDifficultyChoice()));
    connect(&button_human_vs_human, SIGNAL(clicked()), this, SLOT(disableDifficultyChoice()));

    configuration_form.addRow("IA vs IA\t\t", &button_ia_vs_ia);
    configuration_form.addRow("Humain vs IA\t\t", &button_human_vs_ia);
    configuration_form.addRow("Humain vs Humain\t\t", &button_human_vs_human);

    configuration_box.setLayout(&configuration_form);
    configuration_box.setMaximumSize(QSize((int) window_width , (int) window_height * 0.33));


    // -------------------------------------
    // Box de configuration de la difficulté
    // -------------------------------------
    //configuration_form->setVerticalSpacing(15);
    difficulty_form.setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    button_medium.setChecked(true);
    difficulty_form.addRow("Facile\t\t", &button_easy);
    difficulty_form.addRow("Moyen\t\t", &button_medium);
    difficulty_form.addRow("Difficile\t\t", &button_hard);

    difficulty_box.setLayout(&difficulty_form);
    difficulty_box.setMaximumSize(QSize((int) window_width , (int) window_height * 0.33));


    // -------------------------------------
    // layout qui contient les deux box de configuration
    // -------------------------------------
    params_layout.addWidget(&configuration_box);
    params_layout.insertSpacing(1, screen_height * 1/7);
    params_layout.addWidget(&difficulty_box);
    params_layout.setAlignment(Qt::AlignHCenter);


    // -------------------------------------
    // quitter / commencer / recommencer
    // -------------------------------------
    connect(&quit_button, SIGNAL(clicked()), qApp, SLOT(quit()));
    buttons_layout.addWidget(&quit_button);
    buttons_layout.insertSpacing(1, screen_width / 4);
    restart_button.setVisible(false);

    connect(&restart_button, SIGNAL(clicked()), this->parent(), SLOT(restartGame()));
    buttons_layout.addWidget(&restart_button);

    connect(&start_button, SIGNAL(clicked()), this, SLOT(launch_game()));
    buttons_layout.addWidget(&start_button);


    // -------------------------------------
    // vertical et horizontal layouts qui contiennent tous les widgets de Home
    // -------------------------------------
    vertical_layout.addLayout(&title_layout);
    //vertical_layout->insertSpacing(1, screen_height * 1/7);
    vertical_layout.addLayout(&params_layout);
    vertical_layout.insertSpacing(3, screen_height * 1/7);
    vertical_layout.addLayout(&buttons_layout);

    this->setLayout(&vertical_layout);
}



// envoie l'utilisateur sur la page du jeu
void Home::launch_game()
{
    emit changeInterface("board");
    // les boutons de la page d'accueil évolue une fois le jeu lancé une première fois
    this->start_button.setText("Continuer");
    this->restart_button.setVisible(true);
}



// ------------------------------------
// ------------- SETTERS --------------
// ------------------------------------

// si une IA est choisie dans le mode de jeu
// on peut modifier sa force
void Home::enableDifficultyChoice()
{
    this->button_easy.setEnabled(true);
    this->button_medium.setEnabled(true);
    this->button_hard.setEnabled(true);
}


// si aucune IA n'est choisie dans le mode de jeu
// on ne peut pas modifier sa force
void Home::disableDifficultyChoice()
{
    this->button_easy.setEnabled(false);
    this->button_medium.setEnabled(false);
    this->button_hard.setEnabled(false);
}



// ------------------------------------
// ------------- GETTERS --------------
// ------------------------------------

bool Home::isIAVsIA()
{
    return this->button_ia_vs_ia.isChecked();
}


bool Home::isHumanVsIA()
{
    return this->button_human_vs_ia.isChecked();
}


bool Home::isHumanVsHuman()
{
    return this->button_human_vs_human.isChecked();
}

bool Home::isEasy()
{
    return this->button_easy.isChecked();
}


bool Home::isMedium()
{
    return this->button_medium.isChecked();
}


bool Home::isHard()
{
    return this->button_hard.isChecked();
}


Home::~Home()
{
}

