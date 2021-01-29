#ifndef PANDOS_PCBT_H_INCLUDED
#define PANDOS_PCBT_H_INCLUDED

/****************************************************************************
 *
 * This header file contains definition of Process Tree.
 *
 ****************************************************************************/

#include "pandos_types.h"

int emptyChild(pcb_t *p);

void insertChild(pcb_t *prnt, pcb_t *p);

pcb_t *removeChild(pcb_t *p);

pcb_t *outChild(pcb_t *p);

#endif