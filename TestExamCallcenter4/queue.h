#ifndef QUEUE_H
#define QUEUE_H

/*
  Generische Warteschlange (Queue) als Ringpuffer.

  WICHTIG:
  - Wenn Sie MAX_WARTESCHLANGE im Hauptprogramm definieren möchten,
    dann definieren Sie MAX_WARTESCHLANGE VOR dem Include von queue.h.
  - Andernfalls gilt der Defaultwert unten.
*/

#ifndef MAX_WARTESCHLANGE
#define MAX_WARTESCHLANGE 20
#endif

typedef struct {
    unsigned char* daten;
    /* Speicherbereich fuer alle Elemente (Byte-Array) */

    int element_groesse;
    /* Groesse eines Elements in Bytes (z.B. sizeof(Anruf)) */

    int kapazitaet;
    /* Maximale Anzahl Elemente in der Queue */

    int vorne;
    /* Index des ersten gueltigen Elements */

    int hinten;
    /* Index der naechsten freien Position */

    int laenge;
    /* Aktuelle Anzahl Elemente (0..kapazitaet) */
} Warteschlange;


typedef struct Anruf {
    int id;
    int ankunft_minute;
    int komplex;
}Anruf;

typedef struct {
    /* 0 = frei , 1 = bearbeitet gerade einen Anruf */
    int besetzt;
    /* Verbleibende Minuten bis der aktuelle Anruf fertig ist */
    int restzeit;
    /* ID des aktuell bearbeiteten Anrufs */
    int akt_id;
    /* Ankunftsminute des aktuell bearbeiteten Anrufs */
    int akt_ankunft;
    /* Startminute der Bearbeitung */
    int akt_start;
    /* Wie viele Minuten dieser Agent insgesamt besetzt war */
    long besetzt_minuten;
    /* Wie viele Anrufe dieser Agent insgesamt bearbeitet hat */
    int bearbeitet_anzahl;
} Agent;



/* Initialisierung
   - warteschlange_init: nutzt MAX_WARTESCHLANGE als Kapazitaet
   - warteschlange_init_mit_kapazitaet: Kapazitaet frei waehlbar
   Rueckgabe: 1 = ok, 0 = Fehler (Parameter oder Speicher) */
int  warteschlange_init(Warteschlange* w, int element_groesse);
int  warteschlange_init_mit_kapazitaet(Warteschlange* w, int kapazitaet, int element_groesse);

/* Speicher freigeben (am Ende des Programms aufrufen) */
void warteschlange_freigeben(Warteschlange* w);

/* Status/Abfragen */
int  warteschlange_leer(const Warteschlange* w);
int  warteschlange_voll(const Warteschlange* w);
int  warteschlange_laenge(const Warteschlange* w);
int  warteschlange_kapazitaet(const Warteschlange* w);

/* Einfuegen/Entnehmen
   - element zeigt auf das einzufuegende Element (Bytes werden kopiert)
   - element_out zeigt auf Speicher, in den das Element kopiert wird
   Rueckgabe: 1 = ok, 0 = nicht moeglich (voll/leer/ungueltig) */
int  warteschlange_einfuegen(Warteschlange* w, const void* element);
int  warteschlange_entnehmen(Warteschlange* w, void* element_out);

/* Optional: erstes Element lesen, ohne es zu entfernen */
int  warteschlange_spitze(const Warteschlange* w, void* element_out);

#endif