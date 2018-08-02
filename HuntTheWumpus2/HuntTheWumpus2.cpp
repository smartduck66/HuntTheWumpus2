//
// Book : chapitre 19 de l'ouvrage
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 28/07/2018 - Exos 14&15 page 710 - Hunt The Wumpus "Graphique"
// Commit en cours : 02/08/2018 
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @
// NOTE : on a tenté de conserver la structure de la version 1 autant que faire se peut. HuntTheWumpus.cpp contient les fonctions Helper du jeu initial.

#include <iostream>
#include <thread>
#include "Graph.h"			// get access to our graphics library facilities
#include "GUI.h"
#include "Window.h"
#include "MyHeader.h"


using namespace std;
using namespace Graph_lib;
using namespace HuntTheWumpus_lib;


struct Figures_window : Window {
	Figures_window(Point xy, int w, int h, const string& title);

	// Différence principale par rapport à la version I du jeu : on stocke les données principales dans la structure
	multimap<int, int> maze{};
	int nb_rooms = 20;
	int nb_pits = 3;
	int nb_bats = 3;
	int wumpus_room = 0;
	vector <int> pit_rooms{};
	vector <int> bat_rooms{};
	bool player_room_full = true;
	int player_room = 0;
	bool wumpus_alive = true;
	bool player_alive = true;
	int nb_arrows = 5;
	vector <int> tunnels_dispo{};
	// --------------------------------------------------------
			
	void msg_display(const string& s) { msg_game.put(s); }
	void arrows_display(int a) { arrows.put(a); }
	void game_over();
	void player_position_display();

private:
	Button quit_button;
	Button action_button;
	In_box saisie_action;
	Out_box msg_game;
	Out_box arrows;
	Out_box player_position;

	// actions invoked by callbacks
	void quit();
	void action();

	// callbacks functions
	static void cb_quit(Address, Address);
	static void cb_action(Address, Address);

};

//------------------------------------------------------------------------------

Figures_window::Figures_window(Point xy, int w, int h, const string& title)		// Constructeur
	:Window(xy, w, h, title),
	quit_button(Point(x_max() - 70, 0), 70, 20, "Quit", cb_quit),
	action_button(Point(1020, y_max() - 20), 70, 20, "GO !", cb_action),
	saisie_action(Point(970, y_max()-20), 50, 20, "Action :"),
	msg_game(Point(100, y_max() - 40), 800, 20, "Message :"),
	arrows(Point(970, y_max() - 40), 20, 20, "Flèches :"),
	player_position(Point(100, y_max() - 20), 600, 20, "Position :")
		

{
	
	attach(quit_button);
	attach(action_button);
	attach(saisie_action);
	attach(msg_game);
	attach(arrows);
	attach(player_position);
}

//------------------------------------------------------------------------------

void Figures_window::cb_quit(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).quit();
}


void Figures_window::cb_action(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).action();
}
//------------------------------------------------------------------------------

void Figures_window::quit()
{
	hide();			// curious FLTK idiom for delete window
}

void Figures_window::player_position_display()    // Affichage de la position du joueur et des tunnels disponibles
{
		
	// Remise à zéro systématique des tunnels disponibles quand on arrive dans une nouvelle salle
	tunnels_dispo.erase(tunnels_dispo.begin(), tunnels_dispo.end());
	
	// Recherche des tunnels disponibles
	tunnels_in_room(player_room, tunnels_dispo, maze);
	
	string position = "Tu es en salle " + to_string(player_room);
	position += " dotée des tunnels suivants : [ ";
	for (auto x : tunnels_dispo) position += to_string(x) + ' ';
	position += "]. Move (mx), Shoot (s) or Quit (q) ?";
	
	player_position.put(position);
		
}

void Figures_window::game_over()    // Le jeu est terminé
{

	detach(action_button);
	detach(saisie_action);
	detach(arrows);
	detach(player_position);

}

