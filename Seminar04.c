#define _CRT_SECURE_NO_WARININGS
#include <stdio.h>
#include<stdlib.h>
#include<string.h>

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
	Nod* urm;
};

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
	printf("Id: $d \n", masina.id);
	printf("Nr usi: %d\n", masina.nrUsi);
	printf("Pret: %d\n", masina.pret);
	printf("Nume sofer: %d\n", masina.numeSofer);
	printf("Serie: %d\n", masina.serie);
}

void afisareListaMasini(Nod* cap) {
	while (cap)
	{
		afisareMasina(cap->info);
		cap = cap->urm;
	}
}

void adaugaMasinaInLista(Nod** cap, Masina masinaNoua) {
	Nod* nou = (Nod*)malloc(sizeof(Nod));
	nou->info = masinaNoua;
	nou->urm = NULL;

	if ((*cap) = NULL)
	{
		*cap = nou;
	}
	else
	{
		Nod* p = *cap;
		while (p->urm)
		{
			p = p->urm;
		}
		p->urm = nou;
	}
}

void adaugaLaInceputInLista(Nod** cap, Masina masinaNoua) {
	Nod* nou = (Nod*)malloc(sizeof(Nod));
	nou->info = masinaNoua;
	nou->urm = *cap;
	*cap = nou;
}


Nod* citireListaMasiniDinFisier(const char* numeFisier) {
	Nod* cap = NULL;
	FILE* f = fopen(numeFisier, "r");
	if (f)
	{
		while (!feof(f))
		{
			adaugaMasinaInLista(&cap, citireMasinaDinFisier(f));
		}
	}
	fclose(f);
	return cap;
}

void dezalocareListaMasini(Nod** cap) {
	while ((*cap) != NULL) {
		Nod* aux = (*cap);
		*cap = (*cap)->urm;
		free(aux->info.numeSofer);
		free(aux->info.model);
		free(aux);
	}
}

float calculeazaPretMediu(Nod* lista) {
	if (!lista) return 0;
	int count = 0;
	float sum = 0;
	while (lista) {
		sum += lista->info.pret;
		count++;
		lista = lista->urm;
	}
	return (count > 0) ? sum / count : 0;
}

void stergeMasiniDinSeria(Nod** cap, char serieCautata)
{
	Nod* NodCurent = *cap;
	Nod* NodAnterior = NULL;

	while (NodCurent != NULL)
	{
		if (NodCurent->info.serie == serieCautata)
		{
			Nod* deSters = NodCurent;

			if (NodAnterior == NULL)
			{
				*cap = NodCurent->urm;
				NodCurent = *cap;
			}
			else
			{
				NodAnterior->urm = NodAnterior->urm;
				NodCurent = NodAnterior->urm;
			}

			free(deSters->info.model);
			free(deSters->info.numeSofer);
			free(deSters);
		}
		else
		{
			NodAnterior = NodCurent;
			NodCurent = NodCurent->urm;
		}
	}
}

float calculeazaPretulMasinilorUnuiSofer(Nod* lista, const char* numeSofer) {
	float sum = 0;
	while (lista) {
		if (strcmp(numeSofer, lista->info.numeSofer) == 0)
		{
			sum = lista->info.pret;
		}
		lista = lista->urm;
	}
	return sum;
}

char* getCeaMaiScumpaMasina(Nod* lista) {
	float pretulMax = 0;
	char* modelScump = NULL;
	while (lista)
	{
		if (lista->info.pret > pretulMax)
		{
			pretulMax = lista->info.pret;
			modelScump = lista->info.model;
		}
		lista = lista->urm;
	}
	if (pretulMax > 0) {
		char* nou = malloc(strlen(modelScump) + 1);
		strcpy_s(nou, strlen(modelScump) + 1, modelScump);
		return nou;
	}
	return NULL;
}

int main()
{
	Nod* cap = NULL;
	cap = citireListaMasiniDinFisier("masini.txt");
	afisareListaMasini(cap);

	printf("%f \n\n", calculeazaPretMediu(cap));
	printf("%f \n\n", calculeazaPretulMasinilorUnuiSofer(cap, "Ionescu"));
	char* model = getCeaMaiScumpaMasina(cap);
	printf("%s", model);
	if (model) {
		free(model);
	}

	dezalocareListaMasini(&cap);

	stergeMasiniDinSeria(&cap, 'A');

	return 0;
}