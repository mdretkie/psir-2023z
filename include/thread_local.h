#ifndef THREAD_LOCAL_H
#define THREAD_LOCAL_H

#include "arduino.h"

#ifdef ARDUINO

#define thread_local

#else

#include <threads.h>

#endif

#endif
