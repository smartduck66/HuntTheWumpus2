// Fichier modifié suite aux exercices BS du chapitre 13

#ifndef GRAPH_GUARD
#define GRAPH_GUARD 1

#include "Point.h"
#include<vector>
#include<map>	// pour bénéficier du container multimap qui n'est a priori pas défini dans std_lib_facilities.h
#include "std_lib_facilities.h"
#include "fltk.h"
#include <functional>
#include <algorithm>
//#include<string>
//#include<cmath>

namespace Graph_lib {
// defense against ill-behaved Linux macros:
#undef major
#undef minor

// Constantes pour l'espace de travail bar_graph : on les place en dehors de la struct pour pouvoir les utiliser dans graph.cpp
static const double PI = 3.1415926535897;
static int const bar_width = 20;		// x pixels par barre
static int const bar_space = 10;		// x pixels de séparation entre les barres
static int const left_space = 30;		// x pixels entre le bord gauche et le graphe
static int const right_space = 10;		// x pixels entre le bord droit et le graphe
static int const top_space = 30;		// x pixels entre le bord haut et le graphe
static int const bottom_space = 30;		// x pixels entre le bord bas et le graphe

struct Color {
	enum Color_type {
		red=FL_RED, blue=FL_BLUE, green=FL_GREEN,
		yellow=FL_YELLOW, white=FL_WHITE, black=FL_BLACK,
		magenta=FL_MAGENTA, cyan=FL_CYAN, dark_red=FL_DARK_RED,
		dark_green=FL_DARK_GREEN, dark_yellow=FL_DARK_YELLOW, dark_blue=FL_DARK_BLUE,
		dark_magenta=FL_DARK_MAGENTA, dark_cyan=FL_DARK_CYAN, defaut_background=FL_BACKGROUND_COLOR // Rajout exo 3 page 579
	};
	enum Transparency { invisible = 0, visible=255 };

	Color(Color_type cc) noexcept:c(Fl_Color(cc)), v(visible) { }
	Color(Color_type cc, Transparency vv) noexcept :c(Fl_Color(cc)), v(vv) { }
	Color(int cc) noexcept:c(Fl_Color(cc)), v(visible) { } 
	Color(Transparency vv) noexcept:c(Fl_Color()), v(vv) { }

	int as_int() const noexcept { return c; }
	char visibility() const noexcept { return v; }
	void set_visibility(Transparency vv) noexcept { v=vv; }
private:
	unsigned char v;	// 0 or 1 for now
	Fl_Color c;
};

struct Line_style {
	enum Line_style_type {
		solid=FL_SOLID,				// -------
		dash=FL_DASH,				// - - - -
		dot=FL_DOT,					// ....... 
		dashdot=FL_DASHDOT,			// - . - . 
		dashdotdot=FL_DASHDOTDOT,	// -..-..
	};
	Line_style(Line_style_type ss) noexcept:s(ss), w(0) { }
	Line_style(Line_style_type lst, int ww) noexcept :s(lst), w(ww) { }
	Line_style(int ss) noexcept :s(ss), w(0) { }
	
	int width() const noexcept { return w; }
	int style() const noexcept { return s; }
private:
	int s;
	int w;
};

class Font {
public:
	enum Font_type {
		helvetica=FL_HELVETICA,
		helvetica_bold=FL_HELVETICA_BOLD,
		helvetica_italic=FL_HELVETICA_ITALIC,
		helvetica_bold_italic=FL_HELVETICA_BOLD_ITALIC,
		courier=FL_COURIER,
  		courier_bold=FL_COURIER_BOLD,
  		courier_italic=FL_COURIER_ITALIC,
  		courier_bold_italic=FL_COURIER_BOLD_ITALIC,
		times=FL_TIMES,
		times_bold=FL_TIMES_BOLD,
		times_italic=FL_TIMES_ITALIC,
		times_bold_italic=FL_TIMES_BOLD_ITALIC,
		symbol=FL_SYMBOL,
		screen=FL_SCREEN,
		screen_bold=FL_SCREEN_BOLD,
		zapf_dingbats=FL_ZAPF_DINGBATS
	};

	Font(Font_type ff) noexcept:f(ff) { }
	Font(int ff) noexcept :f(ff) { }

	int as_int() const noexcept { return f; }
private:
	int f;
};

template<class T> class Vector_ref {
	vector<T*> v;
	vector<T*> owned;
public:
	Vector_ref() noexcept {}

	Vector_ref(T* a, T* b=0, T* c=0, T* d=0)
	{
			if (a) push_back(a);
			if (b) push_back(b);
			if (c) push_back(c);
			if (d) push_back(d);
	}

	~Vector_ref() { for (unsigned i=0; i<owned.size(); ++i) delete owned[i]; }

	void push_back(T& s) { v.push_back(&s); }
	void push_back(T* p) { v.push_back(p); owned.push_back(p); }

	// ???void erase(???)

	T& operator[](int i) { return *v[i]; }
	const T& operator[](int i) const { return *v[i]; }
	int size() const noexcept { return v.size(); }
};


class Shape  {	// deals with color and style, and holds sequence of lines
protected:
	Shape() noexcept { }
	Shape(initializer_list<Point> lst);  // add() the Points to this Shape

//	Shape() : lcolor(fl_color()),
//		ls(0),
//		fcolor(Color::invisible) { }
	
