// Fichier modifi� suite aux exercices BS du chapitre 13

#ifndef GRAPH_GUARD
#define GRAPH_GUARD 1

#include "Point.h"
#include<vector>
#include "std_lib_facilities.h"
//#include<string>
//#include<cmath>
#include "fltk.h"

namespace Graph_lib {
// defense against ill-behaved Linux macros:
#undef major
#undef minor

struct Color {
	enum Color_type {
		red=FL_RED, blue=FL_BLUE, green=FL_GREEN,
		yellow=FL_YELLOW, white=FL_WHITE, black=FL_BLACK,
		magenta=FL_MAGENTA, cyan=FL_CYAN, dark_red=FL_DARK_RED,
		dark_green=FL_DARK_GREEN, dark_yellow=FL_DARK_YELLOW, dark_blue=FL_DARK_BLUE,
		dark_magenta=FL_DARK_MAGENTA, dark_cyan=FL_DARK_CYAN
	};
	enum Transparency { invisible = 0, visible=255 };

	Color(Color_type cc) :c(Fl_Color(cc)), v(visible) { }
	Color(Color_type cc, Transparency vv) :c(Fl_Color(cc)), v(vv) { }
	Color(int cc) :c(Fl_Color(cc)), v(visible) { }
	Color(Transparency vv) :c(Fl_Color()), v(vv) { }

	int as_int() const { return c; }
	char visibility() const { return v; }
	void set_visibility(Transparency vv) { v=vv; }
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
	Line_style(Line_style_type ss) :s(ss), w(0) { }
	Line_style(Line_style_type lst, int ww) :s(lst), w(ww) { }
	Line_style(int ss) :s(ss), w(0) { }

	int width() const { return w; }
	int style() const { return s; }
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

	Font(Font_type ff) :f(ff) { }
	Font(int ff) :f(ff) { }

	int as_int() const { return f; }
private:
	int f;
};

template<class T> class Vector_ref {
	vector<T*> v;
	vector<T*> owned;
public:
	Vector_ref() {}

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
	int size() const { return v.size(); }
};

typedef double Fct(double);

class Shape  {	// deals with color and style, and holds sequence of lines
protected:
	Shape() { }
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

	void set_color(Color col) { lcolor = col; }
	Color color() const { return lcolor; }

	void set_style(Line_style sty) { ls = sty; }
	Line_style style() const { return ls; }

	void set_fill_color(Color col) { fcolor = col; }
	Color fill_color() const { return fcolor; }

	Point point(int i) const { return points[i]; }
	int number_of_points() const { return int(points.size()); }

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

struct Function : Shape {
	// the function parameters are not stored
	Function(Fct f, double r1, double r2, Point orig, int count = 100, double xscale = 25, double yscale = 25);
	//Function(Point orig, Fct f, double r1, double r2, int count, double xscale = 1, double yscale = 1);	
};

struct Fill {
	Fill() :no_fill(true), fcolor(0) { }
	Fill(Color c) :no_fill(false), fcolor(c) { }

	void set_fill_color(Color col) { fcolor = col; }
	Color fill_color() { return fcolor; }
protected:
	bool no_fill;
	Color fcolor;
};

struct Line : Shape {
	Line(Point p1, Point p2) { add(p1); add(p2); }
};

struct Rectangle : Shape {
	Rectangle() { }			// Constructeur par d�faut (utile pour les classes d�riv�es, telles que Striped_rectangle...)
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
	void draw_lines() const;

//	void set_fill_color(Color col) { fcolor = col; }
//	Color fill_color() { return fcolor; }

	int height() const { return h; }
	int width() const { return w; }

	// Rajout des fonctions demand�es � l'exercice n�4 page 438
	Point n() const { return Point{ x_origin + w / 2,y_origin }; }
	Point s() const { return Point{ x_origin + w / 2,y_origin + h }; }
	Point e() const { return Point{ x_origin + w,y_origin + h/2 }; }
	Point o() const { return Point{ x_origin,y_origin + h / 2 }; }
	Point c() const { return Point{ x_origin + w / 2,y_origin + h / 2 }; }
	Point ne() const { return Point{ x_origin + w ,y_origin }; }
	Point se() const { return Point{ x_origin + w ,y_origin+h }; }
	Point so() const { return Point{ x_origin ,y_origin+h }; }
	Point no() const { return Point{ x_origin ,y_origin }; }

