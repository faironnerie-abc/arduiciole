#ifndef __ARDUICIOLE_H__
#define __ARDUICIOLE_H__

#ifdef ENERGIA
  #include "Energia.h"
#else
  #include "Arduino.h"
#endif

#include "arduiciole_config.h"
#include "arduiciole_helpers.h"

//
// Définition des commandes contenues dans les paquets.
//

#define CMD_SYNC  0x01
#define CMD_RESET 0x02
#define CMD_DEBUG 0x03

//
// Paramètres des lucioles
//

#ifndef LUCIOLE_VIEW
#define LUCIOLE_VIEW 4
#endif

#ifndef LUCIOLE_JUMP_THRESHOLD
#define LUCIOLE_JUMP_THRESHOLD 2
#endif

#ifndef LUCIOLE_FLASH_LENGTH
#define LUCIOLE_FLASH_LENGTH 1000
#endif

#ifndef LUCIOLE_CHARGE_LENGTH
#define LUCIOLE_CHARGE_LENGTH 1000
#endif

#ifndef LUCIOLE_CYCLE_LENGTH
#define LUCIOLE_CYCLE_LENGTH 5000
#endif

#ifndef LUCIOLE_ADD_IN_SWARM_PROB
#define LUCIOLE_ADD_IN_SWARM_PROB 0.1
#endif


//
// Code d'erreur
//

#define ERROR_NO_XBEE_RESPONSE 3
#define ERROR_BAD_XBEE_RESPONSE 4

/**
 * Définition de la structure des paquets émits et reçus.
 */
typedef struct {
  uint8_t  cmd;
  uint32_t data;
} cmd_t;

/**
 * La luciole rentre en mode d'erreur.
 *
 * La LED intégrée à l'arduino se met à clignoter un nombre de fois correspondant
 * au numéro de l'erreur.
 *
 * @param error le numéro d'erreur
 */
void error_mode(uint8_t);


/**
 * Donne l'indice d'une luciole dans le voisinage, -1 si elle n'y est pas
 *
 * @param addr adresse de la luciole voisine
 */
int essaim_index(uint16_t);

/**
 * Ajoute une luciole au voisinage.
 *
 * Si la taille du voisinage a été atteinte, la plus ancienne voisine est
 * remplacée par la nouvelle.
 *
 * @param addr adresse de la nouvelle voisine
 */
void add_to_essaim(uint16_t);

int essaim_flashing_count();

#include "arduiciole_xbee.h"

#endif /* __ARDUICIOLE_H__ */
