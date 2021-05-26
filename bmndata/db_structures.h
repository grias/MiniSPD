#ifndef DB_STRUCTURES_H
#define DB_STRUCTURES_H

struct GemMapStructure
{
    unsigned int serial;
    int channel_low;
    int channel_high;
    int station;
    int side;
    int part;
};

struct TriggerMapStructure
{
    unsigned int serial;
    unsigned int slot;
    int channel;
};

#endif // DB_STRUCTURES_H