	void add(Point p){ points.push_back(p); }
	void set_point(int i, Point p) { points[i] = p; }
public:
	void draw() const;					// deal with color and draw_lines
	
protected:
	virtual void draw_lines() const;	// simply draw the appropriate lines
public:
	virtual void move(int dx, int dy);	// move the shape +=dx and +=dy

	void set_color(Color col) noexcept { lcolor = col; }
	Color color() const noexcept { return lcolor; }

	void set_style(Line_style sty) noexcept { ls = sty; }
	Line_style style() const noexcept { return ls; }

	void set_fill_color(Color col) noexcept { fcolor = col; }
	Color fill_color() const noexcept { return fcolor; }

	Point point(int i) const noexcept { return points[i]; }
	int number_of_points() const noexcept { return int(points.size()); }

	virtual ~Shape() { }
	/*
	struct Window* attached; // ********************************
	Shape(const Shape& a)
		:attached(a.attached), points(a.points), line_color(a.line_color), ls(a.ls)
	{
		if (a.attached)error("attempt to copy attached shape");
	} // ********************************************
	*/
	Shape(const Shape&) = delete;				// Copy constructor
	Shape& operator=(const Shape&) = delete;	// Copy assignment
private:
	vector<Point> points;	// not used by all shapes
	Color lcolor {fl_color()};
	Line_style ls {0};
	Color fcolor {Color::invisible};

//Shape(const Shape&); // *******************************
//Shape& operator=(const Shape&);  // *******************************
};

typedef double Fct(double);

struct Function : Shape {
	// the function parameters are not stored
	Function(Fct f, double r1, double r2, Point orig, int count = 100, double xscale = 25, double yscale = 25);
	//Function(Point orig, Fct f, double r1, double r2, int count, double xscale = 1, double yscale = 1);	
};

struct Fct_stored : Shape {		// Exo 2 page 548 : on modifie la fonction précédente pour stocker les paramètres
								// On utilise le mot-clé function issu de la STD (#include <functional>) pour stocker la valeur de f retournée par une fonction définie dans fltk_test (ex : sin, cos, etc.)
								// On remarque la similitude avec "typedef double Fct(double)" utilisé par la structure initiale Function : Shape
								// Plus d'informations : A tour of C++ page 127
	
	Fct_stored() noexcept { }	// Constructeur par défaut

	Fct_stored(function<double(double)> f, double r1, double r2, Point orig, int count, double xscale, double yscale) : fonction{ f }, r_min{ r1 }, r_max{ r2 }, o{ orig }, n_points{ count }, x_scale{ xscale }, y_scale{ yscale }
	{  
		if (r_max - r_min <= 0) error("bad graphing range");
		if (n_points <= 0) error("non-positive graphing count");
		const double dist = (r_max - r_min) / n_points;
		double r = r_min;
		for (int i = 0; i<count; ++i) {
			add(Point(o.x + int(r*x_scale), o.y - int(f(r)*y_scale)));
			r += dist;
		}
		
	}
	void reset() const noexcept { Fct_stored(); }	// On appelle le constructeur par défaut pour remettre à zéro les paramètres

private:
	function<double(double)> fonction{};	
	double r_min = 0;
	double r_max = 0;
	Point o{};
	int n_points = 100;
	double x_scale = 25;
	double y_scale = 25;

};

struct Fill {
	Fill() noexcept:no_fill(true), fcolor(0) { }
	Fill(Color c) noexcept :no_fill(false), fcolor(c) { }

	void set_fill_color(Color col) noexcept { fcolor = col; }
	Color fill_color() noexcept { return fcolor; }
protected:
	bool no_fill;
	Color fcolor;
};

struct Line : Shape {
	Line() noexcept { }			// Constructeur par défaut (exo 6 page 579)
	Line(Point p1, Point p2) { add(p1); add(p2); }
};

struct Rectangle : Shape {
	Rectangle() noexcept{ }			// Constructeur par défaut (utile pour les classes dérivées, telles que Striped_rectangle...)
	Rectangle(Point xy, int ww, int hh) :w{ ww }, h{ hh }, x_origin{ xy.x }, y_origin{ xy.y }			// Rajout de la construction de x&y_origin pour l'exo 4 page 484
	{
		if (h<=0 || w<=0) error("Bad rectangle: non-positive side");
		add(xy);
	}
	Rectangle(Point x, Point y) :w{ y.x - x.x }, h{ y.y - x.y }, x_origin{ x.x }, y_origin{ x.y }		// Rajout de la construction de x&y_origin pour l'exo 4 page 484
	{
		if (h<=0 || w<=0) error("Bad rectangle: first point is not top left");
		add(x);
	}
	void draw_lines() const override;

//	void set_fill_color(Color col) { fcolor = col; }
//	Color fill_color() { return fcolor; }

