/**
 * Util class for all serializatio and deserialization.
 */
#pragma once
#include <cstring>

class SerializeUtils
{
public:
    static void                     serialize_short(char* buf, int value);
    static void                     serialize_int(char* buf, int value);
    static void                     serialize_double(char* buf, double value);
    static int                      deserialize_short(char* buf);
    static int                      deserialize_int(char* buf);
    static double                   deserialize_double(char* buf);
};

inline void SerializeUtils::serialize_short(char* buf, int value) {
    value = value & 0xFFFF;
    buf[0] = value & 0xFF00;
    buf[1] = value & 0x00FF;
}

inline int SerializeUtils::deserialize_short(char* buf) {
    int value = 0;
    value += buf[1];
    value += (buf[0] << 8);
    return value;
}

inline void SerializeUtils::serialize_int(char* buf, int value) {
    memcpy(buf, &value, 4);
}

inline int SerializeUtils::deserialize_int(char* buf) {
    int* val = new int[1];
    memcpy(val, buf, 4);

    return *val;
}

inline void SerializeUtils::serialize_double(char* buf, double value) {
    memcpy(buf, &value, sizeof(double));
}

inline double SerializeUtils::deserialize_double(char* buf) {
    double* val = new double[1];
    memcpy(val, buf, sizeof(double));

    return *val;
}