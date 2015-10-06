#ifndef __ARDUICIOLE_H__
#define __ARDUICIOLE_H__

#ifdef ENERGIA
  #include "Energia.h"
#else
  #include "Arduino.h"
#endif

#include "arduiciole_config.h"
#include "arduiciole_helpers.h"

#ifdef DEBUG
#include <AltSoftSerial.h>
extern AltSoftSerial altSoftSerial;
#endif

//
// Définition des commandes contenues dans les paquets.
//

#define CMD_NONE      0x00
#define CMD_SYNC      0x01
#define CMD_RESET     0x02
#define CMD_DEBUG     0x03
#define CMD_SWARM     0x04
#define CMD_TX_STATUS 0x05

//
// Paramètres des lucioles
//

#ifndef LUCIOLE_VIEW
# define LUCIOLE_VIEW 4
#endif

#ifndef LUCIOLE_JUMP_THRESHOLD
# define LUCIOLE_JUMP_THRESHOLD 5
#endif

#ifndef LUCIOLE_FLASH_LENGTH
# define LUCIOLE_FLASH_LENGTH 500
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

#define LUCIOLE_ADJUST_EPSILON 0.25
#define LUCIOLE_ADJUST_BASE_DELAY 500
#define LUCIOLE_ADJUST_ESTIMATE_MEAN_TX_DELAY 150

#define LUCIOLE_RESET_MIN_DELAY 1000
#define LUCIOLE_RESET_MAX_DELAY 5000

#ifndef TIMEOUT
# define TIMEOUT 2000
#endif

#ifndef ZB_TX_STATUS_TIMEOUT
# define ZB_TX_STATUS_TIMEOUT 2000
#endif

#define XBEE_SPREAD_DELAY 10000

//
// Code d'erreur
//

#define ERROR_NO_XBEE_RESPONSE 3
#define ERROR_BAD_XBEE_RESPONSE 4

/**
 * Définition de la structure des paquets émits et reçus.
 */
typedef uint8_t cmd_t;

typedef struct {
  unsigned long start_at;
  unsigned long swarm_cumul;
  uint8_t swarm_size;
} luciole_t;

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
 * Écoute des paquets en provenance du module XBee.
 *
 * On lit les paquets tant que le timeout n'a pas été atteint. On utilise pour
 * cela la fonction `xbee_receive()` définie dans arduiciole_xbee.h. Cette dernière
 * méthode gère la gestion des paquets et retourne une commande lorsque cela est
 * nécessaire.
 *
 * Cette méthode doit être appelée le plus souvent possible afin de lire le plus
 * de paquets posible. Elle remplace donc la méthode `delay()` habituelle.
 *
 * Description des actions ayant un impact sur la luciole :
 *
 * - "CMD_SYNC" : action de synchronisation avec le voisinage.
 * - "CMD_RESET" : désynchronisation. On applique pour cela un délai aléatoire
 *   compris entre LUCIOLE_RESET_MIN_DELAY et LUCIOLE_RESET_MAX_DELAY ;
 *
 * @param time_out
 */
void listen(unsigned long);

/**
 * Émission d'un flash sous la forme d'un paquet diffusé sur le réseau ZigBee.
 *
 * Un paquet est transmis à chaque membre du voisinage pour les informer du
 * nouveau cycle entamé par la luciole. Les LEDs sont ensuite allumer puis,
 * la luciole rentre dans une phase d'écoute jusqu'à atteindre le délai voulu
 * pour cette phase. Les LEDs sont ensuite éteintes progressivement. La durée de
 * cette phase est définie par LUCIOLE_FLASH_LENGTH.
 */
void flash();

/**
 * Phase de synchronisation de la luciole.
 *
 * Il s'agit d'une phase intermédiaire durant laquelle la luciole est éteinte
 * mais à l'écoute des transmissions venant de ses voisines. La durée de cette
 * phase est définie par LUCIOLE_WAIT_PHASE_LENGTH.
 */
void sync();

/**
 * Ajustement de l'état de la luciole.
 *
 * La valeur non-ajustée de cette phase est LUCIOLE_ADJUST_BASE_DELAY. On
 * calcule ensuite une durée à ajouter ou à soustraire en fonction des
 * informations reçues de la part des autres lucioles.
 *
 * La valeur d'ajustement est comprise dans [-LUCIOLE_ADJUST_BASE_DELAY; LUCIOLE_ADJUST_BASE_DELAY].
 * On obtiendra ainsi un délai final compris entre 0 et 2 * LUCIOLE_ADJUST_BASE_DELAY.
 *
 * Cette valeur traduit la volonté de se rapprocher de la phase moyenne des
 * voisins, en atténuant le déplacement via le facteur LUCIOLE_ADJUST_EPSILON.
 *
 */
void adjust()

#include "arduiciole_xbee.h"

#endif /* __ARDUICIOLE_H__ */
