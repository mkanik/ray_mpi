#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Obiekty.h"
#include "Obliczenia.h"         

#include "mpi.h"

#define WORLD MPI_COMM_WORLD                

using namespace std;

struct RGB {
	double r;
	double g;
	double b;
};

int dpi = 72;
int szerokosc = 640;
int wysokosc = 480;
int ilosc_pikseli = szerokosc*wysokosc; //liczba pikseli
double proporcja = (double)szerokosc/(double)wysokosc;

double swiatlo_otoczenia = 0.1;
double dokladnosc = 0.000001;

//MPI
int n, myid, numprocs, i, islave;

//zdefiniowane kierunki
	Wektor O (0,0,0);
	Wektor X (1,0,0);
	Wektor Y (0,1,0);
	Wektor Z (0,0,1);

//zdefiniowane kolory
	Kolor white (1.0, 1.0, 1.0, 0);
	Kolor yellow (1.0, 1.0, 0.1, 0.3);
	Kolor red (1.0, 0.1, 0.1, 0.7);
	Kolor green (0.1, 1.0, 0.1, 0.7);
	Kolor blue (0.1, 0.1, 1.0, 0.7);
	Kolor gray (0.5, 0.5, 0.5, 0);
	Kolor black (0.0, 0.0, 0.0, 0);

//dane kamery	
	Wektor kamera_pozycja (-4, 0.1, -9);
//Wektor kamera_pozycja (-4, 200, -9);
	Wektor patrz_w_kierunku (5, 3, 0);

//dane zrodla swiatla
	Wektor pozycja_swiatla (-7, 10, -10);

double x_przelicznik, y_przelicznik;

void generuj_scene () {
}

void zapiszbmp (const char *nazwa_pliku, RGB *dane) {
	FILE *plik;
	int wielkosc_pliku = 54 + 3*ilosc_pikseli;
	unsigned char fheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0}; 
	unsigned char iheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
	fheader[ 2] = (unsigned char)(wielkosc_pliku);
	fheader[ 3] = (unsigned char)(wielkosc_pliku>>8);
	fheader[ 4] = (unsigned char)(wielkosc_pliku>>16);
	fheader[ 5] = (unsigned char)(wielkosc_pliku>>24);
	iheader[ 4] = (unsigned char)(szerokosc);
	iheader[ 5] = (unsigned char)(szerokosc>>8);
	iheader[ 6] = (unsigned char)(szerokosc>>16);
	iheader[ 7] = (unsigned char)(szerokosc>>24);
	iheader[ 8] = (unsigned char)(wysokosc);
	iheader[ 9] = (unsigned char)(wysokosc>>8);
	iheader[10] = (unsigned char)(wysokosc>>16);
	iheader[11] = (unsigned char)(wysokosc>>24);
	plik = fopen(nazwa_pliku,"wb");
	fwrite(fheader, 1, 14, plik);
	fwrite(iheader, 1, 40, plik);
	for (int i = 0; i < ilosc_pikseli; i++) {
		double red = (dane[i].r)*255;
		double green = (dane[i].g)*255;
		double blue = (dane[i].b)*255;
		unsigned char kolor[3] = {(int)floor(blue),(int)floor(green),(int)floor(red)};
		fwrite(kolor, 1, 3, plik);
	}
	fclose(plik);
}
/*
void zapiszppm_1 (RGB *dane) {
	std::ofstream ofs("./ppm1.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << szerokosc << " " << wysokosc << "\n255\n";
	for (unsigned i = 0; i < ilosc_pikseli; ++i) {
		ofs << (int)(dane[i].r * 255) << ((int)dane[i].g * 255) << ((int)1, dane[i].b * 255); 
	}
	ofs.close();
}

void zapiszppm_2 (RGB *dane) {
	FILE *f = fopen("ppm2.ppm", "w");         // Write image to PPM file.
	fprintf(f, "P3\n%d %d\n%d\n", szerokosc, wysokosc, 255);
	for (int i = 0; i < ilosc_pikseli; i++) {
		int i2 = ilosc_pikseli - i;
		//fprintf(f,"%d %d %d ", ((int)dane[i2].r*255, (int)dane[i2].g*255, (int)dane[i2].b*255)); //niebieskie tlo z bialym W
		//fprintf(f,"%d %d %d ", ((int)dane[i].r, (int)dane[i].g, (int)dane[i].b)); //niebieskei tlo
		fprintf(f,"%d %d %d ", (int)(floor(dane[i].r*255), (int)floor(dane[i].g*255), (int)floor(dane[i].b*255))); 
	}
	fclose(f);
}
*/

