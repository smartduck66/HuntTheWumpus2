#include<iostream>
#include<iterator>
#include<vector>
#include<string>
#include<utility>
#include<algorithm>
#include<ctime>
#include<map>	// pour bénéficier du container multimap
#include "MyHeader.h"


using namespace std;

namespace HuntTheWumpus_lib {

int nb_aleatoire(const int min, const int max)
{
	// Cette fonction helper gère le tirage de nombres aléatoires entre deux bornes passées en arguments
	// On préfère la fonction C rand() à randint(min, max) incluse dans std_lib_facilities.h
	// Le seeding réalisée dans le main() permet une génération pseudo-aléatoire (meilleurs outils à partir de C++ 11)
	int a= rand() % max + min;
	return a;
}


void tunnels(const int room, const int nb_rooms_maze, multimap<int, int>&current_maze)
{  
	// ***********************************************************************************************************************************
	// Cette fonction alimente le container multimap 'current_maze' passé par référence (créé dans le main) en créant le nombre de tunnels
	// adéquats pour la 'room' passée en argument. Chaque 'room' ne peut avoir que 3 tunnels.
	// ***********************************************************************************************************************************

	// On ne peut avoir que 3 tunnels possibles au maximum pour une pièce donnée
	// On vérifie donc leur nombre pour la pièce passée en paramètre en utilisant les algorithmes liés au container multimap
	auto z = current_maze.equal_range(room);
	int nb_tunnels_existants = distance(z.first, z.second);
	int new_tunnel = 0;
	
	// On crée alors le bon nombre de tunnels pour la room (0 éventuellement si 3 tunnels existent déjà...)
	for (int i=1;i<= 3-nb_tunnels_existants;++i) {
		
		// Création du sens room -> new room
		bool creation_possible = false;
				
		while (!creation_possible) {
			new_tunnel = nb_aleatoire(1, nb_rooms_maze);	
			
			// Le nouveau tunnel ne peut boucler sur lui-même
			if (new_tunnel != room) creation_possible = true;
			
			// Tunnel déjà existant pour la pièce en cours ? L'algo "any_of" est mal maîtrisé, remplacé donc par une boucle simple
			// On ne peut pas se baser sur le 'z = current_maze.equal_range(room)' dans le cas où aucune occurence de la room n'existe 
			// ET que deux tunnels identiques se créent dans la boucle 'i' en cours
			auto ze = current_maze.equal_range(room);
			// if (any_of(ze.first, ze.second, [&new_tunnel](auto const& ze) { return ze.second == &new_tunnel; })) creation_possible = false;
			for (auto t = ze.first; t != ze.second; ++t)
				if (t->second == new_tunnel) creation_possible = false;
			
			// Le nombre de tunnels de la pièce pointée par new_tunnel ne doit pas déjà être égal à 3...
			auto zn = current_maze.equal_range(new_tunnel);
			if (distance(zn.first, zn.second) == 3) {
				creation_possible = false;
				
				// ... Néanmoins, pour éviter une boucle infinie (un ou plusieurs tunnels n'auront pas alors 3 issues)
				if (size(current_maze)>static_cast<unsigned int>((room-1)*3)) creation_possible = true;
			}
			
		}
		current_maze.insert(make_pair(room, new_tunnel));	// On crée le tunnel dès que l'on sort du 'while'
		
		// Création du sens new room -> room si et seulement si le nombre de tunnels est également inférieur ou égal à 3
		// Cette étape 'inverse' est nécessaire pour éviter de créer de nouveaux tunnels quand l'appel à la fonction fera que 'room' sera égal à 'new_tunnel'
		auto zx = current_maze.equal_range(new_tunnel);
		if (distance(zx.first, zx.second) < 3) 
			current_maze.insert(make_pair(new_tunnel, room));
		
	}

}

bool is_wumpus_here(const int room, const int wumpus_room)
{
	// Cette fonction helper teste si le wumpus se trouve dans la pièce où entre le joueur
	return (room == wumpus_room);

}

bool is_pit_here(const int room, const vector<int>& pit_rooms)
{
	// Cette fonction helper teste si une fosse se trouve dans la pièce où entre le joueur
	// Utilisation d'un algorithme de recherche plus compact qu'une boucle (A tour of C++ : page 109)
	 return find(pit_rooms.begin(), pit_rooms.end(),room) != pit_rooms.end();

}

bool is_bat_here(const int room, const vector<int>& bat_rooms)
{
	// Cette fonction helper teste si une chauve-souris se trouve dans la pièce où entre le joueur
	return find(bat_rooms.begin(), bat_rooms.end(), room) != bat_rooms.end();
}

void tunnels_in_room(const int room, vector<int>& tunnels, const multimap<int, int>&current_maze)
{
	// Cette fonction helper popule le vecteur passé en référence avec les tunnels disponibles dans une pièce donnée
	auto ze = current_maze.equal_range(room);
	
	for (auto t = ze.first; t != ze.second; ++t)
		tunnels.push_back(t->second);
	
}

bool is_new_room_valid(const int room, const vector<int>& tunnels)
{
	// Cette fonction helper vérifie si le mouvement souhaité par le joueur est valide
	for (unsigned int i = 0; i< size(tunnels); ++i) {

		if (tunnels[i] == room) return true;

	}
		
	return false;

}

const int hazard(int room, const int wumpus_room, const vector<int>& pit_rooms, const vector<int>& bat_rooms, int& player_room, const int nb_rooms_maze)
{
	// Cette fonction helper vérifie les dangers de la salle où le joueur a souhaité bouger
	// On retourne un event pour pouvoir afficher des messages correctement en retour
	// int room n'est pas passé en const car peut être modifié dans le cas de la chauve-souris ;
	// player_room également pour repartir dans la boucle principale du jeu
	// *****************************************************************************************
	
	if (is_wumpus_here(room, wumpus_room)) {
		return EVENT_WUMPUS;
	}
	
	if (is_pit_here(room, pit_rooms)) {
		return EVENT_PIT;
	}

	if (is_bat_here(room, bat_rooms)) {
		
		// Une chauve-souris nous transporte dans une salle sans danger
		bool clear_room = false;
		while (!clear_room) {
			room = nb_aleatoire(1, nb_rooms_maze);

			if (is_wumpus_here(room, wumpus_room) || is_pit_here(room, pit_rooms) || is_bat_here(room, bat_rooms))
				clear_room = false;
			else
				clear_room = true;

		}

		// La salle est "clear" : on modifie donc la salle du joueur et on repart dans la boucle du jeu
		player_room = room;
		return EVENT_BAT;
		
	}

	// La salle est "clear" : on modifie donc la salle du joueur et on repart dans la boucle du jeu
	player_room = room;
	return EVENT_EMPTY_ROOM;

}

int wumpus_move(const int w_room, const multimap<int, int>&current_maze)
{
	// Fonction Helper qui fait bouger le wumpus s'il est réveillé par un tir de flèche
	// ********************************************************************************
	
	// On récupère les tunnels disponibles à partir de la salle où dort le Wumpus
	vector <int> tunnels_dispo{};
	tunnels_in_room(w_room, tunnels_dispo, current_maze);

	// On tire au hasard le rang de la salle, compris entre 0 et la taille du vecteur précédent et
	// on retourne la nouvelle salle où le wumpus a bougé
	return tunnels_dispo[nb_aleatoire(0, size(tunnels_dispo)-1)];

}

const int shoot(const int room, const vector<int>& tunnels, int& wumpus_room, const multimap<int, int>&current_maze)
{
	// Cette fonction helper gère le tir
	// On renvoit seulement le résultat du tir
	// On passe wumpus_room par référence NON CONST car elle peut être modifiée si le Wumpus est réveillé et bouge
	// *************************************************************************************************************
	
	// Le Wumpus est-il présent dans l'une des salles adjacentes ?
	for (unsigned int i = 0; i< size(tunnels); ++i) {

		if (tunnels[i] == wumpus_room) {
			return SHOOT_WUMPUS_KILLED;
		}
	}
	
	// Le Wumpus se réveille et bouge si la flèche a été tirée vers une salle accessible de la pièce où il se trouve
	for (unsigned int i = 0; i< size(tunnels); ++i) {

		// On récupère les salles accessibles de chaque pièce où une flèche a été décochée
		auto ze = current_maze.equal_range(tunnels[i]);
		for (auto t = ze.first; t != ze.second; ++t)
			if (t->second == wumpus_room) {
				wumpus_room=wumpus_move(wumpus_room, current_maze);
				return SHOOT_WUMPUS_WAKEUP;
			}
	}
	
	// Rien ne se passe...
	return SHOOT_NOEFFECT;

}

}	// HuntTheWumpus_lib

  