	// Fonctions rajout�es pour l'exo 5 page 516, afin de pouvoir initialiser les valeurs h et w provenant de la classe d�riv�e
	void set_major(int ww) { w = ww; }
	void set_minor(int hh) { h = hh; }
	void set_x_origin(int xo) { x_origin = xo; }
	void set_y_origin(int yo) { y_origin = yo; }


private:
	int h;			// height
	int w;			// width

	// Rajout du point "origine" (=coin haut-gauche) � l'exercice n�4 page 438
	int x_origin;
	int y_origin;
	
	
//	Color fcolor;	// fill color; 0 means "no fill"
};

struct Striped_rectangle : Rectangle {		// Exo 5 page 516

	Striped_rectangle(Point xy, int ww, int hh)		// Constructeur : on "set" les valeurs indispensables au bon tra�age du rectangle (classe de base)
	{
		add(xy);
		set_major(ww);
		set_minor(hh);
		set_x_origin(xy.x);
		set_y_origin(xy.y);
	}

	void draw_lines() const;

};

struct Box : Shape {		// Rajout : exos 2&6 page 484 - On repart de la structure du rectangle, 
							// ce qui permet de r�utiliser les constructeurs et ses v�rifications ************************************

	Box(Point xy, int ww, int hh, const string& s) :w{ ww }, h{ hh }, lab{ s }
	{
		if (h <= 0 || w <= 0) error("Bad box: non-positive side");
		add(xy);
	}
	Box(Point x, Point y) :w{ y.x - x.x }, h{ y.y - x.y }
	{
		if (h <= 0 || w <= 0) error("Bad box: first point is not top left");
		add(x);
	}
	void draw_lines() const;

	//	void set_fill_color(Color col) { fcolor = col; }
	//	Color fill_color() { return fcolor; }

	int height() const { return h; }
	int width() const { return w; }

private:
	int h;					// height
	int w;					// width
	
	// Data du label
	string lab;				// texte
	Font fnt{ fl_font() };	// fonte
	int fnt_sz{ (14<fl_size()) ? fl_size() : 14 };	// taille : at least 14 point				
					
					
					//	Color fcolor;	// fill color; 0 means "no fill"
};


struct Arrow : Shape {		// Rajout : exo 3 page 484 ************************************

	Arrow(Point p1, Point p2, bool left_arrow, bool right_arrow) :la{ left_arrow }, ra{ right_arrow }
	{
		
		add(p1);
		add(p2);
	}
	