int ObiektyPierwszoplanoweIndeks(vector<double> punkty_przeciecia_obiektu) {
	//zawraca numer pierwszoplanowego punktu przeciecia
	int indeks_minimum;
	
	//zapobieganie niepotrzebnym obliczeniom
	if (punkty_przeciecia_obiektu.size() == 0) {
		//jeśli nie ma żadnych punktów przecięcia
		return -1;
	}
	else if (punkty_przeciecia_obiektu.size() == 1) {
		if (punkty_przeciecia_obiektu.at(0) > 0) {
			//jesli jest jeden pkt przeciecia i ma wartosc wieksza niz 0
			return 0;
		}
		else {
			//jeslijest jedenpkt przeciecia ale ma wartosc 0
			return -1;
		}
	}
	else {
		// jeśli jest więcej niż jeden punkt przecięcia 
		// trzeba znaleźć ich maksymalną ilość
		
		double max = 0;
		for (int i = 0; i < punkty_przeciecia_obiektu.size();i++) {
			if (max < punkty_przeciecia_obiektu.at(i)) {
				max = punkty_przeciecia_obiektu.at(i);
			}
		}
		//nalezy znalezc minimalna wartosc
		if (max > 0) {
			for (int i = 0; i < punkty_przeciecia_obiektu.size(); i ++) {
				if (punkty_przeciecia_obiektu.at(i) > 0 && punkty_przeciecia_obiektu.at(i) <= max) {
					max = punkty_przeciecia_obiektu.at(i);
					indeks_minimum = i;
				}
			}
			return indeks_minimum;
		}
		else {
			return -1;
		}
	}
}

Kolor pobierzKolorW(Wektor pozycja_punktu_przeciecia, Wektor kierunek_przecinajacego_promienia, vector<Obiekt*> scena_obiekty, int indeks_pierwszoplanowych, vector<Zrodlo*> zrodla_swiatla, double dokladnosc, double swiatlo_otoczenia) {
	
	Kolor pierwszoplanowy_obiekt_kolor = scena_obiekty.at(indeks_pierwszoplanowych)->pobierzKolor();
	Wektor pierwszoplanowy_obiekt_norm = scena_obiekty.at(indeks_pierwszoplanowych)->pobierzZnormalizowanyW(pozycja_punktu_przeciecia);
	
	Kolor kolor_ostateczny = pierwszoplanowy_obiekt_kolor.kolorSkalar(swiatlo_otoczenia);
	
	for (int swiatlo_indeks = 0; swiatlo_indeks < zrodla_swiatla.size(); swiatlo_indeks++) {
		Wektor swiatlo_kierunek = zrodla_swiatla.at(swiatlo_indeks)->pobierzPozycjeSwiatla().wektorDodaj(pozycja_punktu_przeciecia.odwroc()).normalizuj();
		
		float kosinus_kata = pierwszoplanowy_obiekt_norm.wektorIlSkalarny(swiatlo_kierunek);
		
		if (kosinus_kata > 0) {
			// sprawdzanie cieni
			bool zacieniony = false;
			
			Wektor odleglosc_do_swiatla = zrodla_swiatla.at(swiatlo_indeks)->pobierzPozycjeSwiatla().wektorDodaj(pozycja_punktu_przeciecia.odwroc()).normalizuj();
			float odleglosc_do_swiatla_dlugoscWektora = odleglosc_do_swiatla.dlugoscWektora();
			
			Promien promien_od_cienia (pozycja_punktu_przeciecia, zrodla_swiatla.at(swiatlo_indeks)->pobierzPozycjeSwiatla().wektorDodaj(pozycja_punktu_przeciecia.odwroc()).normalizuj()); 
			
			vector<double> punkt_przeciecia_drugi;
			
			for (int nr_obiektu = 0; nr_obiektu < scena_obiekty.size() && zacieniony == false; nr_obiektu++) {
				punkt_przeciecia_drugi.push_back(scena_obiekty.at(nr_obiektu)->znajdzPunktPrzeciecia(promien_od_cienia));
			}
			
			for (int c = 0; c < punkt_przeciecia_drugi.size(); c++) {
				if (punkt_przeciecia_drugi.at(c) > dokladnosc) {
					if (punkt_przeciecia_drugi.at(c) <= odleglosc_do_swiatla_dlugoscWektora) {
						zacieniony = true;
					}
				}
				break;
			}
			
			if(zacieniony == false) {
				kolor_ostateczny = kolor_ostateczny.kolorDodaj(pierwszoplanowy_obiekt_kolor.kolorPomnoz(zrodla_swiatla.at(swiatlo_indeks)->pobierzKolorSwiatla()).kolorSkalar(kosinus_kata));
				
				if (pierwszoplanowy_obiekt_kolor.pobierzKolorPolysk() > 0 && pierwszoplanowy_obiekt_kolor.pobierzKolorPolysk() <= 1) {
					// polysk [0-1]
					double dot1 = pierwszoplanowy_obiekt_norm.wektorIlSkalarny(kierunek_przecinajacego_promienia.odwroc());
					Wektor skalar1 = pierwszoplanowy_obiekt_norm.wektorPomnoz(dot1);
					Wektor dodaj_1 = skalar1.wektorDodaj(kierunek_przecinajacego_promienia);
					Wektor skalar2 = dodaj_1.wektorPomnoz(2);
					Wektor dodaj_2 = kierunek_przecinajacego_promienia.odwroc().wektorDodaj(skalar2);
					Wektor odbicie_kierunek = dodaj_2.normalizuj();
					
					double specular = odbicie_kierunek.wektorIlSkalarny(swiatlo_kierunek);
					if (specular > 0) {
						specular = pow(specular, 10);
						kolor_ostateczny = kolor_ostateczny.kolorDodaj(zrodla_swiatla.at(swiatlo_indeks)->pobierzKolorSwiatla().kolorSkalar(specular*pierwszoplanowy_obiekt_kolor.pobierzKolorPolysk()));
					}
				}
			}
		}
	}
	
	//return Kolor (0, 0, 0, 0);
	return kolor_ostateczny.kolorPrzytnij();
}