	int height() const noexcept { return h; }
	int width() const noexcept { return w; }

	// Rajout des fonctions demandées à l'exercice n°4 page 438
	Point n() const { return Point{ x_origin + w / 2,y_origin }; }
	Point s() const { return Point{ x_origin + w / 2,y_origin + h }; }
	Point e() const { return Point{ x_origin + w,y_origin + h/2 }; }
	Point o() const { return Point{ x_origin,y_origin + h / 2 }; }
	Point c() const { return Point{ x_origin + w / 2,y_origin + h / 2 }; }
	Point ne() const { return Point{ x_origin + w ,y_origin }; }
	Point se() const { return Point{ x_origin + w ,y_origin+h }; }
	Point so() const { return Point{ x_origin ,y_origin+h }; }
	Point no() const { return Point{ x_origin ,y_origin }; }

	// Fonctions rajoutées pour l'exo 5 page 516, afin de pouvoir initialiser les valeurs h et w provenant de la classe dérivée
	void set_major(int ww) noexcept { w = ww; }
	void set_minor(int hh) noexcept { h = hh; }
	void set_x_origin(int xo) noexcept { x_origin = xo; }
	void set_y_origin(int yo) noexcept { y_origin = yo; }
	void add(Point p) { Shape::add(p); }						// Rajouté pour l'exo 4 page 579 : sinon impossible d'ajouter un point car se réfère à la fonction protected de shape
	void update_origine(Point p) noexcept { set_point(0, p); }	// Rajouté également mais seulement pour modifier le point origine (haut - gauche) précédemment créé

private:
	int h{};			// height
	int w{};			// width

	// Rajout du point "origine" (=coin haut-gauche) à l'exercice n°4 page 438
	int x_origin{};
	int y_origin{};
	
//	Color fcolor;	// fill color; 0 means "no fill"
};


struct Striped_rectangle : Rectangle {		// Exo 5 page 516

	Striped_rectangle(Point xy, int ww, int hh) :Rectangle{ xy,ww,hh } { }	// Constructeur : on appelle simplement le constructeur du Rectangle en passant les arguments

	void draw_lines() const override;
};


struct Arc : Shape {				// Rajout : exo 1 page 484 - db et de définissent l'arc à dessiner *********************************
	Arc(Point p, int ww, int hh, int deg_begin, int deg_end) :w{ ww }, h{ hh }, db{ deg_begin }, de{ deg_end }
	{
		add(Point{ p.x - ww, p.y - hh });
	}

	void draw_lines() const override;

	Point center() const { return{ point(0).x + w, point(0).y + h }; }
	Point focus1() const { return{ center().x + int(sqrt(double(w*w - h * h))), center().y }; }
	Point focus2() const { return{ center().x - int(sqrt(double(w*w - h * h))), center().y }; }

	void set_major(int ww) noexcept { w = ww; }
	int major() const noexcept { return w; }
	void set_minor(int hh) noexcept { h = hh; }
	int minor() const noexcept { return h; }

private:
	int w{};
	int h{};
	int db{};		// Début de l'arc à dessiner en degrés (ex : 90°)
	int de{};		// Fin de l'arc à dessiner en degrés (ex : 270°)
};


struct Box : Rectangle {		// Exo 2 page 484 (rounded box) : on dérive la classe Rectangle pour utiliser son Constructeur (on appelle simplement le constructeur du Rectangle en passant les arguments)

	Box(Point xy, int ww, int hh, const string& s) :Rectangle{ xy,ww,hh }, lab{ s } 
	{ 
		// Création des 4 côtés droits
		add_sides(new Line{ Point{ xy.x + 20,xy.y }, Point{ xy.x + ww - 20, xy.y } });
		add_sides(new Line{ Point{ xy.x + 20,xy.y+hh }, Point{ xy.x + ww - 20, xy.y+hh } });
		add_sides(new Line{ Point{ xy.x,xy.y + 20 }, Point{ xy.x , xy.y + hh-20 } });
		add_sides(new Line{ Point{ xy.x + ww,xy.y + 20 }, Point{ xy.x + ww, xy.y + hh-20 } });
		  
		// Création des 4 arrondis
		add_sides(new Arc{ Point{ xy.x+20,xy.y+20 }, 20, 20, 90, 180 });				// arc haut-gauche
		add_sides(new Arc{ Point{ xy.x + ww -20,xy.y+20 },20, 20, 0, 90 });				// arc haut-droit
		add_sides(new Arc{ Point{ xy.x+20,xy.y+hh-20 },20, 20, 180, 270 });				// arc bas-gauche
		add_sides(new Arc{ Point{ xy.x + ww - 20,xy.y + hh -20 }, 20, 20, 270, 0 });	// arc bas-droit
		
	}		
	
	~Box()	// Destructeur
	{
		
		for (auto p : sides)
			delete p;
	}

