
#ifndef _CONSTS_DEVICE_H_
#define _CONSTS_DEVICE_H_

#if PA28
#include "consts_PA28.h"
#elif PB25
#include "consts_PB25.h"
#elif PA23
#include "consts_PA23.h"
#elif PA21
#include "consts_PA21.h"
#elif AS97
#include "consts_AS97.h"
#elif AS87
#include "consts_AS87.h"
#elif DL75
#include "consts_DL75.h"
#elif DL40
#include "consts_DL40.h"
#else
#error "Not implemented yet."
#endif

#endif