int aktualny_piksel; //indeks piksela

int main (int argc, char *argv[]) {
	
	RGB *piksele = new RGB[ilosc_pikseli];

	cout << "inicjowanie kamery" << endl;
	Wektor kamera_kierunek_tmp (kamera_pozycja.pobierzWektorX() - patrz_w_kierunku.pobierzWektorX(), kamera_pozycja.pobierzWektorY() - patrz_w_kierunku.pobierzWektorY(), kamera_pozycja.pobierzWektorZ() - patrz_w_kierunku.pobierzWektorZ());
	Wektor kamera_kierunek = kamera_kierunek_tmp.odwroc().normalizuj();
	Wektor kamera_margines_horyzont = Y.wektorIlWektorowy(kamera_kierunek).normalizuj();
	Wektor kamera_margines_pion = kamera_margines_horyzont.wektorIlWektorowy(kamera_kierunek);
	Kamera scena_kamera (kamera_pozycja, kamera_kierunek, kamera_margines_horyzont, kamera_margines_pion);
	
	generuj_scene();
	//scena

	//dodawanie obiektów do sceny
	vector<Zrodlo*> zrodla_swiatla;
	vector<Obiekt*> scena_obiekty;

	cout << "inicjowanie zrodel swiatla" << endl;
	zrodla_swiatla.push_back(dynamic_cast<Zrodlo*>(new Swiatlo(pozycja_swiatla, white)));
	zrodla_swiatla.push_back(dynamic_cast<Zrodlo*>(new Swiatlo(Wektor (8, 6, -5), yellow)));

	cout << "inicjowanie obiektow" << endl;
	//W
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (0.3, 4.0 ,0.0), 0.8, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (0.6, 3.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (0.9, 2.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (1.2, 1.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (1.5, 0.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (1.8, 1.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (2.1, 2.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (2.4, 3.0 ,0.0), 0.8, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (2.8, 2.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (3.1, 1.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (3.4, 0.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (3.7, 1.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (4.0, 2.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (4.3, 3.0 ,0.0), 0.6, blue)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (4.6, 4.0 ,0.0), 0.8, blue)));
	
	//S
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (6.2, 3.5 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (6.0, 3.0 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (6.2, 2.5 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (6.0, 0.5 ,0.0), 0.8, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (6.5, 3.9 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (6.5, 2.1 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (6.5, 0.0 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (7.0, 4.0 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (7.0, 1.9 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (7.0, 0.1 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (8.0, 3.5 ,0.0), 0.8, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (7.8, 1.5 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (8.0, 1.0 ,0.0), 0.6, red)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (7.8, 0.5 ,0.0), 0.6, red)));

	//I
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (10.0, 4.0 ,0.0), 0.8, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (10.0, 0.0 ,0.0), 0.8, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (10.8, 4.0 ,0.0), 0.6, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (10.8, 3.0 ,0.0), 0.6, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (10.8, 2.0 ,0.0), 0.6, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (10.8, 1.0 ,0.0), 0.6, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (10.8, 0.0 ,0.0), 0.6, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (12.0, 4.0 ,0.0), 0.8, yellow)));
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Sfera(Wektor (12.0, 0.0 ,0.0), 0.8, yellow)));
	
	//podłoga
	scena_obiekty.push_back(dynamic_cast<Obiekt*>(new Plaszczyzna(Y, -1, green)));

	cout << "obliczanie sceny" << endl;
	
	//int num_threads, thread_id;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(WORLD, &numprocs);
	MPI_Comm_rank(WORLD, &myid);
	MPI_Status mpi_status;	
	
	
	for (int x_proc = 0; x_proc < numprocs; x_proc++) {
	
		for (int x = 0; x < szerokosc; x = x + 1) {
			for (int y = 0; y < wysokosc; y++) {
				
				if (myid == x_proc) {
					
				aktualny_piksel = y*szerokosc + x;
				
					x_przelicznik = ((x+0.5)/szerokosc)*proporcja - ((szerokosc-wysokosc)/(double)wysokosc/2);
					y_przelicznik = ((wysokosc -y) + 0.5)/wysokosc;
				
				
				// poczatek promienia jest taki sam jak pozycja kamery
				Wektor kamera_zrodlo_promienia = scena_kamera.pobierzPozycjeKamery();
				Wektor kamera_kierunek_promienia = kamera_kierunek.wektorDodaj(kamera_margines_horyzont.wektorPomnoz(x_przelicznik - 0.5).wektorDodaj(kamera_margines_pion.wektorPomnoz(y_przelicznik - 0.5))).normalizuj();
				
				Promien kamera_promien (kamera_zrodlo_promienia, kamera_kierunek_promienia);
				
				vector<double> punkty_przeciecia;
				
				//sprawdzanie czy promien przecina ktorys obiekt
				
				
				for (int i = 0; i < scena_obiekty.size(); i++){
					punkty_przeciecia.push_back(scena_obiekty.at(i)->znajdzPunktPrzeciecia(kamera_promien));
				}

				int indeks_pierwszoplanowych = ObiektyPierwszoplanoweIndeks(punkty_przeciecia);
				//odnajdywanie najbliższych obiektów
				
				if (indeks_pierwszoplanowych == -1) {
					// jesli promien nie trafia w obiekt rysuje czarne tlo
					piksele[aktualny_piksel].r = 0;
					piksele[aktualny_piksel].g = 0;
					piksele[aktualny_piksel].b = 0;
				}
				else {
					//indeks odpowiada obiektowi w scenie
					if (punkty_przeciecia.at(indeks_pierwszoplanowych) > dokladnosc) {
						//okreslenie pozycji i kierunku wektora w pokncie przeciecia
						Wektor pozycja_punktu_przeciecia = kamera_zrodlo_promienia.wektorDodaj(kamera_kierunek_promienia.wektorPomnoz(punkty_przeciecia.at(indeks_pierwszoplanowych)));
						Wektor kierunek_przecinajacego_promienia = kamera_kierunek_promienia;
						Kolor punkt_przeciecia_kolor = pobierzKolorW(pozycja_punktu_przeciecia, kierunek_przecinajacego_promienia, scena_obiekty, indeks_pierwszoplanowych, zrodla_swiatla, dokladnosc, swiatlo_otoczenia);
						piksele[aktualny_piksel].r = punkt_przeciecia_kolor.pobierzKolorRed();
						piksele[aktualny_piksel].g = punkt_przeciecia_kolor.pobierzKolorGreen();
						piksele[aktualny_piksel].b = punkt_przeciecia_kolor.pobierzKolorBlue();
					}
				}
			}
		}
	}
}
	
	
	cout << "zapisywanie wyniku" << endl;
	zapiszbmp("wynik.bmp", piksele);
	//zapiszppm_1(piksele);
	//zapiszppm_2(piksele);
	
	MPI_Finalize();
	return 0;
}