	void add_sides(Shape* s) { sides.push_back(s); }	// Création des 8 côtés
	void draw_lines() const override;

private:
	string lab{};				// Label éventuel
	vector<Shape*>sides{};	// Côtés de la rounded box
			
};


struct Arrow : Shape {		// Rajout : exo 3 page 484 ************************************

	Arrow(Point p1, Point p2, bool left_arrow, bool right_arrow) :la{ left_arrow }, ra{ right_arrow }
	{

		// On crée d'abord le trait...
		add_traits(new Line{ p1,p2 });
		
		// On détermine si le trait est horizontale ou vertical : cela va induire la manière de tracer la flèche
		bool horizontal = false;		// Par défaut, on considère que la flèche est verticale
		if (p1.y == p2.y) horizontal = true;

		// ... puis on trace les flèches gauche et droite si elles sont demandées
		// NB : le tracé de ces flèches est correct que si la droite est horizontale ou verticale - Amélioration : tracer en fonction de l'angle
		if (la) {
			if (horizontal) {
				add_traits(new Line{ p1,Point{ p1.x+10,p1.y-10 } });
				add_traits(new Line{ p1,Point{ p1.x + 10,p1.y + 10 } });
				
			}
			else
			{
				add_traits(new Line{ p1,Point{ p1.x - 10,p1.y + 10 } });
				add_traits(new Line{ p1,Point{ p1.x + 10,p1.y + 10 } });
				
			}


		}
		if (ra) {
			if (horizontal) {
				add_traits(new Line{ p2,Point{ p2.x - 10,p2.y - 10 } });
				add_traits(new Line{ p2,Point{ p2.x - 10,p2.y + 10 } });
			
			}
			else
			{
				add_traits(new Line{ p2,Point{ p2.x - 10,p2.y - 10 } });
				add_traits(new Line{ p2,Point{ p2.x + 10,p2.y - 10 } });
				
			}
		}

	}

	~Arrow()	// Destructeur
	{

		for (auto p : traits)
			delete p;
	}

	void draw_lines() const override;
	void add_traits(Shape* s) { traits.push_back(s); }	// Création des 3 traits

private:
	int la{};					// left arrow
	int ra{};					// right arrow
	vector<Shape*>traits{};	// Les 3 traits	

};


struct Circle : Shape {

	Circle() noexcept { }			// Constructeur par défaut (utile pour les classes dérivées, telles que Smiley ou Frowny)
	Circle(Point p, int rr)	// center and radius
		:r{ rr } {
		add(Point{ p.x - r, p.y - r });
	}

	void draw_lines() const override;

	Point center() const { return { point(0).x + r, point(0).y + r }; }

	void set_radius(int rr) noexcept { r = rr; }
	void set_center(Point p) noexcept { add(Point{ p.x - r, p.y - r }); }			// Rajouté afin de créer le "point" central sans passer par le constructeur (exo 4 page 579) - le "add point" ne passe pas !
	void new_center(Point p) noexcept {set_point(0, Point{ p.x - r, p.y - r });}	// Rajouté également mais seulement pour modifier le centre précédemment créé

	int radius() const noexcept { return r; }
private:
	int r{};
};


bool intersect(Point p1, Point p2, Point p3, Point p4);


struct Open_polyline : Shape {	// open sequence of lines
	using Shape::Shape;
	void add(Point p) { Shape::add(p); }
	void draw_lines() const override;
};

struct Closed_polyline : Open_polyline {	// closed sequence of lines
	using Open_polyline::Open_polyline;
	void draw_lines() const override;
	
//	void add(Point p) { Shape::add(p); }
};

struct Polygon : Closed_polyline {	// closed sequence of non-intersecting lines
	using Closed_polyline::Closed_polyline;
	void add(Point p);
	void draw_lines() const override;
};

struct Regular_Hexagon : Closed_polyline {	// Exo 8 page 484 ***************************************
	//using Closed_polyline::Closed_polyline;	// comme un polygone, on dérive la classe Closed_polyline : using declaration est un raccourci pour utiliser les constructeurs de Closed_polyline. Pas utile ici ?
	Regular_Hexagon() noexcept { }			// Constructeur par défaut (utile pour l'exercice 4 page 579)
	
	Regular_Hexagon(Point c, int a)			// on utilise les deux arguments centre et rayon
		:r{ a }								// on construit le rayon
	{
		// "add" des 6 points de l'hexagone régulier que l'on détermine grâce au rayon passé pour les deux points extrêmes du segment et des hauteurs calculées pour les 4 autres
		const int demi_r = static_cast<int>(round(r / 2));
		const int hauteur = static_cast<int>(round(r*sqrt(3) / 2));
		add(Point{ c.x - r, c.y});					// Point A
		add(Point{ c.x - demi_r, c.y - hauteur });	// Point B
		add(Point{ c.x + demi_r, c.y - hauteur });	// Point C
		add(Point{ c.x + r, c.y });					// Point D
		add(Point{ c.x + demi_r, c.y + hauteur });	// Point E
		add(Point{ c.x - demi_r, c.y + hauteur });	// Point F
			
	}
	void draw_lines() const override;				// on override la fonction définie dans shape pour tracer les 6 côtés

	
private:
	int r{};
};

struct Regular_octogon : Closed_polyline {	// exo 8 page 516
	