	void draw_lines() const;

	
private:
	int la;			// left arrow
	int ra;			// right arrow
					
};

bool intersect(Point p1, Point p2, Point p3, Point p4);


struct Open_polyline : Shape {	// open sequence of lines
	using Shape::Shape;
	void add(Point p) { Shape::add(p); }
	void draw_lines() const;
};

struct Closed_polyline : Open_polyline {	// closed sequence of lines
	using Open_polyline::Open_polyline;
	void draw_lines() const;
	
//	void add(Point p) { Shape::add(p); }
};

struct Polygon : Closed_polyline {	// closed sequence of non-intersecting lines
	using Closed_polyline::Closed_polyline;
	void add(Point p);
	void draw_lines() const;
};

struct Regular_Hexagon : Closed_polyline {	// Exo 8 page 484 ***************************************
	using Closed_polyline::Closed_polyline;	// comme un polygone, on d�rive la classe Closed_polyline
	Regular_Hexagon(Point c, int a)			// on utilise les deux arguments centre et rayon
		:r{ a }								// on construit le rayon
	{
		// "add" des 6 points de l'hexagone r�gulier que l'on d�termine gr�ce au rayon pass� pour les deux points extr�mes du segment et des hauteurs calcul�es pour les 4 autres
		int demi_r = static_cast<int>(round(r / 2));
		int hauteur = static_cast<int>(round(r*sqrt(3) / 2));
		add(Point{ c.x - r, c.y});					// Point A
		add(Point{ c.x - demi_r, c.y - hauteur });	// Point B
		add(Point{ c.x + demi_r, c.y - hauteur });	// Point C
		add(Point{ c.x + r, c.y });					// Point D
		add(Point{ c.x + demi_r, c.y + hauteur });	// Point E
		add(Point{ c.x - demi_r, c.y + hauteur });	// Point F
			
	}
	void draw_lines() const;				// on d�clare la classe-membre qui va tracer les 6 c�t�s

private:
	int r;
};

struct Triangle_Rectangle : Closed_polyline {	// Exo 14 page 485 ***************************************
	using Closed_polyline::Closed_polyline;	// comme un polygone, on d�rive la classe Closed_polyline
	Triangle_Rectangle(Point c, int width_x,int width_y)		// on utilise les deux arguments que sont les coordonn�es de l'angle droit du triangle et les longueurs des deux cot�s
		:wx{ width_x },	wy {width_y}							// on construit les longueurs des deux cot�s : l'utilisation des valeurs n�gatives oriente le trac� dans le plan
	{
		// "add" des 3 points du triangle
		add(Point{ c.x, c.y });				// Point A (angle droit)
		add(Point{ c.x +wx, c.y });			// Point B
		add(Point{ c.x , c.y +wy });		// Point C

	}
	void draw_lines() const;				// on d�clare la classe-membre qui va tracer les 6 c�t�s

private:
	int wx;
	int wy;
};

struct Etoile : Closed_polyline {	// Exo 19 page 485 ***************************************
	using Closed_polyline::Closed_polyline;	// comme un polygone, on d�rive la classe Closed_polyline
	Etoile(Point c, int a)					// on utilise les deux arguments centre et rayon
		:r{ a }								// on construit le rayon
	{
		// "add" des 5 points du pentagone en utilisant la construction de Ptol�m�e
		// On d�compose bien les op�rations pour clarifier le code
		// Attention : LES ANGLES DOIVENT ETRE PASSES EN RADIANS AUX FONCTIONS MATHEMATIQUES DE LA STL !!!!!
				
		// Point A
		add(Point{ c.x + r, c.y });	
		
		// Point B
		CONST double conv_deg= 3.1415926535897 /180;

		int x_b = static_cast<int>(round(c.x + r * cos(144* conv_deg)));
		int y_b = static_cast<int>(round(c.y - r * sin(144* conv_deg)));
		add(Point{ x_b, y_b });	
		
		// Point C
		int x_c = static_cast<int>(round(c.x + r * cos(72* conv_deg)));
		int y_c = static_cast<int>(round(c.y + r * sin(72* conv_deg)));
		add(Point{ x_c, y_c });
		
		// Point D
		int x_d = static_cast<int>(round(c.x + r * cos(72* conv_deg)));
		int y_d = static_cast<int>(round(c.y - r * sin(72* conv_deg)));
		add(Point{ x_d, y_d });
		
		// Point E
		int x_e = static_cast<int>(round(c.x + r * cos(144* conv_deg)));
		int y_e = static_cast<int>(round(c.y + r * sin(144* conv_deg)));
		add(Point{ x_e, y_e });
		

	}
	void draw_lines() const;				// on d�clare la classe-membre qui va tracer les 6 c�t�s

private:
	int r;
};

struct Lines : Shape {	// independent lines
	Lines() {}
	Lines(initializer_list<Point> lst) : Shape{lst} { if (lst.size() % 2) error("odd number of points for Lines"); }
	void draw_lines() const;
	void add(Point p1, Point p2) { Shape::add(p1); Shape::add(p2); }
};

struct Text : Shape {
	// the point is the bottom left of the first letter
	Text(Point x, const string& s) : lab{ s } { add(x); }

	void draw_lines() const;

	void set_label(const string& s) { lab = s; }
	string label() const { return lab; }

	void set_font(Font f) { fnt = f; }
	Font font() const { return Font(fnt); }

	void set_font_size(int s) { fnt_sz = s; }
	int font_size() const { return fnt_sz; }
private:
	string lab;	// label
	Font fnt{ fl_font() };
	int fnt_sz{ (14<fl_size()) ? fl_size() : 14 };	// at least 14 point
};


struct Axis : Shape {
	// representation left public
	enum Orientation { x, y, z };
	Axis(Orientation d, Point xy, int length, int nummber_of_notches=0, string label = "");

