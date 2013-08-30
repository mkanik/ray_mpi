#ifndef _OBIEKTY_H
#define _OBIEKTY_H

#include "math.h"
#include "Obliczenia.h"


class Obiekt {
	public:
	
	Obiekt ();
	
	//metody
	virtual Kolor pobierzKolor () { return Kolor (0.0, 0.0, 0.0, 0); }

	virtual Wektor pobierzZnormalizowanyW(Wektor pozycja_punktu_przeciecia){
		return Wektor (0, 0, 0);
	}

	virtual double znajdzPunktPrzeciecia(Promien promien) {
		return 0;
	}
};

//powołanie nowego obiektu
Obiekt::Obiekt () {}


class Zrodlo {
	public:
	
	Zrodlo();
	
	virtual Wektor pobierzPozycjeSwiatla() {return Wektor(0,0,0);}
	virtual Kolor pobierzKolorSwiatla() {return Kolor(1, 1, 1, 0);}
	
};

Zrodlo::Zrodlo() {}


class Plaszczyzna : public Obiekt {
	Wektor normal;
	double dystans;
	Kolor kolor;
	
	public:
	
	Plaszczyzna ();
	
	Plaszczyzna (Wektor, double, Kolor);
	
	//metody
	Wektor pobierzPlaszczyznaZnormalizowany () { return normal; }
	double pobierzPlaszczyznaDystans () { return dystans; }
	virtual Kolor pobierzKolor () { return kolor; }
	
	virtual Wektor pobierzZnormalizowanyW (Wektor point) {
		return normal;
	}
	
	virtual double znajdzPunktPrzeciecia(Promien promien){ //odleglosc promienia od punktu przeciecia
		Wektor kierunek_promienia = promien.pobierzKierunekPromienia();
		
		double a = kierunek_promienia.wektorIlSkalarny(normal);
		
		if (a == 0) {
			//promien jesr rownolegly do plaszczyzny
			return -1;
		}
		else {
			double b = normal.wektorIlSkalarny(promien.pobierzZrodloPromienia().wektorDodaj(normal.wektorPomnoz(dystans).odwroc()));
			return -1*b/a;
		}
	}
	
};

//powołanie nowej plaszczyzny (defaultowo w kierunku x)
Plaszczyzna::Plaszczyzna () {
	normal = Wektor(1,0,0); //defaultowy kierunek x
	dystans = 0;
	kolor = Kolor(0.5, 0.5, 0.5, 0);
}

Plaszczyzna::Plaszczyzna (Wektor normalValue, double plaszczyzna_dystans_wartosc, Kolor plaszczyzna_kolor_wartosc) {
	normal = normalValue;
	dystans = plaszczyzna_dystans_wartosc;
	kolor = plaszczyzna_kolor_wartosc;
}

class Sfera : public Obiekt {
	Wektor sfera_srodek;
	double radius;
	Kolor kolor;
	
	public:
	
	Sfera ();
	Sfera (Wektor, double, Kolor);
	
