#ifndef MOTION_H
#define MOTION_H

void motion_tick(void);
int motion_getSpeed(void);
int motion_getCurrSpeed(void);
void motion_incrSpeed(void);
void motion_decrSpeed(void);
void motion_reset(void);
void motion_softStart(void);

#endif // MOTION_H