	void draw_lines() const;
	void move(int dx, int dy);

	void set_color(Color c);

	Text label;
	Lines notches;
//	Orientation orin;
//	int notches;
};

struct Circle : Shape {
	
	Circle() { }			// Constructeur par d�faut (utile pour les classes d�riv�es, telles que Smiley ou Frowny)
	Circle(Point p, int rr)	// center and radius
	:r{ rr } {
		add(Point{ p.x - r, p.y - r });
	}

	void draw_lines() const;

	Point center() const { return { point(0).x + r, point(0).y + r }; }

	void set_radius(int rr) { r=rr; }
	int radius() const { return r; }
private:
	int r;
};

struct Immobile_Circle : Circle {		// Exo 4 page 516 : Circle cannot be moved

	Immobile_Circle(Point p, int r):Circle{p,r} { }	// On appelle simplement le constructeur de Circle en lui passant les arguments p et rr (A tour of C++, page 44)

	void move(int dx, int dy) { }	// On override la fonction qui se trouve dans shape... en ne faisant rien
	
};

struct Smiley : Circle {		// Exo 1 page 516 (A tour of C++, page 44)
								
	Smiley(Point p, int r):Circle{ p,r }, mouth{nullptr} { 
	
		add_eye(new Circle{ Point(p.x - r / 4, p.y - r / 4),r / 8 });
		add_eye(new Circle{ Point(p.x + r / 4, p.y - r / 4),r / 8 });
		add_eye(new Circle{ Point(p.x, p.y + r / 4), r / 12 });		// Trac� de la bouche pour le moment... le set_mouth est � impl�menter
	}
	
	~Smiley()	// Destructeur
	{ 
		delete mouth;
		for(auto p:eyes)
			delete p;
	}
		
	void draw_lines() const;
	void add_eye(Shape* s) { eyes.push_back(s); }
	
	//void rotate(int);
	//void move(Point to);
	void set_mouth(Shape* s);
	//virtual void wink(int i);

private:
	vector<Shape*>eyes;
	Shape* mouth;
};


struct Ellipse : Shape {
	Ellipse(Point p, int ww, int hh)	// center, min, and max distance from center
	:w{ ww }, h{ hh } {
		add(Point{ p.x - ww, p.y - hh });
	}

	void draw_lines() const;

	Point center() const { return{ point(0).x + w, point(0).y + h }; }
	Point focus1() const { return{ center().x + int(sqrt(double(w*w - h*h))), center().y }; }
	Point focus2() const { return{ center().x - int(sqrt(double(w*w - h*h))), center().y }; }
	
	void set_major(int ww) { w=ww; }
	int major() const { return w; }
	void set_minor(int hh) { h=hh; }
	int minor() const { return h; }
private:
	int w;
	int h;
};


struct Arc : Shape {				// Rajout : exo 1 page 484 - db et de d�finissent l'arc � dessiner *********************************
	Arc(Point p, int ww, int hh, int deg_begin, int deg_end)	
		:w{ ww }, h{ hh }, db{ deg_begin }, de{ deg_end } {
		add(Point{ p.x - ww, p.y - hh });
	}

	void draw_lines() const;

	Point center() const { return{ point(0).x + w, point(0).y + h }; }
	Point focus1() const { return{ center().x + int(sqrt(double(w*w - h * h))), center().y }; }
	Point focus2() const { return{ center().x - int(sqrt(double(w*w - h * h))), center().y }; }

	void set_major(int ww) { w = ww; }
	int major() const { return w; }
	void set_minor(int hh) { h = hh; }
	int minor() const { return h; }
private:
	int w;
	int h;
	int db;		// D�but de l'arc � dessiner en degr�s (ex : 90�)
	int de;		// Fin de l'arc � dessiner en degr�s (ex : 270�)
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
	void draw_lines() const;
private:
	string mark;
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
	void draw_lines() const;
	void set_mask(Point xy, int ww, int hh) { w=ww; h=hh; cx=xy.x; cy=xy.y; }
	void move(int dx,int dy) { Shape::move(dx,dy); p->draw(point(0).x,point(0).y); }
private:
	int w,h,cx,cy; // define "masking box" within image relative to position (cx,cy)
	Fl_Image* p;
	Text fn;
};

}
#endif

