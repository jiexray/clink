/**
 * Util class for all serializatio and deserialization.
 */
#pragma once
#include <cstring>

class SerializeUtils
{
public:
    static void                     serialize_short(unsigned char* buf, int value);
    static void                     serialize_int(unsigned char* buf, int value);
    static void                     serialize_double(unsigned char* buf, double value);
    static int                      deserialize_short(unsigned char* buf);
    static int                      deserialize_int(unsigned char* buf);
    static double                   deserialize_double(unsigned char* buf);
};

inline void SerializeUtils::serialize_short(unsigned char* buf, int value) {
    value = value & 0xFFFF;
    buf[0] = (value & 0xFF00) >> 8;
    buf[1] = value & 0x00FF;
}

inline int SerializeUtils::deserialize_short(unsigned char* buf) {
    int value = 0;
    value += buf[1];
    value += (buf[0] << 8);
    return value;
}

inline void SerializeUtils::serialize_int(unsigned char* buf, int value) {
    memcpy(buf, &value, 4);
}

inline int SerializeUtils::deserialize_int(unsigned char* buf) {
    int* val = new int[1];
    memcpy(val, buf, 4);

    return *val;
}

inline void SerializeUtils::serialize_double(unsigned char* buf, double value) {
    memcpy(buf, &value, sizeof(double));
}

inline double SerializeUtils::deserialize_double(unsigned char* buf) {
    double* val = new double[1];
    memcpy(val, buf, sizeof(double));

    return *val;
}