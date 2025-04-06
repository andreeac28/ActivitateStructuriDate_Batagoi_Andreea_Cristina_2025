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

//creare structura pentru un nod dintr-o lista simplu inlantuita
typedef struct Nod Nod;
struct Nod {
	Masina info;
	Nod* urm;
}

//creare structura pentru tabela de dispersie
// aceasta este un vector de liste
; struct HashTable {
	int dim;
	Nod** v;
};
typedef struct HashTable HashTable;

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

void afisareListaMasini(Nod* v) {

	while (v->urm != NULL)
	{
		afisareMasina(v->info);
		v = v->urm;
	}
}

void adaugaMasinaInLista(Nod* v, Masina masinaNoua) {

	Nod* nou = (Nod*)malloc(sizeof(Nod));
	nou->info = masinaNoua;
	nou->urm = NULL;
	while (v->urm != NULL)
	{
		v = v->urm;
	}

	v->urm = nou;
}


HashTable initializareHashTable(int dimensiune) {
	HashTable ht;
	//initializeaza vectorul de liste si seteaza fiecare lista ca fiind NULL;
	ht.dim = dimensiune;
	ht.v = (Nod**)malloc(sizeof(Nod*) * dimensiune);
	for (int i = 0; i < dimensiune; i++)
	{
		ht.v[i] = NULL;
	}

	return ht;
}

int calculeazaHash(const char* nume, int dimensiune) {
	if (dimensiune == 0)
		return 0;
	// este calculat hash-ul in functie de dimensiunea tabelei si un atribut al masinii
	int s = 0;
	for (int i = 0; i < strlen(nume); i++)
		s += nume[i];

	return s % dimensiune;

}

void inserareMasinaInTabela(HashTable hash, Masina masina) {
	//este folosit mecanismul CHAINING
	//este determinata pozitia si se realizeaza inserarea pe pozitia respectiva
	int poz = calculeazaHash(masina.numeSofer, hash.dim);

	if (hash.v[poz] == NULL) {  //nu avem coliziune
		Nod* nou = (Nod*)malloc(sizeof(Nod));
		nou->info = masina;
		nou->urm = NULL;
		hash.v[poz] = nou;
	}
	else //avem coliziune
	{
		adaugaMasinaInLista(hash.v[poz], masina);
	}
}

HashTable citireMasiniDinFisier(const char* numeFisier, int dimensiune) {
	//functia primeste numele fisierului, il deschide si citeste toate masinile din fisier
	//prin apelul repetat al functiei citireMasinaDinFisier()
	// aceste masini sunt inserate intr-o tabela de dispersie initializata aici
	//ATENTIE - la final inchidem fisierul/stream-ul

	FILE* file = fopen(numeFisier, "r");
	HashTable ht = initializareHashTable(dimensiune);

	while (!feof(file))
	{
		inserareMasinaInTabela(ht, citireMasinaDinFisier(file));
	}
	fclose(file);
	return ht;
}

void afisareTabelaDeMasini(HashTable ht, int i) {
	//sunt afisate toate masinile cu evidentierea clusterelor realizate
	for (int i = 0; i < ht.dim; i++)
		printf("\nClusterul %d\\n", i + 1);
	if (ht.v[i] != NULL) {
		afisareListaMasini(ht.v[i]);
	}
	else {
		printf("(gol)\n");
	}
}


void dezalocareTabelaDeMasini(HashTable* ht) {
	//sunt dezalocate toate masinile din tabela de dispersie
	for (int i = 0; i < ht->dim; i++) {
		Nod* current = ht->v[i];
		while (current != NULL) {
			Nod* temp = current;
			current = current->urm;
			free(temp->info.model);
			free(temp->info.numeSofer);
			free(temp);
		}
	}
	free(ht->v);
	ht->v = NULL;
	ht->dim = 0;
}

float* calculeazaPreturiMediiPerClustere(HashTable ht, int* nrClustere) {
	*nrClustere = 0;
	for (int i = 0; i < ht.dim; i++) {
		if (ht.v[i] != NULL) {
			(*nrClustere)++;
		}
	}

	float* medii = (float*)malloc(sizeof(float) * (*nrClustere));
	if (medii == NULL) return NULL;

	int index = 0;
	for (int i = 0; i < ht.dim; i++) {
		if (ht.v[i] != NULL) {
			float suma = 0;
			int count = 0;
			Nod* current = ht.v[i];
			while (current != NULL) {
				suma += current->info.pret;
				count++;
				current = current->urm;
			}
			medii[index++] = suma / count;
		}
	}
	return medii;
}

Masina getMasinaDupaCheie(HashTable ht, const char* nume) {
	Masina m;
	//cauta masina dupa valoarea atributului cheie folosit in calcularea hash-ului
	//trebuie sa modificam numele functiei 
	int hash = calculeazaHash(nume, ht.dim);
	if (ht.v[hash])
	{
		Nod* p = ht.v[hash];
		while (p && strcmp(nume, p->info.numeSofer) != 0) {
			p = p->urm;
		}
		if (p)
		{
			m = p->info;
			m.model = malloc(strlen(p->info.model) + 1);
			strcpy_s(m.model, strlen(p->info.model) + 1, p->info.model);

			m.numeSofer = malloc(strlen(p->info.numeSofer) + 1);
			strcpy_s(m.model, strlen(p->info.numeSofer) + 1, p->info.numeSofer);
		}

	}
	return m;
}

int main() {
	HashTable ht = citireMasiniDinFisier("masini.txt", 5);
	afisareTabelaDeMasini(ht);

	Masina m = getMasinaDupaCheie(ht, "Gigel");
	if (m.id != -1)
	{
		afisareMasina(m);
		free(m.model);
		free(m.numeSofer);
	}
	else
	{
		printf("Nu a fost gasita!\n");
	}

	int nrClustere;
	float* medii = calculeazaPreturiMediiPerClustere(ht, &nrClustere);
	if (medii != NULL) {
		printf("\nPreturi medii per cluster:\n");
		for (int i = 0; i < nrClustere; i++) {
			printf("Cluster %d: %.2f\n", i + 1, medii[i]);
		}
		free(medii);
	}

	dezalocareTabelaDeMasini(&ht);
	return 0;
}