void Figures_window::action()
{
	// **********************************************************************************************************************************
	// "Boucle de jeu"
	
	// Position du joueur et tunnels disponibles
	msg_display("");
	player_position_display();

	// Saisie de l'option Move (mx), Shoot (s) or Quit (q)
	string option = saisie_action.get_string();
		
	string choix = option.substr(0, 1);
	if (choix == "q") {
		msg_display("Bye");
		player_alive = false;
		game_over();

	}

	else if (choix == "m") {
		// Si le numéro du tunnel choisi est valide, on transporte le joueur dans la nouvelle salle
		// Son "état de vie" peut y être affecté en fonction de la présence du wumpus ou d'une fosse ; sinon il est transféré dans la nouvelle salle (player_room updaté)
		int new_room = stoi(option.substr(1, option.size() - 1));
		if (is_new_room_valid(new_room, tunnels_dispo)) 
		{
			//player_alive = hazard(new_room, wumpus_room, pit_rooms, bat_rooms, player_room, nb_rooms);
			auto live = hazard(new_room, wumpus_room, pit_rooms, bat_rooms, player_room, nb_rooms);
			player_alive = get<0>(live);
			msg_display(get<1>(live));

			// Si le joueur est mort, le jeu est arrêté et on doit donc retirer la possibilité de continuer à jouer
			if (!player_alive) game_over();
		}
		else 
		{
			msg_display("Ce tunnel n'est pas accessible !");
		}

	}

	else if (choix == "s") {
		// Il faut qu'il reste des flèches pour en tirer une
		if (nb_arrows) {
			--nb_arrows;	// on les décrémente et on tire !
			arrows_display(nb_arrows);

			// On doit vérifier la survivance du wumpus ET du joueur post-tir : d'où le besoin de retourner deux valeurs bool en utilisant un t-uple, la string est un message à afficher
			auto live = shoot(player_room, tunnels_dispo, wumpus_room, maze);
			wumpus_alive = get<0>(live);
			player_alive = get<1>(live);
			msg_display(get<2>(live));

			// Si l'un des deux est morts, le jeu est arrêté et on doit donc retirer la possibilité de continuer à jouer
			if (!wumpus_alive || !player_alive) game_over();
		}

	}

	// Position du joueur et tunnels disponibles
	player_position_display();
	saisie_action.put("");
	redraw();							// On redessine la fenêtre
}

//------------------------------------------------------------------------------




// ********************************************************************************************************************************************************
int main()
try {
	
	Figures_window lab(Point(100, 100), 1200, 800, "Hunt the wumpus II");		
	
	// Initialisation du jeu ************************************************************************************************************** 
	srand((int)time(0));	// Seeding du moteur de génération aléatoire en partant de l'heure

	for (int room = 1; room< lab.nb_rooms; ++room) {

		// Création des tunnels de la room concernée
		tunnels(room, lab.nb_rooms, lab.maze);

	}

	// Construction 'visuelle' du labyrinthe  A FAIRE ***********************************************************************
	// for (auto const& tunnels : maze) cout << tunnels.first << '-' << tunnels.second << '\n';
	lab.msg_display("Taille du réseau créé : " + to_string(size(lab.maze)));
	lab.arrows_display(lab.nb_arrows);
	
	// Placement du Wumpus, des fosses et des chauve-souris au hasard
	lab.wumpus_room = nb_aleatoire(1, lab.nb_rooms);

	for (int i = 0; i< lab.nb_pits; ++i)
		lab.pit_rooms.push_back(nb_aleatoire(1, lab.nb_rooms));

	for (int i = 0; i< lab.nb_bats; ++i)
		lab.bat_rooms.push_back(nb_aleatoire(1, lab.nb_rooms));

	// Téléportation du joueur au démarrage dans une room vide
	while (lab.player_room_full) {
		lab.player_room = nb_aleatoire(1, lab.nb_rooms);

		if (is_wumpus_here(lab.player_room, lab.wumpus_room) || is_pit_here(lab.player_room, lab.pit_rooms) || is_bat_here(lab.player_room, lab.bat_rooms))
			lab.player_room_full = true;
		else
			lab.player_room_full = false;

	}

	// Position du joueur et tunnels disponibles
	lab.player_position_display();
	
	return gui_main();	// Control inversion page 569
}

catch (exception& e) {
	cerr << "exception: " << e.what() << '\n';
	return 1;
}

catch (...) {
	cerr << "Some exception\n";
	return 2;
}

//------------------------------------------------------------------------------