	Regular_octogon(Point c, int a)			// on utilise les deux arguments centre et rayon
		:r{ a }								// on construit le rayon
	{
		// "add" des 8 points de l'hexagone régulier que l'on détermine en balayant le cercle
		for (double i = 0; i < 2*PI; i += PI/4)	
		{

			const int x_polaire = static_cast<int>(round(c.x + r * cos(i)));
			const int y_polaire = static_cast<int>(round(c.y + r * sin(i)));
			add(Point{ x_polaire,y_polaire });

		}
	
	}
	void draw_lines() const override;				// on override la fonction définie dans shape pour tracer les 6 côtés

private:
	int r{};

};

struct Triangle_Rectangle : Closed_polyline {	// Exo 14 page 485 ***************************************
	//using Closed_polyline::Closed_polyline;	// comme un polygone, on dérive la classe Closed_polyline
	Triangle_Rectangle() noexcept { }			// Constructeur par défaut (utile pour l'exercice 4 page 579)
	
	Triangle_Rectangle(Point c, int width_x,int width_y)		// on utilise les deux arguments que sont les coordonnées de l'angle droit du triangle et les longueurs des deux cotés
		:wx{ width_x },	wy {width_y}							// on construit les longueurs des deux cotés : l'utilisation des valeurs négatives oriente le tracé dans le plan
	{
		// "add" des 3 points du triangle
		add(Point{ c.x, c.y });				// Point A (angle droit)
		add(Point{ c.x +wx, c.y });			// Point B
		add(Point{ c.x , c.y +wy });		// Point C

	}
	void draw_lines() const override;								// on déclare la classe-membre qui va tracer les 6 côtés
	void update_point(int i,Point p) noexcept { set_point(i, p); }	// Rajouté pour l'exercice 4 page 579 : on modifie les 3 points définissant le triangle rectangle

private:
	int wx{};
	int wy{};
};

struct Etoile : Closed_polyline {	// Exo 19 page 485 ***************************************
	//using Closed_polyline::Closed_polyline;	// comme un polygone, on dérive la classe Closed_polyline
	Etoile(Point c, int a)						// on utilise les deux arguments centre et rayon
		:r{ a }									// on construit le rayon
	{
		// "add" des 5 points du pentagone en utilisant la construction de Ptolémée
		// On décompose bien les opérations pour clarifier le code
		// Attention : LES ANGLES DOIVENT ETRE PASSES EN RADIANS AUX FONCTIONS MATHEMATIQUES DE LA STL !!!!!
				
		// Point A
		add(Point{ c.x + r, c.y });	
		
		// Point B
		CONST double conv_deg= PI /180;

		const int x_b = static_cast<int>(round(c.x + r * cos(144* conv_deg)));
		const int y_b = static_cast<int>(round(c.y - r * sin(144* conv_deg)));
		add(Point{ x_b, y_b });	
		
		// Point C
		const int x_c = static_cast<int>(round(c.x + r * cos(72* conv_deg)));
		const int y_c = static_cast<int>(round(c.y + r * sin(72* conv_deg)));
		add(Point{ x_c, y_c });
		
		// Point D
		const int x_d = static_cast<int>(round(c.x + r * cos(72* conv_deg)));
		const int y_d = static_cast<int>(round(c.y - r * sin(72* conv_deg)));
		add(Point{ x_d, y_d });
		
		// Point E
		const int x_e = static_cast<int>(round(c.x + r * cos(144* conv_deg)));
		const int y_e = static_cast<int>(round(c.y + r * sin(144* conv_deg)));
		add(Point{ x_e, y_e });
		

	}
	void draw_lines() const override;				// on déclare la classe-membre qui va tracer les 6 côtés

private:
	int r{};
};

struct Lines : Shape {	// independent lines
	Lines() noexcept {}
	Lines(initializer_list<Point> lst) : Shape{lst} { if (lst.size() % 2) error("odd number of points for Lines"); }
	void draw_lines() const override;
	void add(Point p1, Point p2) { Shape::add(p1); Shape::add(p2); }
};

struct Text : Shape {
	
	Text() noexcept {}	// Rajout dans le cadre de l'exo 7 page 548

	// the point is the bottom left of the first letter
	Text(Point x, const string& s) : lab{ s } { add(x); }

	void draw_lines() const override;

	void set_label(const string& s) { lab = s; }
	string label() const { return lab; }

	void set_font(Font f) noexcept { fnt = f; }
	Font font() const noexcept { return Font(fnt); }

	void set_font_size(int s) noexcept { fnt_sz = s; }
	int font_size() const noexcept { return fnt_sz; }

	void add(Point p) { Shape::add(p); }						// Rajouté pour l'exo 6 page 579 : sinon impossible d'ajouter un point car se réfère à la fonction protected de shape

private:
	string lab{};	// label
	Font fnt{ fl_font() };
	int fnt_sz{ (14<fl_size()) ? fl_size() : 14 };	// at least 14 point
};


struct Axis : Shape {
	// representation left public
	enum Orientation { x, y, z };
	Axis(Orientation d, Point xy, int length, int nummber_of_notches=0, string label = "");

