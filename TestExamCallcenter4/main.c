#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simulationsdauer in Minuten */
# define SIM_DAUER 180
/* Anzahl Hotline - Agents ( und damit Anzahl Warteschlangen ) */
# define ANZAHL_AGENTS 4
/* Wahrscheinlichkeit ( in %) fuer genau 1 neuen Anruf pro Minute */
# define WARHSCHEINLICHKEIT_ANRUF 28
/* Untere Grenze : minimale Komplexitaet eines Anrufs */
# define MIN_KOMPLEX 2
/* Obere Grenze : maximale Komplexitaet eines Anrufs */
# define MAX_KOMPLEX 30
/* Wie viele Komplexitaetspunkte ein Agent pro Minute schafft */
# define PUNKTE_PRO_MIN 5
/* Untere Grenze : zusaetzliche Abschlusszeit in Minuten */
# define ABSCHLUSS_MIN 2
/* Obere Grenze : zusaetzliche Abschlusszeit in Minuten */
# define ABSCHLUSS_MAX 5
/* Kapazitaet der Queue pro Agent */
# define MAX_WARTESCHLANGE 12

#include "queue.h"

int zufall_bereich(int min, int max);
int aufrunden_div(int a, int b);
int gib_kuerzeste_warteschlange(Warteschlange warten[], int n);
int berechne_bearbeitungszeit(int komplex);
void simuliere_minute(int t,
	Warteschlange warten[], Agent agents[],
	long* summe_wartezeit, long* summe_systemzeit,
	int* max_wartezeit, int* max_systemzeit,
	int* angekommen, int* abgewiesen, int* bearbeitet,
	int* naechste_id);


int main() {
	srand(1);
	Warteschlange warten[ANZAHL_AGENTS];
	Agent agents[ANZAHL_AGENTS];
	int summe_wartezeit = 0;
	int summe_systemzeit = 0;
	int max_wartezeit = 0;
	int max_systemzeit = 0;
	int angekommen = 0;
	int abgewiesen = 0;
	int bearbeitet = 0;
	int naechste_id = 0;
	for (int i = 0; i < ANZAHL_AGENTS; i++) {
		warteschlange_init(&warten[i], sizeof(Anruf));
	}

	for (int t = 0; t < SIM_DAUER; t++) {
		simuliere_minute(t, warten, agents, &summe_wartezeit, &summe_systemzeit, &max_wartezeit, &max_systemzeit, &angekommen, &abgewiesen, &bearbeitet, &naechste_id);
	}

	printf("\n====== Report Callcenter Simulation ======\n");
	printf("Dauer:	%d Minuten\n", SIM_DAUER);
	printf("Agents:	%d\n", ANZAHL_AGENTS);
	printf("Wahrscheinlichkeit Anruf:	%d %%\n", WARHSCHEINLICHKEIT_ANRUF);
	printf("\nAnrufe angekommen:	%d\n", angekommen);
	printf("Anrufe bearbeitet:	%d\n", bearbeitet);
	printf("Anrufe abgewiesen:	%d\n", abgewiesen);
	printf("\nWartezeit in Minuten:	avg=%.2f	max=%d\n", (double)summe_wartezeit / bearbeitet, max_wartezeit);
	printf("Systemzeit in Minuten:	avg=%.2f	max=%d\n\n", (double)summe_systemzeit / bearbeitet, max_systemzeit);

	for (int i = 0; i < ANZAHL_AGENTS; i++) {
		printf("Agent %d:	id=%d	bearbeitet=%d	besetzte minuten=%d	auslastung=%.1f%%\n", i + 1, i, agents[i].bearbeitet_anzahl, agents[i].besetzt_minuten, (double)agents[i].besetzt_minuten / SIM_DAUER);
		warteschlange_freigeben(&warten[i]);
	}

	return 0;
}

int zufall_bereich(int min, int max) {
	return rand() % (max - min + 1) + min;
}

int aufrunden_div(int a, int b) {
	if (a % b == 0) {
		return a / b;
	}
	else {
		return a / b + 1;
	};
}

int gib_kuerzeste_warteschlange(Warteschlange warten[], int n) {
	int index = 0;
	for (int i = 0; i < n; i++) {
		if (warten[i].laenge < warten[index].laenge) {
			index = i;
		}
	}
	return index;
}

int berechne_bearbeitungszeit(int komplex) {
	int basiszeit = aufrunden_div(komplex, PUNKTE_PRO_MIN);
	int abschluss = zufall_bereich(ABSCHLUSS_MIN, ABSCHLUSS_MAX);
	int bearbeitungszeit = basiszeit + abschluss;
	return bearbeitungszeit;
}

void simuliere_minute(int t,
	Warteschlange warten[], Agent agents[],
	long* summe_wartezeit, long* summe_systemzeit,
	int* max_wartezeit, int* max_systemzeit,
	int* angekommen, int* abgewiesen, int* bearbeitet,
	int* naechste_id) {
	for (int i = 0; i < ANZAHL_AGENTS; i++) {
		if (agents[i].besetzt != 0) {
			agents[i].restzeit -= 1;
			if (agents[i].restzeit == 0) {
				agents[i].bearbeitet_anzahl += 1;
				agents[i].besetzt_minuten = t - agents[i].akt_start;
				agents[i].besetzt = 0;
				*bearbeitet += 1;
				int wartezeit = agents[i].akt_start - agents[i].akt_ankunft;
				int systemzeit = t - agents[i].akt_ankunft;
				*summe_wartezeit += wartezeit;
				*summe_systemzeit += systemzeit;
				if (wartezeit > *max_wartezeit) {
					*max_wartezeit = wartezeit;
				}
				if (systemzeit > *max_systemzeit) {
					*max_systemzeit = systemzeit;
				}
			}
		}
	}
	if (zufall_bereich(0, 100) <= WARHSCHEINLICHKEIT_ANRUF) {
		Anruf neu = {
		.id = naechste_id,
		.ankunft_minute = t,
		.komplex = zufall_bereich(MIN_KOMPLEX, MAX_KOMPLEX),
		};
		*angekommen += 1;
		int id_k_warten = gib_kuerzeste_warteschlange(warten, ANZAHL_AGENTS);
		if (warten[id_k_warten].laenge < MAX_WARTESCHLANGE) {
			if (warteschlange_einfuegen(&warten[id_k_warten], &neu) != 1) {
				*abgewiesen += 1;
			}
		}
		else {
			*abgewiesen += 1;
		}

	}
	for (int i = 0; i < ANZAHL_AGENTS; i++) {
		if (agents[i].besetzt == 0) {
			Anruf entnommen = { 0 };
			if (warteschlange_entnehmen(&warten[i], &entnommen) != 1) {
				printf("Fehler beim zuweisen eines Anrufs zu Agent mit id %d", i);
			}
			else
			{
				agents[i].akt_ankunft = entnommen.ankunft_minute;
				agents[i].akt_start = t;
				agents[i].akt_id = entnommen.id;
				agents[i].restzeit = berechne_bearbeitungszeit(entnommen.komplex);
			}
			

		}
	}
	*naechste_id += 1;
}
