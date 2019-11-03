#ifndef OPENNES_FLAGS_H
#define OPENNES_FLAGS_H

#define FLAG_NEGATIVE 7
#define FLAG_OVERFLOW 6
#define FLAG_BREAK 4
#define FLAG_DECIMAL 3
#define FLAG_INTERRUPT 2
#define FLAG_ZERO 1
#define FLAG_CARRY 0

struct cpustate;

bool setzeroflag(uint8_t, cpustate *);
bool getflag(cpustate *, int flag);
bool setnegflag(uint8_t, cpustate *);

#endif //OPENNES_FLAGS_H
