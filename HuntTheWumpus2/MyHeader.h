/*
MyHeader.h
*/

/*
first own header

Revised July 21, 2018: initial release
Revised August 3, 2018: function prototypes added

*/

#ifndef MY_HEADER_GUARD	// Pour éviter que mon header soit compilé deux fois
#define MY_HEADER_GUARD

#include<string>
#include<iostream>
#include<vector>
#include<map>	// pour bénéficier du container multimap

using namespace std;

inline void keep_window_open() // From "std_lib_facilities.h" of BS
{
	cin.clear();
	cout << "Please enter a character to exit\n";
	char ch;
	cin >> ch;
	return;
}

inline void keep_window_open(string s)
{
	if (s == "") return;
	cin.clear();
	cin.ignore(120, '\n');
	for (;;) {
		cout << "Please enter " << s << " to exit\n";
		string ss;
		while (cin >> ss && ss != s)
			cout << "Please enter " << s << " to exit\n";
		return;
	}
}

struct Exit : runtime_error {
	Exit() : runtime_error("Exit") {}
};

// error() simply disguises throws:
inline void error(const string& s)
{
	throw runtime_error(s);
}

inline void error(const string& s, const string& s2)
{
	error(s + s2);
}

// SPECIFIQUE AU PROJET EN COURS **************************************************************************************************************************************
// Définition des prototypes de fonctions écrites dans la 1ère version de HuntTheWumpus (corps présent dans HuntTheWumpus.cpp)
namespace HuntTheWumpus_lib {

	const int EVENT_WUMPUS = 1;
	const int EVENT_PIT = 2;
	const int EVENT_BAT = 3;
	const int EVENT_EMPTY_ROOM = 4;

	const int SHOOT_WUMPUS_KILLED = 1;
	const int SHOOT_WUMPUS_WAKEUP = 2;
	const int SHOOT_NOEFFECT = 3;
	
	int nb_aleatoire(const int min, const int max);
	void tunnels(const int room, const int nb_rooms_maze, multimap<int, int>&current_maze);
	bool is_wumpus_here(const int room, const int wumpus_room);
	bool is_pit_here(const int room, const vector<int>& pit_rooms);
	bool is_bat_here(const int room, const vector<int>& bat_rooms);
	void tunnels_in_room(const int room, vector<int>& tunnels, const multimap<int, int>&current_maze);
	bool is_new_room_valid(const int room, const vector<int>& tunnels);
	const int hazard(int room, const int wumpus_room, const vector<int>& pit_rooms, const vector<int>& bat_rooms, int& player_room, const int nb_rooms_maze);
	int wumpus_move(const int w_room, const multimap<int, int>&current_maze);
	const int shoot(const int room, const vector<int>& tunnels, int& wumpus_room, const multimap<int, int>&current_maze);

}
// *************************************************************************************************************************************************************************

#endif