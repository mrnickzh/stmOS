#ifndef DRIVER_H
#define DRIVER_H

typedef struct driver {
    const char *name;
    int (*init)(const void *cfg);
} driver_t;

const driver_t *driver_get(const char *name);

#endif