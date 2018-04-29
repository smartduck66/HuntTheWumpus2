//
// Book : chapitre 16 de l'ouvrage
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2�me �dition : 2014)
// Commit initial : 22/04/2018 - Drill page 577 - Reprise d'une partie des fichiers de BS pr�sents dans C:\Users\andre\source\ppp2\GUI (probl�mes de compilation)
// Commit en cours : 26/04/2018 - exos pages 578 � 579
// Caract�res sp�ciaux : [ ]   '\n'   {  }   ||   ~   _     @

#include <iostream>
#include <sstream>
#include <memory>
#include "Graph.h"        // get access to our graphics library facilities
#include "GUI.h"
#include "Window.h"

using namespace Graph_lib;
using namespace std;

// DRILL page 577 **********************************************************************************************************************************

struct Lines_window : Window {
	Lines_window(Point xy, int w, int h, const string& title);
	Open_polyline lines;
	
private:
	Button next_button;        // add (next_x,next_y) to lines
	Button quit_button;
	In_box next_x;
	In_box next_y;
	Out_box xy_out;
	Menu color_menu;
	Menu style_menu;
	Button menu_button;			// Pour masquer ou pas le menu "color"

	void change(Color c) { lines.set_color(c); }
	void style(Line_style l) { lines.set_style(l); }
	void hide_menu() { color_menu.hide(); menu_button.show(); }

	// actions invoked by callbacks
	void red_pressed() { change(Color::red); }
	void blue_pressed() { change(Color::blue); }
	void black_pressed() { change(Color::black); }
	void solid_pressed() { style(Line_style::solid); }
	void dash_pressed() { style(Line_style::dash); }
	void dot_pressed() { style(Line_style::dot); }
	void menu_pressed() { menu_button.hide(); color_menu.show(); }
	void next();
	void quit();

	// callbacks functions
	static void cb_red(Address, Address);		
	static void cb_blue(Address, Address);		
	static void cb_black(Address, Address);	
	static void cb_solid(Address, Address);
	static void cb_dash(Address, Address);
	static void cb_dot(Address, Address);
	static void cb_menu(Address, Address);	
	static void cb_next(Address, Address);	
	static void cb_quit(Address, Address);	
	
};

//------------------------------------------------------------------------------

Lines_window::Lines_window(Point xy, int w, int h, const string& title)		// Constructeur
	:Window(xy, w, h, title),
	next_button(Point(x_max() - 150, 0), 70, 20, "Next point", cb_next),
	quit_button(Point(x_max() - 70, 0), 70, 20, "Quit", cb_quit),
	next_x(Point(x_max() - 310, 0), 50, 20, "next x:"),
	next_y(Point(x_max() - 210, 0), 50, 20, "next y:"),
	xy_out(Point(100, 0), 100, 20, "current (x,y):"),
	color_menu {Point{x_max()-70,30},70,20,Menu::vertical,"color"},
	style_menu{ Point{ x_max() - 70,100 },70,20,Menu::vertical,"style" },
	menu_button{ Point{ x_max() - 80,30 },80,20,"color menu",cb_menu }

{
	attach(next_button);
	attach(quit_button);
	attach(next_x);
	attach(next_y);
	attach(xy_out);
	xy_out.put("no point");
	
	color_menu.attach(new Button{ Point{ 0,0 },0,0,"red",cb_red });
	color_menu.attach(new Button{ Point{ 0,0 },0,0,"blue",cb_blue });
	color_menu.attach(new Button{ Point{ 0,0 },0,0,"black",cb_black });
	attach(color_menu);
	color_menu.hide();
	attach(menu_button);
	
	style_menu.attach(new Button{ Point{ 0,0 },0,0,"solid",cb_solid });
	style_menu.attach(new Button{ Point{ 0,0 },0,0,"dash",cb_dash });
	style_menu.attach(new Button{ Point{ 0,0 },0,0,"dot",cb_dot });
	attach(style_menu);

	attach(lines);

	
}

//------------------------------------------------------------------------------

void Lines_window::cb_quit(Address, Address pw)    // "the usual"
{
	reference_to<Lines_window>(pw).quit();
}

//------------------------------------------------------------------------------

void Lines_window::quit()
{
	hide();        // curious FLTK idiom for delete window
}

//------------------------------------------------------------------------------

void Lines_window::cb_next(Address, Address pw)     // "the usual"
{
	reference_to<Lines_window>(pw).next();
}

//------------------------------------------------------------------------------

void Lines_window::next()
{
	int x = next_x.get_int();
	int y = next_y.get_int();

	lines.add(Point(x, y));

	// update current position readout:
	stringstream ss;
	ss << '(' << x << ',' << y << ')';
	xy_out.put(ss.str());

	redraw();
}

