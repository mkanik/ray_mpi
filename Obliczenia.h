#ifndef _OBLICZENIA_H
#define _OBLICZENIA_H

#include "math.h"
#include "Obiekty.h"

class Wektor {
	double x, y, z;
	
	public:
	
	Wektor ();
	
	Wektor (double, double, double);
	
	//metody
	double pobierzWektorX() { 
		return x; 
	}
	double pobierzWektorY() { 
		return y; 
	}
	double pobierzWektorZ() { 
		return z; 
	}
	
	double dlugoscWektora () {
		return sqrt((x*x) + (y*y) + (z*z)); 
	}
	
	Wektor normalizuj () {
		double dlugoscWektora = sqrt((x*x) + (y*y) + (z*z)); 
		return Wektor (x/dlugoscWektora, y/dlugoscWektora, z/dlugoscWektora);
	}
	
	Wektor odwroc () {
		return Wektor (-x, -y, -z);
	}
	
	//iloczyn skalarny
	double wektorIlSkalarny(Wektor w) {
		return x*w.pobierzWektorX() + y*w.pobierzWektorY() + z*w.pobierzWektorZ();
	}
	
	//iloczyn wektorowy
	Wektor wektorIlWektorowy(Wektor w) {
		return Wektor (y*w.pobierzWektorZ() - z*w.pobierzWektorY(), z*w.pobierzWektorX() - x*w.pobierzWektorZ(), x*w.pobierzWektorY() - y*w.pobierzWektorX());
	}
	
	Wektor wektorDodaj (Wektor w) {
		return Wektor (x + w.pobierzWektorX(), y + w.pobierzWektorY(), z + w.pobierzWektorZ());
	}

	Wektor wektorPomnoz (double skalar) {
		return Wektor (x*skalar, y*skalar, z*skalar);
	}
};

Wektor::Wektor () {
	x = 0;
	y = 0;
	z = 0;
}

Wektor::Wektor (double i, double j, double k) {
	x = i;
	y = j;
	z = k;
}


class Kolor {
	double red, green, blue, polysk;
	
	public:
	
	Kolor ();
	
	Kolor (double, double, double, double);
	
	//metody
	double pobierzKolorRed() { 
		return red; 
	}
	double pobierzKolorGreen() { 
		return green; 
	}
	double pobierzKolorBlue() { 
		return blue; 
	}
	double pobierzKolorPolysk() { 
		return polysk; 
	}
	
	double ustawKolorRed(double wartoscRed) { 
		red = wartoscRed; 
		return red; 
	};
	double ustawKolorGreen(double wartoscGreen) { 
		green = wartoscGreen; 
		return green; 
	};
	double ustawKolorBlue(double wartoscBlue) { 
		blue = wartoscBlue; 
		return blue; 
	};
	double ustawKolorPolysk(double wartoscPolysk) { 
		polysk = wartoscPolysk; 
		return polysk; 
	};
	
	double jasnosc() {
		return(red + green + blue)/3;
	}
	
	Kolor kolorSkalar(double skalar) {
		return Kolor (red*skalar, green*skalar, blue*skalar, polysk);
	}
	
	Kolor kolorDodaj(Kolor kolor) {
		return Kolor (red + kolor.pobierzKolorRed(), green + kolor.pobierzKolorGreen(), blue + kolor.pobierzKolorBlue(), polysk);
	}
	
	Kolor kolorPomnoz(Kolor kolor) {
		return Kolor (red*kolor.pobierzKolorRed(), green*kolor.pobierzKolorGreen(), blue*kolor.pobierzKolorBlue(), polysk);
	}
	
	Kolor kolorSrednia(Kolor kolor) {
		return Kolor ((red + kolor.pobierzKolorRed())/2, (green + kolor.pobierzKolorGreen())/2, (blue + kolor.pobierzKolorBlue())/2, polysk);
	}
	
	Kolor kolorPrzytnij() {
		double calosc_swiatla = red + green + blue;
		double nadmiar_swiatla = calosc_swiatla -3;
		if (nadmiar_swiatla > 0) {
			red = red + nadmiar_swiatla*(red/calosc_swiatla);
			green = green + nadmiar_swiatla*(green/calosc_swiatla);
			blue = blue + nadmiar_swiatla*(blue/calosc_swiatla);
		}
		if (red > 1) {
			red = 1;
		}
		if (green > 1) {
			green = 1;
		}
		if (blue > 1) {
			blue = 1;
		}
		if (red < 0) {
			red = 0;
		}
		if (green < 0) {
			green = 0;
		}
		if (blue < 0) {
			blue = 0;
		}
		
		return Kolor (red, green, blue, polysk);
	}
	
};

Kolor::Kolor () {
	red = 0.0;
	green = 0.0;
	blue = 0.0;
}

Kolor::Kolor (double r, double g, double b, double p) {
	red = r;
	green = g;
	blue = b;
	polysk = p;
}


class Promien {
	Wektor zrodlo, kierunek;
	
	public:
	
	Promien ();
	
	Promien (Wektor, Wektor);
	
	//metody
	Wektor pobierzZrodloPromienia () { 
		return zrodlo; 
	}
	Wektor pobierzKierunekPromienia () { 
		return kierunek; 
	}
	
};

//powołanie nowego wektora (defaultowo w kierunku x)
Promien::Promien () {
	zrodlo = Wektor(0,0,0);
	kierunek = Wektor(1,0,0);
}

Promien::Promien (Wektor z, Wektor k) {
	zrodlo = z;
	kierunek = k;
}


#endif
