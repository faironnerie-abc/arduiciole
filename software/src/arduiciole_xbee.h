#ifndef __ARDUCIOLE_XBEE_H__
#define __ARDUCIOLE_XBEE_H__

#include "arduiciole.h"

void xbee_init(cmd_t*);
void xbee_transmit();
void xbee_flush();
void xbee_wait_tx_status();
cmd_t* xbee_receive(unsigned long);

void xbee_spread();

/**
 * Vérifie si l'adresse d'une luciole est dans le voisinage.
 *
 * @param addr adresse de la luciole voisine
 */
int xbee_is_in_swarm(uint64_t);

/**
 * Ajoute une luciole au voisinage.
 *
 * Si la taille du voisinage a été atteinte, la plus ancienne voisine est
 * remplacée par la nouvelle.
 *
 * @param addr adresse de la nouvelle voisine
 */
void xbee_add_to_swarm(uint64_t);

#endif /* __ARDUCIOLE_XBEE_H__ */