//------------------------------------------------------------------------------

void Lines_window::cb_red(Address, Address pw)      // "the usual"
{
	reference_to<Lines_window>(pw).red_pressed();
}

//------------------------------------------------------------------------------

void Lines_window::cb_blue(Address, Address pw)     // "the usual"
{
	reference_to<Lines_window>(pw).blue_pressed();
}

//------------------------------------------------------------------------------

void Lines_window::cb_black(Address, Address pw)    // "the usual"
{
	reference_to<Lines_window>(pw).black_pressed();
}

//------------------------------------------------------------------------------

void Lines_window::cb_solid(Address, Address pw)      // "the usual"
{
	reference_to<Lines_window>(pw).solid_pressed();
}

//------------------------------------------------------------------------------

void Lines_window::cb_dash(Address, Address pw)     // "the usual"
{
	reference_to<Lines_window>(pw).dash_pressed();
}

//------------------------------------------------------------------------------

void Lines_window::cb_dot(Address, Address pw)    // "the usual"
{
	reference_to<Lines_window>(pw).dot_pressed();
}

//------------------------------------------------------------------------------

void Lines_window::cb_menu(Address, Address pw)     // "the usual"
{
	reference_to<Lines_window>(pw).menu_pressed();
}

// Exercices 1, 2, 3 page 578/579 **************************************************************************************
struct Simple_window : Window {
	Simple_window(Point xy, int w, int h, const string& title)
		: Window(xy, w, h, title),
		pressions_out(Point(100, 0), 30, 20, "Pressions :"),
		checker_out(Point(300, 0), 30, 20, "Case press�e :"),
		quit_button(Point(x_max() - 70, 0), 70, 20, "Quit", [](Address, Address pw) {reference_to<Simple_window>(pw).quit(); })	// Lambda as a callback : page 560 rather than "the usual callback"

	{
		// Cr�ation du damier
		int x_pos = 0;
		int y_pos = 50;
		
		for (int i = 0; i<16; ++i)
		{
			if (i != 0) {	// Gestion de la position
				if (i % 4 == 0) { x_pos = 0; y_pos += 80; } else ++x_pos;
			}
			
			add_checkers(new Button{ Point{ 100 + x_pos * 80,y_pos },80,80,to_string(i),cb_checker_pressed });	// Cr�ation des cases du damier
						
		}

		// Attachement de chaque case du damier � la fen�tre
		for (auto p : checkers)
			attach(*p);		 

		// Attachement des boutons de commande � la fen�tre
		attach(pressions_out);
		pressions_out.put("0");
		attach(checker_out);
		checker_out.put("ras");
		attach(quit_button);
		attach(part_of_duck);
		
	}

	
	void add_checkers(Graph_lib::Button* s) { checkers.push_back(s); }	// On cr�e une case du damier en la stockant dans un vecteur qui stocke un objet graphique "bouton"
	int nombre_pressions() const noexcept { return nb_pressions; }		// On retourne le nombre de pressions effectu�es sur les cases du damier
	
	~Simple_window()	// Destructeur
	{

		for (auto p : checkers)
			delete p;

	}
	
private:
	Button quit_button;
	Out_box pressions_out;
	Out_box checker_out;
	vector<Graph_lib::Button*>checkers{};							// Stockage des boutons du damier dans un vecteur
	int nb_pressions = 0;											// Stockage du nombre de pressions effectu�es sur les cases du damier
	Address pp_button{};											// Stockage de l'adresse du bouton pr�c�demment press�
	Graph_lib::Image part_of_duck{Point{ 0,50 }, "duck_eye.jpg" };	// On cr�e une image par d�faut que l'on place � gauche du damier, et que l'on fera bouger dessus


	// actions invoked by callbacks
	void quit();
	void checker_pressed(Address xw);

	// callbacks functions
	static void cb_checker_pressed(Address, Address);
	
};