	void draw_lines() const override;
	void move(int dx, int dy) override;

	void set_color(Color c);

	Text label;
	Lines notches;
//	Orientation orin;
//	int notches;
};



struct Immobile_Circle : Circle {					// Exo 4 page 516 : Circle cannot be moved

	Immobile_Circle(Point p, int r):Circle{p,r} { }	// On appelle simplement le constructeur de Circle en lui passant les arguments p et r (A tour of C++, page 44)

	void move(int dx, int dy) noexcept override { }					// On override la fonction qui se trouve dans shape... en ne faisant rien
	
};


struct Striped_circle : Circle {					// Exo 6 page 516

	Striped_circle(Point p, int r) :Circle{ p,r } { }	// On appelle simplement le constructeur de Circle en lui passant les arguments 

	void draw_lines() const override;

};


struct Smiley : Circle {		// Exo 1 page 516 (solution initialisée grâce à "A tour of C++", page 44)
								
	Smiley(Point p, int r):Circle{ p,r }, mouth{nullptr} {				// Constructeur
	
		add_eye(new Circle{ Point(p.x - r / 4, p.y - r / 4),r / 8 });
		add_eye(new Circle{ Point(p.x + r / 4, p.y - r / 4),r / 8 });
		this->set_mouth(new Circle{ Point(p.x, p.y + r / 4),r / 12 });	// On se sert de this pour désigner l'objet que l'on crée, en lui affectant une bouche via le membre "set_mouth"
		
	}
	
	~Smiley()	// Destructeur
	{ 
		delete mouth;
		for(auto p:eyes)
			delete p;
	}
		
	void draw_lines() const override;						// Overriding de la fonction draw_lines() définie dans shape
	void add_eye(Shape* s) { eyes.push_back(s); }	// Création des yeux
	void set_mouth(Shape* s) noexcept { mouth = s; };		// Création de la bouche

	//void rotate(int);
	//void move(Point to);
	//virtual void wink(int i);
		
private:
	vector<Shape*>eyes{};
	Shape* mouth{};
};


struct Ellipse : Shape {
	Ellipse(Point p, int ww, int hh)	// center, min, and max distance from center
	:w{ ww }, h{ hh } {
		add(Point{ p.x - ww, p.y - hh });
	}

	void draw_lines() const override;

	Point center() const { return{ point(0).x + w, point(0).y + h }; }
	Point focus1() const { return{ center().x + int(sqrt(double(w*w - h*h))), center().y }; }
	Point focus2() const { return{ center().x - int(sqrt(double(w*w - h*h))), center().y }; }
	
	void set_major(int ww) noexcept { w=ww; }
	int major() const noexcept { return w; }
	void set_minor(int hh) noexcept { h=hh; }
	int minor() const noexcept { return h; }

private:
	int w{};
	int h{};
};




/*
struct Mark : Text {
	static const int dw = 4;
	static const int dh = 4;
	Mark(Point xy, char c) : Text(Point(xy.x-dw, xy.y+dh),string(1,c)) {}
};
*/

struct Marked_polyline : Open_polyline {
	Marked_polyline(const string& m) :mark(m) { }
	void draw_lines() const override;
private:
	string mark{};
};

struct Marks : Marked_polyline {
	Marks(const string& m) :Marked_polyline(m)
	{ set_color(Color(Color::invisible)); }
};

struct Mark : Marks {
	Mark(Point xy, char c) : Marks(string(1,c)) {add(xy); }
};

/*

struct Marks : Shape {
	Marks(char m) : mark(string(1,m)) { }
	void add(Point p) { Shape::add(p); }
	void draw_lines() const;
private:
	string mark;
};
*/

struct Bad_image : Fl_Image {
	Bad_image(int h, int w) : Fl_Image(h,w,0) { }
	void draw(int x,int y, int, int, int, int) { draw_empty(x,y); }
};

struct Suffix {
	enum Encoding { none, jpg, gif, bmp };
};

Suffix::Encoding get_encoding(const string& s);

struct Image : Shape {
	Image(Point xy, string s, Suffix::Encoding e = Suffix::none);
	~Image() { delete p; }
	void draw_lines() const override;
	void set_mask(Point xy, int ww, int hh) noexcept { w=ww; h=hh; cx=xy.x; cy=xy.y; }
	void move(int dx,int dy) override { Shape::move(dx,dy); p->draw(point(0).x,point(0).y); }
private:
	int w,h,cx,cy; // define "masking box" within image relative to position (cx,cy)
	Fl_Image* p;
	Text fn;
};


struct Bar_graph : Shape {				// Exos 6 & 7 page 548