	//metody
	Wektor pobierzSrodekSfery () { return sfera_srodek; }
	double pobierzPromienSfery () { return radius; }
	virtual Kolor pobierzKolor () { return kolor; }
	virtual Wektor pobierzZnormalizowanyW(Wektor point) {
		// normal always points away from the sfera_srodek od a sphere
		Wektor normal_Wektor = point.wektorDodaj(sfera_srodek.odwroc()).normalizuj();
		return normal_Wektor;
	}
	virtual double znajdzPunktPrzeciecia(Promien ray) {
		Wektor ray_zrodlo = ray.pobierzZrodloPromienia();
		double promien_zrodlo_x = ray_zrodlo.pobierzWektorX();
		double promien_zrodlo_y = ray_zrodlo.pobierzWektorY();
		double promien_zrodlo_z = ray_zrodlo.pobierzWektorZ();
		
		Wektor kierunek_promienia = ray.pobierzKierunekPromienia();
		double kierunek_promienia_x = kierunek_promienia.pobierzWektorX();
		double kierunek_promienia_y = kierunek_promienia.pobierzWektorY();
		double kierunek_promienia_z = kierunek_promienia.pobierzWektorZ();
		
		Wektor sphere_sfera_srodek = sfera_srodek;
		double sfera_srodek_x = sphere_sfera_srodek.pobierzWektorX();
		double sfera_srodek_y = sphere_sfera_srodek.pobierzWektorY();
		double sfera_srodek_z = sphere_sfera_srodek.pobierzWektorZ();
		
		double a = 1;
		double b = (2*(promien_zrodlo_x - sfera_srodek_x)*kierunek_promienia_x) + (2*(promien_zrodlo_y - sfera_srodek_y)*kierunek_promienia_y) + (2*(promien_zrodlo_z - sfera_srodek_z)*kierunek_promienia_z);
		double c = pow(promien_zrodlo_x - sfera_srodek_x, 2) + pow(promien_zrodlo_y - sfera_srodek_y, 2) + pow(promien_zrodlo_z - sfera_srodek_z, 2) - (radius*radius);
		
		double delta = b*b - 4*a*c;
		
		if (delta > 0) {
			// promien przecina obiekt
			
			// pierwszy punkt
			double pkt_1 = ((-1*b - sqrt(delta))/2) - 0.0000001;
			
			if (pkt_1 > 0) {
				return pkt_1;
			}
			else {
				double pkt_2 = ((sqrt(delta) - b)/2) - 0.0000001;
				return pkt_2;
			}
		}
		else {
			//promien nie trafil w obiekt
			return -1;
		}
	}
	
};

//powołanie nowego wektora (defaultowo w kierunku x)
Sfera::Sfera () {
	sfera_srodek = Wektor(0,0,0);
	radius = 1.0;
	kolor = Kolor(0.5, 0.5, 0.5, 0);
}

Sfera::Sfera (Wektor sfera_srodek_wartosc, double sfera_promien_wartosc, Kolor sfera_kolor_wartosc) {
	sfera_srodek = sfera_srodek_wartosc;
	radius = sfera_promien_wartosc;
	kolor = sfera_kolor_wartosc;
}


class Swiatlo : public Zrodlo {
	Wektor pozycja;
	Kolor kolor;
	
	public:
	
	Swiatlo ();
	
	Swiatlo (Wektor, Kolor);
	
	//metody
	virtual Wektor pobierzPozycjeSwiatla () { return pozycja; }
	virtual Kolor pobierzKolorSwiatla () { return kolor; }
	
};

//powołanie nowego wektora (defaultowo w kierunku x)
Swiatlo::Swiatlo () {
	pozycja = Wektor(0,0,0);
	kolor = Kolor(1,1,1, 0);
}

Swiatlo::Swiatlo (Wektor p, Kolor k) {
	pozycja = p;
	kolor = k;
}


class Kamera {
	Wektor kamera_pozycja, kamera_kierunek, kamera_margines_horyzont, kamera_margines_pion;
	
	public:
	
	Kamera ();
	
	Kamera (Wektor, Wektor, Wektor, Wektor);
	
	//metody
	Wektor pobierzPozycjeKamery () { return kamera_pozycja; }
	Wektor pobierzKierunekKamery () { return kamera_kierunek; }
	Wektor pobierzMargHoryzKamery () { return kamera_margines_horyzont; }
	Wektor pobierzMargPionKamery () { return kamera_margines_pion; }
	
};

//powołanie nowej kamery (defaultowo w kierunku z)
Kamera::Kamera () {
	kamera_pozycja = Wektor(0,0,0);
	kamera_kierunek = Wektor(0,0,1);
	kamera_margines_horyzont = Wektor(0,0,0);
	kamera_margines_pion = Wektor(0,0,0);
}

Kamera::Kamera (Wektor pos, Wektor dir, Wektor right, Wektor down) {
	kamera_pozycja = pos;
	kamera_kierunek = dir;
	kamera_margines_horyzont = right;
	kamera_margines_pion = down;
}


#endif
