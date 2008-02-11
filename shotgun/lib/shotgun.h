#ifndef RBS_SHOTGUN_H
#define RBS_SHOTGUN_H

#define INTERNAL_DEBUG 0

#define TRACK_STATS 0
#define DISABLE_CHECKS 1
// #define TIME_LOOKUP 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "shotgun/config.h"
#include "shotgun/lib/oop.h"
/* declare the message struct since it's used in state.h, but actually
 * defined in message.h */
struct message;
#include "shotgun/lib/state.h"
#include "shotgun/lib/message.h"
#include "shotgun/lib/auto.h"
#include "shotgun/lib/metaclass.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/log.h"
#endif
