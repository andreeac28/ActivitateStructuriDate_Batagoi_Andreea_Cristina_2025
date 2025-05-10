#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//trebuie sa folositi fisierul masini.txt
//sau va creati un alt fisier cu alte date

struct StructuraMasina {
	int id;
	int nrUsi;
	float pret;
	char* model;
	char* numeSofer;
	unsigned char serie;
};
typedef struct StructuraMasina Masina;

typedef struct Nod Nod;
struct Nod {
	Masina info;
	Nod* st;
	Nod* dr;
};

//creare structura pentru un nod dintr un arbore binar de cautare

Masina citireMasinaDinFisier(FILE* file)
{
	char buffer[100];
	char sep[3] = ",\n";
	fgets(buffer, 100, file);
	char* aux;
	Masina m1;
	aux = strtok(buffer, sep);
	m1.id = atoi(aux);
	m1.nrUsi = atoi(strtok(NULL, sep));
	m1.pret = atof(strtok(NULL, sep));
	aux = strtok(NULL, sep);
	m1.model = malloc(strlen(aux) + 1);
	strcpy_s(m1.model, strlen(aux) + 1, aux);

	aux = strtok(NULL, sep);
	m1.numeSofer = malloc(strlen(aux) + 1);
	strcpy_s(m1.numeSofer, strlen(aux) + 1, aux);

	m1.serie = *strtok(NULL, sep);
	return m1;
}

void afisareMasina(Masina masina) {
	printf("Id: %d\n", masina.id);
	printf("Nr. usi : %d\n", masina.nrUsi);
	printf("Pret: %.2f\n", masina.pret);
	printf("Model: %s\n", masina.model);
	printf("Nume sofer: %s\n", masina.numeSofer);
	printf("Serie: %c\n\n", masina.serie);
}

int calculeazaInaltimeArbore(Nod* radacina) {
	if (radacina != NULL) {
		int inaltimeSt = calculeazaInaltimeArbore(radacina->st);
		int inaltimeDr = calculeazaInaltimeArbore(radacina->dr);
		return 1 + (inaltimeSt > inaltimeDr ? inaltimeSt : inaltimeDr);
	}
	else
	{
		return 0;
	}

}

//ALTE FUNCTII NECESARE:
// - aici veti adauga noile functii de care aveti nevoie.

void rotireStanga(Nod** radacina) {
	Nod* nod;
	nod = (*radacina)->dr;
	(*radacina)->dr = nod->st;
	nod->st = *radacina;
	*radacina = nod;
}

void rotireDreapta(Nod** radacina) {
	Nod* aux;
	aux = (*radacina)->st;
	(*radacina)->st = aux->dr;
	aux->dr = (*radacina);
	*radacina = aux;
}

int gradEchilbru(Nod* radacina) {
	if (radacina != NULL) {
		return(calculeazaInaltimeArbore(radacina->st) - calculeazaInaltimeArbore(radacina->dr));
	}
	else {
		return 0;
	}
}

void adaugaMasinaInArboreEchilibrat(Nod** radacina, Masina masinaNoua) {
	if ((*radacina) != NULL)
	{
		if ((*radacina)->info.id > masinaNoua.id)
		{
			adaugaMasinaInArboreEchilibrat(&((*radacina)->st), masinaNoua);
		}
		else
		{
			adaugaMasinaInArboreEchilibrat(&((*radacina)->dr), masinaNoua);
		}
		int grad = gradEchilbru((*radacina));
		if (grad == 2) { //dezechilibru stanga
			if (gradEchilbru((*radacina)->st) == 1) {
				rotireDreapta(radacina);
			}
			else
			{
				rotireStanga((*radacina)->st);
				rotireDreapta(radacina);
			}
		}
	}
}

void* citireArboreDeMasiniDinFisier(const char* numeFisier, Nod** radacina) {
	FILE* f = fopen(numeFisier, "r");
	if (f != NULL)
	{
		while (!feof(f)) {
			Masina m = citireMasinaDinFisier(f);
			if (m.model != NULL && m.numeSofer != NULL)
				adaugaMasinaInArboreEchilibrat(radacina, m);
		}
		fclose(f);
	}
	else
	{
		printf("Eroare la deschiderea fisierului.\n");
	}
}

void afisareInOrdine(Nod* rad) {
	if (rad)
	{
		afisareInOrdine(rad->st);
		afisareMasina(rad->info);
		afisareInOrdine(rad->dr);
	}
}

void afisarePreOrdine(Nod* rad) {
	if (rad) {
		afisareMasina(rad->info);
		afisarePreOrdine(rad->st);
		afisarePreOrdine(rad->dr);
	}
}

void afisarePostOrdine(Nod* rad)
{
	if (rad) {

		afisarePostOrdine(rad->st);
		afisarePostOrdine(rad->dr);
		afisareMasina(rad->info);
	}
}

void afisareMasiniDinArbore(Nod* radacina) {
	printf("\n== Inordine ==\n");
	afisareInOrdine(radacina);
	printf("\n== PreOrdine ==\n");
	afisarePreOrdine(radacina);
	printf("\n== PostOrdine ==\n");
	afisarePostOrdine(radacina);
}

void dezalocareArboreDeMasini(Nod* rad) {
	if (rad) {
		dezalocareArboreDeMasini(rad->st);
		dezalocareArboreDeMasini(rad->dr);
		free(rad->info.model);
		free(rad->info.numeSofer);
		free(rad);
	}
}

Masina getMasinaByID(Nod* radacina, int id) {
	if (radacina == NULL) {
		Masina m = { -1, 0, 0, NULL, NULL, 0 };
		return m;
	}
	if (id > radacina->info.id)
		return getMasinaByID(radacina->dr, id);
	else if (id < radacina->info.id)
		return getMasinaByID(radacina->st, id);
	else
		return radacina->info;
}
int determinaNumarNoduri(Nod* radacina) {
	if (radacina != NULL) {
		return 1 + determinaNumarNoduri(radacina->st) + determinaNumarNoduri(radacina->dr);
	}
	else {
		return 0;
	}
}

float calculeazaPretTotal(Nod* radacina ) {
	if (radacina)
	{
		return radacina->info.pret +
			calculeazaPretTotal(radacina->st) +
			calculeazaPretTotal(radacina->dr);
	}
	return 0;
}
 float calculeazaPretulMasinilorUnuiSofer(Nod* radacina, const char* numeSofer) {
	   if(radacina==NULL)
		return 0;

	   float suma = 0;
	   if (strcmp(radacina->info.numeSofer, numeSofer) == 0) {
		   suma += radacina->info.pret; 
	   }
	   suma += calculeazaPretulMasinilorUnuiSofer(radacina->st, numeSofer);
	   suma += calculeazaPretulMasinilorUnuiSofer(radacina->dr, numeSofer);
	   return suma;
	}

 int main() {
	 Nod* arbore = NULL;
	 citireArboreDeMasiniDinFisier("masini.txt", &arbore);

	 afisareMasiniDinArbore(arbore);

	 printf("\nNumar noduri: %d\n", determinaNumarNoduri(arbore));
	 printf("\nPret total masini: %.2f\n", calculeazaPretulMasinilorUnuiSofer(arbore));

	 const char* nume = "Popescu";
	 printf("Pret masini pentru soferul %s: %.2f\n", nume, calculeazaPretulMasinilorUnuiSofer(arbore, nume));



	 dezalocareArboreDeMasini(arbore);
	 return 0;
 }