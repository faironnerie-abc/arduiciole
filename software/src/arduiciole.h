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
# define LUCIOLE_VIEW 4
#endif

#ifndef LUCIOLE_JUMP_THRESHOLD
# define LUCIOLE_JUMP_THRESHOLD 2
#endif

#ifndef LUCIOLE_FLASH_PHASE_LENGTH
# define LUCIOLE_FLASH_PHASE_LENGTH 1000
#endif

#ifndef LUCIOLE_WAIT_PHASE_LENGTH
# define LUCIOLE_WAIT_PHASE_LENGTH 4000
#endif

#ifndef LUCIOLE_MIN_WAIT_PHASE_LENGTH
# define LUCIOLE_MIN_WAIT_PHASE_LENGTH 500
#endif

#ifndef LUCIOLE_ADD_IN_SWARM_PROB
# define LUCIOLE_ADD_IN_SWARM_PROB 0.1
#endif

#ifndef LUCIOLE_DELAY_BETWEEN_FLASH
# define LUCIOLE_DELAY_BETWEEN_FLASH 250
#endif

#ifndef TIMEOUT
# define TIMEOUT 2000
#endif

#ifndef ZB_TX_STATUS_TIMEOUT
# define ZB_TX_STATUS_TIMEOUT 2000
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
 * Émission d'un flash sous la forme d'un paquet diffusé sur le réseau ZigBee.
 */
void flash();

/**
 * Phase de synchronisation de la luciole.
 */
void sync();

/**
 * Vérifie si l'adresse d'une luciole est dans le voisinage.
 *
 * @param addr adresse de la luciole voisine
 */
int is_in_essaim(uint16_t);

/**
 * Ajoute une luciole au voisinage.
 *
 * Si la taille du voisinage a été atteinte, la plus ancienne voisine est
 * remplacée par la nouvelle.
 *
 * @param addr adresse de la nouvelle voisine
 */
void add_to_essaim(uint16_t);

#include "arduiciole_xbee.h"

#endif /* __ARDUICIOLE_H__ */
