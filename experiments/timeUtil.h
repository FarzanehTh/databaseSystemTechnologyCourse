#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <time.h>

/**
 * Computes the time difference between startTime and endTime and returns it in a struct timespec.
 *
 * @param startTime
 * @param endTime
 * @return a struct timespec of time difference between startTime and endTime.
*/
static inline struct timespec getTimeDiff(struct timespec startTime, struct timespec endTime) {
    return {
        endTime.tv_sec - startTime.tv_sec, endTime.tv_nsec - startTime.tv_nsec
    };
}

/**
 * Computes the time that the struct timespec time refers to in seconds.
 *
 * @param time
 * @return A double indicating the time that struct timespec time refers to in seconds.
*/
static inline double timespecToSecond(struct timespec time) {
    return time.tv_sec * 1.0 + time.tv_nsec / 1000000000.0;
}

/**
 * Computes the time that the struct timespec time refers to in nanoseconds.
 *
 * @param time
 * @return A double indicating the time that struct timespec time refers to in nanoseconds.
*/
static inline double timespecToNanoSecond(struct timespec time) {
    return time.tv_sec * 1000000000.0 + time.tv_nsec / 1.0;
}

#endif // TIME_UTIL_H