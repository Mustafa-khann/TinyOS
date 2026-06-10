#include "../include/topic.h"
#include "../include/string.h"

static topic_t topics[TOPIC_MAX];
static int n_topics = 0;

void topic_init(void) {
    memset(topics, 0, sizeof(topics));
    n_topics = 0;
}

topic_t* topic_find(const char* name) {
    for (int i = 0; i < n_topics; i++) {
        if (strcmp(topics[i].name, name) == 0) return &topics[i];
    }
    return 0;
}

topic_t* topic_advertise(const char* name, uint16_t msg_size) {
    if (msg_size == 0 || msg_size > TOPIC_MSG_MAX) return 0;
    topic_t* t = topic_find(name);
    if (t) return (t->msg_size == msg_size) ? t : 0;
    if (n_topics >= TOPIC_MAX) return 0;

    t = &topics[n_topics++];
    strncpy(t->name, name, TOPIC_NAME_MAX - 1);
    t->msg_size = msg_size;
    t->seq = 0;
    t->used = 1;
    return t;
}

int topic_publish(topic_t* t, const void* msg) {
    if (!t || !msg) return -1;
    memmove(t->ring[t->seq % TOPIC_DEPTH], msg, t->msg_size);
    t->seq++;
    return 0;
}

int topic_poll(topic_t* t, uint32_t* cursor, void* out) {
    if (!t || t->seq == 0 || *cursor >= t->seq) return 0;
    memmove(out, t->ring[(t->seq - 1) % TOPIC_DEPTH], t->msg_size);
    *cursor = t->seq;
    return 1;
}

int topic_latest(topic_t* t, void* out) {
    if (!t || t->seq == 0) return 0;
    memmove(out, t->ring[(t->seq - 1) % TOPIC_DEPTH], t->msg_size);
    return 1;
}

int topic_count(void) {
    return n_topics;
}

const topic_t* topic_info(int index) {
    if (index < 0 || index >= n_topics) return 0;
    return &topics[index];
}
