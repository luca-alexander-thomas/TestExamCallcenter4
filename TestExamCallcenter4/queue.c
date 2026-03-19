#include "queue.h"
#include <stdlib.h>  /* malloc, free */
#include <string.h>  /* memcpy */

static int naechster_index(const Warteschlange* w, int index)
{
    return (index + 1) % w->kapazitaet;
}

static void warteschlange_reset(Warteschlange* w)
{
    w->daten = 0;
    w->element_groesse = 0;
    w->kapazitaet = 0;
    w->vorne = 0;
    w->hinten = 0;
    w->laenge = 0;
}

int warteschlange_init(Warteschlange* w, int element_groesse)
{
    return warteschlange_init_mit_kapazitaet(w, MAX_WARTESCHLANGE, element_groesse);
}

int warteschlange_init_mit_kapazitaet(Warteschlange* w, int kapazitaet, int element_groesse)
{
    unsigned long bytes;

    if (w == 0) return 0;
    warteschlange_reset(w);

    if (kapazitaet <= 0) return 0;
    if (element_groesse <= 0) return 0;

    w->kapazitaet = kapazitaet;
    w->element_groesse = element_groesse;

    bytes = (unsigned long)kapazitaet * (unsigned long)element_groesse;
    w->daten = (unsigned char*)malloc(bytes);
    if (w->daten == 0) {
        warteschlange_reset(w);
        return 0;
    }

    w->vorne = 0;
    w->hinten = 0;
    w->laenge = 0;
    return 1;
}

void warteschlange_freigeben(Warteschlange* w)
{
    if (w == 0) return;

    if (w->daten != 0) {
        free(w->daten);
    }
    warteschlange_reset(w);
}

int warteschlange_leer(const Warteschlange* w)
{
    if (w == 0) return 1;
    return (w->laenge == 0);
}

int warteschlange_voll(const Warteschlange* w)
{
    if (w == 0) return 0;
    return (w->laenge >= w->kapazitaet);
}

int warteschlange_laenge(const Warteschlange* w)
{
    if (w == 0) return 0;
    return w->laenge;
}

int warteschlange_kapazitaet(const Warteschlange* w)
{
    if (w == 0) return 0;
    return w->kapazitaet;
}

int warteschlange_einfuegen(Warteschlange* w, const void* element)
{
    unsigned char* ziel;

    if (w == 0) return 0;
    if (element == 0) return 0;
    if (w->daten == 0) return 0;
    if (warteschlange_voll(w)) return 0;

    ziel = w->daten + ((unsigned long)w->hinten * (unsigned long)w->element_groesse);
    memcpy(ziel, element, (unsigned long)w->element_groesse);

    w->hinten = naechster_index(w, w->hinten);
    w->laenge++;
    return 1;
}

int warteschlange_entnehmen(Warteschlange* w, void* element_out)
{
    unsigned char* quelle;

    if (w == 0) return 0;
    if (element_out == 0) return 0;
    if (w->daten == 0) return 0;
    if (warteschlange_leer(w)) return 0;

    quelle = w->daten + ((unsigned long)w->vorne * (unsigned long)w->element_groesse);
    memcpy(element_out, quelle, (unsigned long)w->element_groesse);

    w->vorne = naechster_index(w, w->vorne);
    w->laenge--;
    return 1;
}

int warteschlange_spitze(const Warteschlange* w, void* element_out)
{
    const unsigned char* quelle;

    if (w == 0) return 0;
    if (element_out == 0) return 0;
    if (w->daten == 0) return 0;
    if (warteschlange_leer(w)) return 0;

    quelle = w->daten + ((unsigned long)w->vorne * (unsigned long)w->element_groesse);
    memcpy(element_out, quelle, (unsigned long)w->element_groesse);

    return 1;
}