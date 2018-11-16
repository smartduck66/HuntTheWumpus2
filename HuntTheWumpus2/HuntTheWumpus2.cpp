//
// Book : chapitre 19 de l'ouvrage PPP2
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 28/07/2018 - Exos 14&15 page 710 - Hunt The Wumpus "Graphique"
// Commit en cours : 03/08/2018 
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @
// NOTE : on a tenté de conserver la structure de la version 1 autant que faire se peut. HuntTheWumpus.cpp contient les fonctions Helper du jeu initial.
//

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
	int nb_arrows = 5;
	vector <int> tunnels_dispo{};
	Vector_ref <Graph_lib::Circle>rooms_maze{};
	Vector_ref <Graph_lib::Text>rooms_maze_number{};
	vector<Graph_lib::Shape*>lab_sprites{};				// Stockage des éléments graphiques éventuels du labyrinthe (ex : images)
	
	~Figures_window()	// Destructeur des éléments "new" - L'utilisation de Vector_ref (Graph.h) aurait évité l'implémentation du destructeur MAIS Vector_ref n'a pas implémenté la boucle for (auto p..)
	{

		for (auto p : lab_sprites)
			delete p;

	}
	
	
	// --------------------------------------------------------
			
	void msg_display(const string& s) { msg_game.put(s); }
	void arrows_display(int a) { arrows.put(a); }
	void game_over();
	void player_position_display();
	void add_sprites(Graph_lib::Shape* s) { lab_sprites.push_back(s); }	// MAJ du vecteur stockant les sprites
	void display_sprites();

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
	msg_game(Point(100, y_max() - 40), 800, 20, "Info :"),
	arrows(Point(970, y_max() - 40), 20, 20, "Flèches :"),
	player_position(Point(100, y_max() - 20), 600, 20, "Position :")
		

{
	// Apparition des éléments graphiques
	attach(quit_button);
	attach(action_button);
	attach(saisie_action);
	attach(msg_game);
	attach(arrows);
	attach(player_position);

	// Construction graphique des salles	
	// . On "boucle" autour d'un cercle imaginaire et on détermine le centre des différentes salles représentées par un cercle
	// . x = a + R*cos angle 	y = b + R*sin angle (a et b étant les coordonnées du centre du cercle, R le rayon)
	// . Attention : i est en radians (360° = 2PI Radians)
	const double pas = (2 * PI)/ nb_rooms;
	const int room_radius = 33;
	for (double i = 0; i<2 * PI; i += pas)
	{
		const int x_polaire = static_cast<int>(round(x_max()/2 + 320 * cos(i)));
		const int y_polaire = static_cast<int>(round(y_max()/2 + 320 * sin(i)));
		
		rooms_maze.push_back(new Graph_lib::Circle{ Point{ x_polaire,y_polaire },room_radius });
		rooms_maze_number.push_back(new Graph_lib::Text{ Point{ x_polaire,y_polaire },to_string(static_cast<int>(round(i/pas))+1) });
		
	}

	// Affichage et "nommage" des rooms
	for (int i = 0; i<rooms_maze.size(); ++i) {
		rooms_maze[i].set_color(Color::black);
		attach(rooms_maze[i]);
		rooms_maze_number[i].set_color(Color::black);
		attach(rooms_maze_number[i]);
				
	}
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

	// On "allume" la salle où se trouve le joueur
	rooms_maze[player_room-1].set_color(Color::red);
	rooms_maze_number[player_room - 1].set_color(Color::red);
			
}

void Figures_window::game_over()    // Le jeu est terminé : on désactive les zones de saisie et d'action : seul QUIT reste actif
{

	detach(action_button);
	detach(saisie_action);
	detach(arrows);
	detach(player_position);

}

void Figures_window::display_sprites()    // Affichage des sprites
{

	// Attachement des éléments à la fenêtre
	for (auto p : lab_sprites) {
		attach(*p);
	}
}




