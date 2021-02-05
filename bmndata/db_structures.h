#ifndef DB_STRUCTURES_H
#define DB_STRUCTURES_H

struct DchMapStructure
{
    int plane;
    int group;
    unsigned int crate;
    int slot;
    int channel_low;
    int channel_high;
};

struct GemMapStructure
{
    unsigned int serial;
    int id;
    int station;
    int channel_low;
    int channel_high;
    int hotZone;
};

struct TriggerMapStructure
{
    unsigned int serial;
    unsigned int slot;
    int channel;
};

#endif // DB_STRUCTURES_H
