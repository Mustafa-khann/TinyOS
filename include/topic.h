// topic.h - publish/subscribe message bus.
// Subsystems communicate over named topics carrying fixed-size messages,
// so producers (drivers) and consumers (world model, planner) never call
// each other directly. Single-core cooperative scheduling means no locking
// is needed; readers poll with their own sequence cursor.
#ifndef TOPIC_H
#define TOPIC_H

#include <stdint.h>

#define TOPIC_MAX        16
#define TOPIC_NAME_MAX   16
#define TOPIC_MSG_MAX    64   // max payload bytes per message
#define TOPIC_DEPTH      8    // ring depth per topic

typedef struct {
    char name[TOPIC_NAME_MAX];
    uint16_t msg_size;
    uint32_t seq;                              // total messages published
    uint8_t ring[TOPIC_DEPTH][TOPIC_MSG_MAX];
    uint8_t used;
} topic_t;

void topic_init(void);
// Create (or return existing) topic with the given message size.
topic_t* topic_advertise(const char* name, uint16_t msg_size);
topic_t* topic_find(const char* name);
int topic_publish(topic_t* t, const void* msg);
// Copy the newest message into out if its sequence is newer than *cursor.
// Updates *cursor and returns 1 if a new message was delivered, else 0.
int topic_poll(topic_t* t, uint32_t* cursor, void* out);
// Copy the newest message regardless of cursor; returns 1 if any exists.
int topic_latest(topic_t* t, void* out);
int topic_count(void);
const topic_t* topic_info(int index);

#endif // TOPIC_H