void Simple_window::checker_pressed(Address xw)		// On a besoin, contrairement � d'habitude, de r�cup�rer l'adresse du widget qui correspond � l'une des cases du damier
{
	++nb_pressions;
	
	// update current nb checker_pressed readout:
	stringstream ss1;
	ss1 << nombre_pressions() ;
	pressions_out.put(ss1.str());

	// Affichage du num�ro de la case du damier press� -> Le code est trop low level toutefois (inspiration : int In_box::get_int() dans GUI.cpp)
	const Fl_Button& damier = reference_to<Fl_Button>(xw);	// On r�f�rence le widget press�
	const char* p = damier.label();							// On r�cup�re son label (fonction d�finie dans Fl_Widget.H)
	stringstream ss2;
	ss2 << p;
	checker_out.put(ss2.str());
	   
	// Exo 3 page 579 ******************************************************************************************************************************************************************************************
	// On utilise plut�t un objet Image que de cr�er une image via new Fl_JPEG_Image et de la stocker par widget.image() - En effet, je ne sais pas supprimer l'objet ensuite pour le faire bouger sur le damier
															
	reference_to<Fl_Button>(xw).color(Color::dark_cyan);					// On change la couleur de fond de la case press�e
	
	int dx, dy = 0;
	if (nombre_pressions()>1) {
			
		reference_to<Fl_Button>(pp_button).color(Color::defaut_background);	// On remet la couleur grise en background de la case "que l'on quitte"
		
		dx = damier.x() - reference_to<Fl_Button>(pp_button).x();			// On calcule l'offset de mouvement car l'acc�s direct � la modification des coordonn�es de l'image est impossible a priori
		dy = damier.y() - reference_to<Fl_Button>(pp_button).y();
		
	}
	
	else 
	{
		dx = damier.x()+15;													// Lors de la premi�re pression, on bouge l'image en partant de son emplacement initial
		dy = damier.y()-35;
	}
		
	part_of_duck.move(dx, dy);												// on bouge l'image 
	
	pp_button = xw;															// On stocke l'adresse de la case du damier qui vient donc d'�tre press�e
	
	redraw();
	
}

void Simple_window::cb_checker_pressed(Address xw, Address pw)     // Contrairement � d'habitude (page 557), on "nomme" la premi�re adresse "xw" qui correspond au widget stock� (pw �tant l'adresse de la fen�tre)
{
	reference_to<Simple_window>(pw).checker_pressed(xw);
}

//------------------------------------------------------------------------------

void Simple_window::quit()
{
	hide();        // curious FLTK idiom for delete window : cette fonction peut �tre supprim�e et on peut mettre le hide() directement dans le "lambda as a callback" ci-dessus (mais moins flexible si on veut changer de librairie)
}


// Exercice 4&5 page 579 **********************************************************************************************************************************

struct Figures_window : Window {
	Figures_window(Point xy, int w, int h, const string& title);
	
	// A l'instar du Drill (Open_polyline lines;), on cr�e les figures potentielles dans la structure afin qu'elles se "r�f�rencent" par rapport � la fen�tre
	// Evolution potentielle : on cr�e un vecteur de shapes pour cr�er � la vol�e le nombre d'objets que l'on d�sire
	Circle c;
	Graph_lib::Rectangle sq;
	Triangle_Rectangle tr;
		
	enum Figure_type {
		circle = 0,
		square = 1,
		triangle = 2,
				
	};

	void tracer(Figure_type fig);	// Fonction "g�n�rique" qui trace la figure souhait�e
	Point pointxy();				// Fonction helper permettant de r�cup�rer les coordonn�es saisies du centre de la figure
	int taille();					// Fonction helper permettant de r�cup�rer la taille de la figure
	Color backcolor_color() const noexcept { return bcolor; }	// Fonction qui retourne la couleur de fond

private:
	Button quit_button;
	Button clear_button;
	Button backcolor_button;
	In_box coord_x;
	In_box coord_y;
	In_box taille_fig;
	Menu figure_menu;
	Color bcolor{ Color::invisible };	// Couleur de fond d'une figure : par d�faut, aucune

	// actions invoked by callbacks
	void circle_pressed() { tracer(Figure_type::circle); }
	void square_pressed() { tracer(Figure_type::square); }
	void triangle_pressed() { tracer(Figure_type::triangle); }
	void quit();
	void clear();
	void backcolor();

	// callbacks functions
	static void cb_circle(Address, Address);
	static void cb_square(Address, Address);
	static void cb_triangle(Address, Address);
	static void cb_quit(Address, Address);
	static void cb_clear(Address, Address);
	static void cb_backcolor(Address, Address);

};

//------------------------------------------------------------------------------

Figures_window::Figures_window(Point xy, int w, int h, const string& title)		// Constructeur
	:Window(xy, w, h, title),
	quit_button(Point(x_max() - 70, 0), 70, 20, "Quit", cb_quit),
	clear_button(Point(x_max() - 70, 30), 70, 20, "Clear", cb_clear),
	backcolor_button(Point(x_max() - 70, 60), 70, 20, "None", cb_backcolor),
	coord_x(Point(x_max() - 410, 0), 50, 20, "x :"),
	coord_y(Point(x_max() - 310, 0), 50, 20, "y :"),
	taille_fig(Point(x_max() - 210, 0), 50, 20, "taille :"),
	figure_menu{ Point{ x_max() - 70,100 },70,20,Menu::vertical,"figures" }
	