void Figures_window::action()
{
	// **********************************************************************************************************************************
	// "Boucle de jeu" principale
	
	// Position du joueur et tunnels disponibles
	msg_display("");
	player_position_display();

	// Coordonnées du centre de la fenêtre pour affichage d'une image
	Point pc{ x_max() / 2 - 100,y_max() / 2 - 100 };

	// Saisie de l'option Move (mx), Shoot (s) or Quit (q)
	string option = saisie_action.get_string();
		
	string choix = option.substr(0, 1);
	if (choix == "q") {
		msg_display("Bye");
		game_over();

	}

	else if (choix == "m") {
		// Si le numéro du tunnel choisi est valide, on transporte le joueur dans la nouvelle salle
		// Son "état de vie" peut y être affecté en fonction de la présence du wumpus ou d'une fosse ; sinon il est transféré dans la nouvelle salle (player_room updaté)
		int new_room = stoi(option.substr(1, option.size() - 1));
		if (is_new_room_valid(new_room, tunnels_dispo)) 
		{
			// On indique la salle quittée par le joueur avec une couleur particulière
			rooms_maze[player_room - 1].set_color(Color::yellow);
			rooms_maze_number[player_room - 1].set_color(Color::yellow);
						
			// On le transfère dans la nouvelle salle
			const int live = hazard(new_room, wumpus_room, pit_rooms, bat_rooms, player_room, nb_rooms);
						
			// Calcul des coordonnées pour afficher le sprite du wumpus, de l'oubliette ou de la chauve-souris
			Point p{ rooms_maze_number[new_room - 1].point(0).x - 22, rooms_maze_number[new_room - 1].point(0).y - 25 };		
			
			switch (live)
			{
			case EVENT_WUMPUS:
				msg_display("Désolé... Le Wumpus est dans cette salle :-( Tu es mort !");
				add_sprites(new Graph_lib::Image{ p ,"wumpus.jpg" });
				add_sprites(new Graph_lib::Image{ pc ,"mort.jpg" });
				display_sprites();
				game_over();
				break;
			
			case EVENT_PIT:
				msg_display("Désolé... Il y a une fosse dans cette salle :-( Tu es mort !");
				add_sprites(new Graph_lib::Image{ p ,"oubliette.jpg" });
				add_sprites(new Graph_lib::Image{ pc ,"mort.jpg" });
				display_sprites();
				game_over();
				break;

			case EVENT_BAT:
				msg_display("Une chauve-souris se trouve dans cette salle... Elle te transporte dans la salle " + to_string(player_room));
				add_sprites(new Graph_lib::Image{ p ,"chauve-souris.jpg" });
				display_sprites();
				break;

			case EVENT_EMPTY_ROOM:
				msg_display("Cette salle est vide... ");
				break;

			default:
				break;
			}
						
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

			// On doit vérifier la survivance du wumpus 
			const int live = shoot(player_room, tunnels_dispo, wumpus_room, maze);
			Point pw{ rooms_maze_number[wumpus_room - 1].point(0).x - 22, rooms_maze_number[wumpus_room - 1].point(0).y - 25 }; // Coordonnées pour afficher le sprite

			switch (live)
			{
			case SHOOT_WUMPUS_KILLED:
				msg_display("BRAVO !! Tu as tué le Wumpus qui se trouvait en salle " + to_string(wumpus_room) + ". C'est gagné !!");
				
				add_sprites(new Graph_lib::Image{ pw ,"wumpus.jpg" });
				add_sprites(new Graph_lib::Image{ pc ,"winner.jpg" });
				display_sprites();
				
				game_over();
				break;

			case SHOOT_WUMPUS_WAKEUP:
				msg_display("Le Wumpus a été réveillé par l'une de tes flèches qui a atterri dans une pièce adjacente à l'endroit où il se trouve. Il a bougé...");
				break;

			case SHOOT_NOEFFECT:
				msg_display("Le tir n'a eu aucun effet...");
				break;
							
			default:
				break;
			}
			
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

	// Affichage de la taille du réseau et du nombre de flèches
	lab.msg_display("Taille du réseau créé : " + to_string(size(lab.maze)));
	lab.arrows_display(lab.nb_arrows);
	
	// Placement du Wumpus, des fosses et des chauve-souris au hasard
	lab.wumpus_room = nb_aleatoire(1, lab.nb_rooms);

	for (int i = 0; i< lab.nb_pits; ++i)
		lab.pit_rooms.push_back(nb_aleatoire(1, lab.nb_rooms));

	for (int i = 0; i< lab.nb_bats; ++i)
		lab.bat_rooms.push_back(nb_aleatoire(1, lab.nb_rooms));

	/*
	// Contrôle des pièges pour debug *******************
	string s="bats =";
	for (auto x : lab.bat_rooms) s += to_string(x) + ' ';
	s += " / pits = ";
	for (auto x : lab.pit_rooms) s += to_string(x) + ' ';
	s += " / wumpus = " + to_string(lab.wumpus_room);
	lab.msg_display(s); 
	// **************************************************
	*/

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