	Bar_graph(vector <int>& v, vector <string>& l,int xmax, int ymax, string la, string lb) : n{ v.size()}, win_width{ xmax }, win_height{ ymax }, lab_xa{la}, lab_ya{lb}
	{
		
		
		// Invariants
		if (n <= 0) error("aucune donnée : tracé du bar graph impossible");
		if (static_cast<int>(n*(bar_width + bar_space)+left_space+right_space) >= win_width) error("trop de données versus la taille de la fenètre : tracé du bar graph impossible");
		
		// On détermine l'échelle verticale en fonction de la plus haute valeur trouvée dans le vecteur
		vector<int>::iterator result = max_element(v.begin(), v.end());
		double const maxValue = *result;
		double const hauteur_utile = win_height - top_space - bottom_space;
		double const y_scale = hauteur_utile / maxValue;	

		// Création des objets représentant les barres et les labels du graphe
		for (unsigned int i = 0; i<n; ++i) {
			
			int const bar_height = static_cast<int>(v[i] * y_scale);
			int const bar_xpos = left_space + (bar_width + bar_space)*i;
			int const bar_ypos = top_space + static_cast<int>(hauteur_utile) - bar_height;	// Le tracé d'un rectangle "va" toujours vers le bas au contraire d'un bar graph qui va vers le haut -> Cette variable trouve la position de départ
			
			add_barre(new Rectangle{ Point(bar_xpos,bar_ypos),bar_width,bar_height });	// L'objet "barre"
			add_value(new Text{ Point(bar_xpos-5,bar_ypos-5),to_string(v[i]) });		// Les valeurs
			add_label(new Text{ Point(bar_xpos +5,bar_ypos +30),l[i] });				// Les labels correspondant aux légendes passées dans le fichier
		}
	
	}

	void add_barre(Rectangle* s) { barres.push_back(s); }	// Création des barres
	void add_value(Text* s) { values.push_back(s); }		// Création des valeurs
	void add_label(Text* s) { labels.push_back(s); }		// Création des labels
	void draw_lines() const override;

	~Bar_graph()	// Destructeur
	{

		for (auto p : barres)
			delete p;

		for (auto p : values)
			delete p;

		for (auto p : labels)
			delete p;

	}

private:
	unsigned int n{};					// Nombre de valeurs à tracer sous forme de barres
	vector<Rectangle*>barres{};			// Les barres à tracer : on stocke des RECTANGLES et pas des SHAPE afin de pouvoir manipuler leurs propriétés dans draw_lines()
	vector<Text*>values{};				// Les valeurs à écrire : on stocke des TEXT et pas des SHAPE afin de pouvoir manipuler leurs propriétés dans draw_lines()
	vector<Text*>labels{};				// Les labels (= légendes des valeurs)
	int win_width{};					// Largeur maximale de la fenètre
	int win_height{};					// Hauteur maximale de la fenètre
	string lab_xa{};					// Label de l'axe x
	string lab_ya{};					// Label de l'axe y

};


struct Binary_tree : Shape {		// Rajout : exo 11 page 517 ************************************
	enum Node_type {
		rond = 0,
		markx = 1,
		carre = 2,

	};