{
	
	attach(quit_button);
	attach(clear_button);
	attach(backcolor_button);
	attach(coord_x);
	coord_x.put(x_max() / 2);	// Valeur par d�faut
	attach(coord_y);
	coord_y.put(y_max() / 2);	// Valeur par d�faut
	attach(taille_fig);
	taille_fig.put(100);		// Valeur par d�faut
		
	figure_menu.attach(new Button{ Point{ 0,0 },0,0,"circle",cb_circle });
	figure_menu.attach(new Button{ Point{ 0,0 },0,0,"square",cb_square });
	figure_menu.attach(new Button{ Point{ 0,0 },0,0,"triangle",cb_triangle });
	
	attach(figure_menu);

}

//------------------------------------------------------------------------------

void Figures_window::cb_quit(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).quit();
}

void Figures_window::cb_clear(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).clear();
}

void Figures_window::cb_backcolor(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).backcolor();
}
//------------------------------------------------------------------------------

void Figures_window::quit()
{
	hide();			// curious FLTK idiom for delete window
}

void Figures_window::clear()
{
	detach(c);							// On d�tache les 3 objets
	detach(sq);
	detach(tr);
	bcolor = Color::invisible;
	backcolor_button.label = "none";
	coord_x.put(x_max() / 2);			// Valeur par d�faut
	coord_y.put(y_max() / 2);			// Valeur par d�faut
	taille_fig.put(100);				// Valeur par d�faut
	redraw();							// On redessine la fen�tre		
}

void Figures_window::backcolor()
{
	// Flip flop : pas de couleur de fond ou bien rouge
	if (backcolor_button.label=="none") {
		bcolor=Color::red;
		backcolor_button.label = "red";
	}
	else {
		bcolor = Color::invisible;
		backcolor_button.label = "none";
	}
}


//------------------------------------------------------------------------------

void Figures_window::cb_circle(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).circle_pressed();
}

void Figures_window::cb_square(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).square_pressed();
}

void Figures_window::cb_triangle(Address, Address pw)    // "the usual"
{
	reference_to<Figures_window>(pw).triangle_pressed();
}


//------------------------------------------------------------------------------

void Figures_window::tracer(Figure_type fig)
{
	// A l'instar du Drill, on construit donc "� la main" la figure en initialisant les param�tres n�cessaires	
	
	
	switch (fig) {
	case circle:
	{
		c.set_radius(taille());
		if (c.number_of_points() == 0) c.set_center(pointxy()); else c.new_center(pointxy());	// Le 1er trac� cr�e le centre, les suivants le modifie
		c.set_color(Color::black);
		c.set_fill_color(backcolor_color());
		attach(c);
		break; 
	}

	case square:
	{
		sq.set_major(taille());
		sq.set_minor(taille());
		if (sq.number_of_points() == 0) sq.add(pointxy()); else sq.update_origine(pointxy());	// Le 1er trac� cr�e le carr�, les suivants le modifie
		sq.set_color(Color::black);
		sq.set_fill_color(backcolor_color());
		attach(sq);
		break;
	}
	
	case triangle:
	{
		if (tr.number_of_points() == 0) {														// Le 1er trac� cr�e le triangle, les suivants le modifie
			tr.add(pointxy());
			tr.add(Point{ pointxy().x + taille(), pointxy().y });
			tr.add(Point{ pointxy().x, pointxy().y+ taille() });

		} else {
			tr.update_point(0,pointxy());
			tr.update_point(1,Point{ pointxy().x + taille(), pointxy().y });
			tr.update_point(2,Point{ pointxy().x, pointxy().y + taille() });

		}
		tr.set_color(Color::black);
		tr.set_fill_color(backcolor_color());
		attach(tr);
		break;
	}

	
	default:
		break;
	}
	
	redraw();
}

Point Figures_window::pointxy()		// Cette fonction helper renvoit les coordonn�es du centre de la figure saisi, sinon prend par d�faut le centre de la fen�tre
{
	int x = coord_x.get_int();
	int y = coord_y.get_int();
	
	if (x < 0 || x > x_max()) x = x_max() / 2;
	if (y < 0 || y > y_max()) y = y_max() / 2;

	return Point{x,y};
}

int Figures_window::taille()		// Cette fonction helper renvoit la taille de la figure, sinon prend par d�faut une valeur de 100
{
	int t = taille_fig.get_int();
	
	if (t < 0 || t > x_max()) t = 100;
	
	return t;
}
//------------------------------------------------------------------------------


// ********************************************************************************************************************************************************
int main()
try {
	// Lines_window win(Point(100, 100), 600, 400, "lines");					// Drill page 577

	// Simple_window My_window(Point(100, 100), 600, 400, "checkerboard");		// Exos 2&3 page 578

	Figures_window win(Point(100, 100), 1000, 800, "Figures g�om�triques");		// Exos 4&5 page 579
	
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
