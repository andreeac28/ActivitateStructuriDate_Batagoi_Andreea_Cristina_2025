#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct StructuraMasina {
    int id;
    int nrUsi;
    float pret;
    char* model;
    char* numeSofer;
    unsigned char serie;
};
typedef struct StructuraMasina Masina;
typedef struct Nod nod;
struct Nod {
    Masina info;
    struct Nod* dr;
    struct Nod* st;
};

Masina citireMasinaDinFisier(FILE* file) {
    char buffer[100];
    char sep[3] = ", ";
        if (!fgets(buffer, 100, file)) {
            Masina m = { 0, 0, 0.0, NULL, NULL, 0 };
            return m;
        }
    char* aux;
    Masina m1;
    aux = strtok(buffer, sep);
    m1.id = atoi(aux);
    m1.nrUsi = atoi(strtok(NULL, sep));
    m1.pret = atof(strtok(NULL, sep));
    aux = strtok(NULL, sep);
    m1.model = malloc(strlen(aux) + 1);
    strcpy(m1.model, aux);

    aux = strtok(NULL, sep);
    m1.numeSofer = malloc(strlen(aux) + 1);
    strcpy(m1.numeSofer, aux);

    m1.serie = *strtok(NULL, sep);
    return m1;
}

void afisareMasina(Masina masina) {
    printf("Id: %d\n", masina.id);
    printf("Nr. usi: %d\n", masina.nrUsi);
    printf("Pret: %.2f\n", masina.pret);
    printf("Model: %s\n", masina.model);
    printf("Nume sofer: %s\n", masina.numeSofer);
    printf("Serie: %c\n\n", masina.serie);
}

void adaugaMasinaInArbore(nod** radacina, Masina masinaNoua) {
    if (*radacina != NULL) {
        if (masinaNoua.id < (*radacina)->info.id) {
            adaugaMasinaInArbore(&((*radacina)->st), masinaNoua);
        }
        else if (masinaNoua.id > (*radacina)->info.id) {
            adaugaMasinaInArbore(&((*radacina)->dr), masinaNoua);
        }
        else {
            free((*radacina)->info.model);
            free((*radacina)->info.numeSofer);
            (*radacina)->info = masinaNoua;
        }
    }
    else {
        *radacina = malloc(sizeof(nod));
        (*radacina)->info = masinaNoua;
        (*radacina)->st = NULL;
        (*radacina)->dr = NULL;
    }
}

nod* citireArboreDeMasiniDinFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    if (!file) return NULL;
    nod* radacina = NULL;
    while (!feof(file)) {
        Masina m = citireMasinaDinFisier(file);
        if (m.model != NULL && m.numeSofer != NULL)
            adaugaMasinaInArbore(&radacina, m);
    }
    fclose(file);
    return radacina;
}

void afisareArboreInOrdine(nod* radacina) {
    if (radacina != NULL) {
        afisareArboreInOrdine(radacina->st);
        afisareMasina(radacina->info);
        afisareArboreInOrdine(radacina->dr);
    }
}

void afisareArborePreOrdine(nod* radacina) {
    if (radacina != NULL) {
        afisareMasina(radacina->info);
        afisareArborePreOrdine(radacina->st);
        afisareArborePreOrdine(radacina->dr);
    }
}

void afisareArborePostOrdine(nod* radacina) {
    if (radacina != NULL) {
        afisareArborePostOrdine(radacina->st);
        afisareArborePostOrdine(radacina->dr);
        afisareMasina(radacina->info);
    }
}

int determinaNumarNoduri(nod* radacina) {
    if (radacina != NULL) {
        return 1 + determinaNumarNoduri(radacina->st) + determinaNumarNoduri(radacina->dr);
    }
    else {
        return 0;
    }
}

Masina getMasinaByID(nod* radacina, int id) {
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

void dezalocareArboreDeMasini(nod** radacina) {
    if (*radacina != NULL) {
        dezalocareArboreDeMasini(&(*radacina)->st);
        dezalocareArboreDeMasini(&(*radacina)->dr);
        free((*radacina)->info.model);
        free((*radacina)->info.numeSofer);
        free(*radacina);
        *radacina = NULL;
    }
}

int main() {
    nod* arbore = citireArboreDeMasiniDinFisier("masini_arbore.txt");

    printf("=== Preordine ===\n");
    afisareArborePreOrdine(arbore);

    printf("\n=== Inordine ===\n");
    afisareArboreInOrdine(arbore);

    printf("\n=== Postordine ===\n");
    afisareArborePostOrdine(arbore);

    Masina m = getMasinaByID(arbore, 2);
    printf("\n\n-------------------\n");
    afisareMasina(m);

    int i = determinaNumarNoduri(arbore);
    printf("Numar noduri: %d\n", i);

    dezalocareArboreDeMasini(&arbore);
    return 0;
}
