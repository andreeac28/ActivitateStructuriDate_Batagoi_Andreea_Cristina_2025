#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//trebuie sa folositi fisierul masini.txt
//sau va creati un alt fisier cu alte date

//lista dubla inlantuinta, retine adresa nodului precedent, va retine NULL, adresa nodului anterior si precedent
//sa realizam o parcurgere- prin WHILE si de la sfarsit la inceput 
//cum indetificam o lsiat simpla inlantuita- prin bpointerul de la primul nod, dar la sf la inc cum se face, retinem si adresa ultimului nod 
// ne cream o alta structura care retine si prim si ulitm, 
//inserare: trebuie sa schimb si cap si ultim , in situatia in care lista inla, retin acelasi NULL, altfel daca am iserare la inceput schimb, daca nu la sfarsit 

struct StructuraMasina {
	int id;
	int nrUsi;
	float pret;
	char* model;
	char* numeSofer;
	unsigned char serie;
};
typedef struct StructuraMasina Masina;
struct Nod {
	Masina info;
	struct Nod* urmator;
	struct Nod* precedent;
};

typedef struct Nod Nod;
//creare structura pentru un nod dintr-o lista dublu inlantuita

//creare structura pentru Lista Dubla 
struct Lista {
	Nod* prim;
	Nod* ultim;
};
typedef struct Lista Lista;


Masina citireMasinaDinFisier(FILE* file) {
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

void afisareListaMasini(Lista list) {
	//de la inceput pana la sf
	Nod* p = list.prim;
	while (p)
	{
		afisareMasina(p->info);
		p = p->urmator; //afisarea in mod clasic 
	}
}

void adaugaMasinaInLista(Lista* list, Masina masinaNoua) {  //de la sf la inceput 
	//trebuie sa modificam ultimul, trebuie sa creeam NODUL, avem info, dar nu nodul 
	Nod* p = (Nod*)malloc(sizeof(Nod));
	p->info = masinaNoua; //shallow copy
	//trebuie sa intializam 
	p->precedent = list->ultim; //de unde incepe blocul, nu la final 
	p->urmator = NULL;
	if (list->ultim)
	{
		list->ultim->urmator = p;
	}
	else
	{
		list->prim = p; //trebuie sa-l modificam pe ultimul, pastram lista, si pe prim trebuie sa-l modificam
	}
	list->ultim = p; //mpd elem din lista, NU LSIA(era Lista* list si se trans prin list->ultim etc), e recomandat si prin pointer dar sa fie constant, TREBUIE PRIN ADRESAA!!!

}

void adaugaLaInceputInLista(Lista* list, Masina masinaNoua) {
	Nod* p = (Nod*)malloc(sizeof(Nod));
	p->info = masinaNoua;
	p->urmator = list->prim;
	p->precedent = NULL;
	if (list->prim) {
		list->prim->precedent = p;
	}
	else
	{
		list->prim = p;
	}
	list->ultim = p;
	
}



void afisareListaMasiniSF(Lista list)
{
	Nod* p = list.ultim;
	while (p)
	{
		afisareMasina(p->info);
		p = p->precedent;
	}

}

Lista citireLDMasiniDinFisier(const char* numeFisier) {
	FILE* f = fopen(numeFisier, "r");
	Lista list;
	list.prim = NULL;
	list.ultim = NULL;
	while (!feof(f))
	{
		//adaugaMasinaInLista(&list, citireMasinaDinFisier(f));
		adaugaLaInceputInLista(&list, citireMasinaDinFisier(f));
	}
	fclose(f);
	return list;

}

void dezalocareLDMasini(Lista* lista) 
{
	Nod* p = lista->prim;
	while (p->urmator != NULL)
	{
		free(p->info.numeSofer);
		free(p->info.model);
		p = p->urmator;
		free(p->precedent);

	}
	free(p->info.numeSofer);
	free(p->info.model);
	free(p);
	lista->prim = NULL;
	lista->ultim = NULL;

}

float calculeazaPretMediu(Lista lista )
{
	Nod* aux = lista.prim;
	float suma = 0;
	float cnt = 1;
	while (aux) {
		suma += aux->info.pret;
		cnt++;
		aux = aux->urmator;
	}
	return suma/cnt;
}

void stergeMasinaDupaID(Lista* lista, int id)
{
	Nod* current = lista->prim;

	while (current != NULL) {
		if (current->info.id == id) {
			if (current->precedent == NULL) {
				lista->prim = current->urmator;
				if (lista->prim != NULL) {
					lista->prim->precedent = NULL;
				}
				else {
					lista->ultim = NULL;
				}
			}
			else if (current->urmator == NULL) {
				lista->ultim = current->precedent;
				lista->ultim->urmator = NULL;
			}
			else {
				current->precedent->urmator = current->urmator;
				current->urmator->precedent = current->precedent;
			}
			free(current->info.model);
			free(current->info.numeSofer);
			free(current);
			return;
		}
		current = current->urmator;
}
	printf("Masina cu ID-ul %d nu a fost gasita.\n", id);
}

char* getNumeSoferMasinaScumpa(Lista lista) {
	if (lista.prim == NULL) {
		return NULL; 
	}

	Nod* current = lista.prim;
	Nod* mostExpensive = current;
	float maxPret = current->info.pret;

	while (current != NULL) {
		if (current->info.pret > maxPret) {
			maxPret = current->info.pret;
			mostExpensive = current;
		}
		current = current->urmator;
	}
	char* numeSofer = malloc(strlen(mostExpensive->info.numeSofer) + 1);
	strcpy(numeSofer, mostExpensive->info.numeSofer);
		return numeSofer;
	}


char* getNumeSoferMasinaScumpa(Lista lista)
{
	if (lista.prim == NULL) {
		return NULL;
	}

	Nod* current = lista.prim;
	Nod* mostExpensive = current;
	float maxPret = current->info.pret;

	while (current != NULL) {
		if (current->info.pret > maxPret) {
			maxPret = current->info.pret;
			mostExpensive = current;
		}
		current = current->urmator;
	}
	char* numeSofer = malloc(strlen(mostExpensive->info.numeSofer) + 1);
	strcpy(numeSofer, mostExpensive->info.numeSofer);
	
	return numeSofer;
};

int main() {
	Lista lista;
	lista = citireLDMasiniDinFisier("masini.txt");
	//afisareListaMasiniInceput(lista);
	afisareListaMasiniSF(lista);

	float pretMediu = calculeazaPretMediu(lista);
	printf("Pret mediu: %.2f\n", pretMediu);

	dezalocareLDMasini(&lista);

	return 0;
}