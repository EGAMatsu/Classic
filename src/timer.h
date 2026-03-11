// timer.h — fixed-timestep timer (ticks & partialTicks)

#ifndef TIMER_H
#define TIMER_H

#define NS_PER_SECOND        1000000000LL
#define MAX_NS_PER_UPDATE    1000000000LL
#define MAX_TICKS_PER_UPDATE 100

typedef struct {
    float      ticksPerSecond;
    long long  lastTime;
    float      timeScale;
    float      fps;
    float      passedTime;
    int        ticks;
    float      partialTicks;
} Timer;

void       Timer_init(Timer* timer, float ticksPerSecond);
void       Timer_advanceTime(Timer* timer);
long long  currentTimeMillis(void);
long long  getCurrentTimeInNanoseconds(void);

#endif /* TIMER_H */