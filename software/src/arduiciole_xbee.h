#ifndef __ARDUCIOLE_XBEE_H__
#define __ARDUCIOLE_XBEE_H__

#include "arduiciole.h"
#include <XBee.h>

/**
 * Configuration du module XBee.
 *
 * La LED intégrée s'éteint jusqu'à ce que la configuration réussisse. Dans le
 * cas contraire, la luciole rentre en mode d'erreur.
 */
void xbee_init(cmd_t*);

/**
 * Transmet un paquet contenant la commande CMD_SYNC à l'ensemble des lucioles
 * contenu dans le voisinage.
 */
void xbee_transmit();

/**
 * Lecture des paquets.
 *
 * La méthode lit des paquets tant que le timeout n'a été pas atteint. L'apiId
 * des paquets doit être ZB_RX_RESPONSE lorsqu'ils contiennent des commandes.
 * Le seul autre type géré est ZB_TX_STATUS_RESPONSE, les confirmations d'envoi.
 *
 * Lorsqu'un paquet Rx est reçu, la méthode effectue les actions nécessaires
 * (si elles existent) poru la partie XBee, puis retourne la valeur de la commande.
 * Le pointeur data est initialisé avec le contenu du paquet.
 *
 * La gestion du voisinage est faite à l'intérieur de cette méthode.
 *
 * @param time_out
 * @param data
 */
cmd_t xbee_receive(unsigned long, uint8_t**);

/**
 * Broadcast d'un paquet contenant la commande CMD_SWARM.
 *
 * Le but est de signaler sa présence aux autres lucioles. Le paquet est emis
 * toutes les XBEE_SPREAD_DELAY millisecondes.
 */
void xbee_spread();

/**
 * Vérifie si l'adresse d'une luciole est dans le voisinage.
 *
 * @param addr adresse de la luciole voisine
 */
int xbee_is_in_swarm(XBeeAddress64&);

/**
 * Ajoute une luciole au voisinage.
 *
 * Si la taille du voisinage a été atteinte, la plus ancienne voisine est
 * remplacée par la nouvelle.
 *
 * @param addr adresse de la nouvelle voisine
 */
void xbee_add_to_swarm(XBeeAddress64&);

#endif /* __ARDUCIOLE_XBEE_H__ */
