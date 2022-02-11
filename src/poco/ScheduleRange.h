#ifndef SHEDULE_RANGE_H
#define SHEDULE_RANGE_H

class ScheduleRange{
  public:
    float start;
    float end;
    uint8_t power;
    uint8_t targetTemp;

    ScheduleRange(){};
};

#endif