	Binary_tree(Point root, int number_levels, Node_type node_shape) :n{ number_levels }, nd{ node_shape } // On construit n et nd (privé) 
	{

		if (n>0 && n<7)				// Invariant : il faut a minima un noeud (root) et pas plus de 6 pour "rester dans la fenêtre"
		{
			add(root);				// on rajoute le point (root) en utilisant le constructeur de la classe de base "shape"

			// Détermination du nombre de noeuds par niveau
			vector <int> nb_noeuds_niveau_n(n + 1);
			nb_noeuds_niveau_n[0] = 0;			// pas d'arbre
			nb_noeuds_niveau_n[1] = 1;			// root
			for (int niv = 1; niv <= n; ++niv)	// autres niveaux de l'arborescence
				if (niv>1) nb_noeuds_niveau_n[niv] = nb_noeuds_niveau_n[niv - 1] * 2;

			// Création des noeuds en partant de la fin pour bien les positionner sur la fenêtre
			const int y_origine = 100;
			int espacement = 30;
			int distance_bord = 15;

			for (int niv = n; niv >0; --niv)
			{
				for (int i = 0; i<nb_noeuds_niveau_n[niv]; ++i) {
					const int x_origine = 100 + distance_bord;
					const int coord_x = x_origine + i * espacement;
					const int coord_y = y_origine + niv * 50;
					//add_noeuds(new Circle{ Point(coord_x,coord_y),5 }); // Insertion des noeuds : v1 (container vecteur) et v2 (container multimap)
					switch (nd)	// Exo 12 page 517
					{
					case rond:
						add_noeuds(niv, new Circle{ Point(coord_x,coord_y),5 });
						break;
					case markx:
						add_noeuds(niv, new Mark{ Point(coord_x,coord_y),'x' });
						break;
					case carre:
						add_noeuds(niv, new Rectangle{ Point(coord_x,coord_y),7,7 });
						break;
					default:
						break;
					}

				}
				espacement *= 2;	// Variables de positionnement sur la fenêtre
				distance_bord *= 2;
			}

			/*
			// Création de liaisons entre les noeuds de chaque niveau (objet "line") : le niveau 1 (root) n'est relié à aucun point ("vers le haut") donc la boucle est bien niv>1

			// Version 1 :
			// - on gère le "déplacement" dans les noeuds via 2 variables de position : offset et noeuds_visites
			int noeuds_visites = 0;
			for (int niv = n; niv >1; --niv)
			{

			for (int i = 0; i<nb_noeuds_niveau_n[niv]; i+=2) {

			// Détermination du noeud "supérieur"
			int offset = noeuds_visites+nb_noeuds_niveau_n[niv];
			int x_sup = noeuds[offset + i/2]->point(0).x;
			int y_sup = noeuds[offset + i/2]->point(0).y;

			// Détermination des deux noeuds "inférieurs" à relier
			int x = noeuds[noeuds_visites +i]->point(0).x;
			int y = noeuds[noeuds_visites +i]->point(0).y;
			int x1 = noeuds[noeuds_visites +i+1]->point(0).x;
			int y1 = noeuds[noeuds_visites +i + 1]->point(0).y;

			// On crée les deux liaisons vers le noeud supérieur
			add_liaisons(new Line{ Point{ x,y },Point{ x_sup,y_sup } });
			add_liaisons(new Line{ Point{ x1,y1 },Point{ x_sup,y_sup } });

			}
			noeuds_visites += nb_noeuds_niveau_n[niv];
			}
			*/

			// Version 2 :
			// - l'utilisation d'un algorithme de la STL (i.e. : multimap) rend le code plus lisible et maintenable, en supprimant les variables de position remplacées par des itérateurs sur "range"
			// - le code n'est pas plus compact mais il est plus "safe" : impossible d'être "outbound" avec des itérateurs
			for (int niv = n; niv >1; --niv)
			{
				auto noeuds_niveau_n = noeuds.equal_range(niv);						// On détermine la plage des noeuds du niveau n (inférieur)

				auto noeuds_niveau_n_sup = noeuds.equal_range(niv - 1);				// On détermine la plage des noeuds du niveau n-1 (supérieur)
				auto j = noeuds_niveau_n_sup.first;									// On crée l'itérateur

				for (auto i = noeuds_niveau_n.first; i != noeuds_niveau_n.second;)	// L'incrément de boucle se fait à l'intérieur
				{

					// Récupération des coordonnées du noeud supérieur
					const int x_sup = j->second->point(0).x;
					const int y_sup = j->second->point(0).y;

					// Récupération des coordonnées des deux noeuds "inférieurs" à relier
					const int x = i->second->point(0).x;
					const int y = i->second->point(0).y;
					++i;	// On avance dans le range des noeuds "inférieurs" (1ère fois)
					const int x1 = i->second->point(0).x;
					const int y1 = i->second->point(0).y;

					// On crée les deux liaisons vers le noeud supérieur
					add_liaisons(new Line{ Point{ x,y },Point{ x_sup,y_sup } });
					add_liaisons(new Line{ Point{ x1,y1 },Point{ x_sup,y_sup } });

					++j;	// On avance dans le range des noeuds "supérieurs"												
					++i;	// On avance dans le range des noeuds "inférieurs" (2ème fois)
				}

			}
		}
	}

	void draw_lines() const override;
	
	//void add_noeuds(Shape* s) { noeuds.push_back(s); }	// Création des noeuds : v1 (container vecteur) et v2 (container multimap)
	void add_noeuds(int niveau, Shape* s) { noeuds.insert(make_pair(niveau, s)); }
	
	void add_liaisons(Shape* s) { liaisons.push_back(s); }	// Création des liaisons

	void nommer_noeud(int niveau, int num_node, string text_node) 					// Exo 14 page 517
	{
		if (niveau<=n)																// Pas de nommage si le paramètre "sort" de l'arbre
		{
			auto range = noeuds.equal_range(niveau);								// On constitue le range du niveau demandé
			auto i = range.first;													// On positionne l'itérateur au début du range
			advance(i, num_node);													// On avance jusqu'au noeud souhaité, sachant que le range débute à 0
																					// Amélioration à apporter : tester que num_node ne dépasse pas le nombre de noeuds du niveau

			const int coord_x = i->second->point(0).x;									// On récupère les coordonnées
			const int coord_y = i->second->point(0).y;
		
			add_noeuds(niveau, new Text{ Point(coord_x,coord_y),text_node });		// On crée un "noeud" qui correspond dans ce cas à un nommage "texte" et pas à une forme => Réutilisation du mécanisme
																					// utilisé dans le constructeur
		}
	}

	~Binary_tree()	// Destructeur
	{

		for (auto p : noeuds)
			p.~pair();		// TC++ page 983
					
		for (auto p : liaisons)
			delete p;
	}

private:
	int n{};
	Node_type nd{};
	
	//vector<Shape*>noeuds;	// Déclaration des noeuds : v1 (container vecteur) et v2 (container multimap)
	multimap<int, Shape*> noeuds{};
	
	vector<Shape*>liaisons{};	// Les liaisons

};

}
#endif

