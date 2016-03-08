#pragma once

#include <cmath>
#include "vector.hpp"

namespace gorc {

template <typename T> T to_degrees(T value) {
    return value * static_cast<T>(57.2957795);
}

template <typename T> T to_radians(T value) {
    return value * static_cast<T>(0.0174532925);
}

template <typename T, typename F> T lerp(const T& a, const T& b, F alpha) {
    return (1 - alpha) * a + alpha * b;
}

template <typename F> F clerp(F a, F b, F alpha) {
    double da = static_cast<double>(a);
    double db = static_cast<double>(b);
    double dalpha = static_cast<double>(alpha);

    // Normalize A and B.
    auto nrm_a = fmod(fmod(da, 360.0) + 360.0, 360.0);
    auto nrm_b = fmod(fmod(db, 360.0) + 360.0, 360.0);

    // Compute the minimum angle between A and B.
    if(fabs(nrm_a - nrm_b) <= 180.0) {
        // A and B are less than 180 degrees apart.
        return static_cast<F>(lerp(nrm_a, nrm_b, dalpha));
    }
    else if(nrm_a < nrm_b) {
        // A and B are more than 180 degrees apart.
        nrm_a += 360.0;
        auto res = lerp(nrm_a, nrm_b, dalpha);
        return static_cast<F>(fmod(fmod(res, 360.0) + 360.0, 360.0));
    }
    else {
        // A and B are more than 180 degrees apart.
        nrm_b += 360.0;
        auto res = lerp(nrm_a, nrm_b, dalpha);
        return static_cast<F>(fmod(fmod(res, 360.0) + 360.0, 360.0));
    }
}

template <typename F> vector<3, F> clerp(const vector<3, F>& a, const vector<3, F>& b, F alpha) {
    return make_vector(clerp(get<0>(a), get<0>(b), alpha),
            clerp(get<1>(a), get<1>(b), alpha),
            clerp(get<2>(a), get<2>(b), alpha));
}

template <typename T> T clamp(T value, T min, T max) {
    return std::max(min, std::min(value, max));
}

template <size_t n, typename F> vector<n, F> clamp_length(const vector<n, F>& value, F min, F max) {
    auto value_len = length(value);
    if(value_len <= F(0)) {
        // Can't do anything. Abort.
        return value;
    }
    else if(value_len < min) {
        return value * (min / value_len);
    }
    else if(value_len > max) {
        return value * (max / value_len);
    }
    else {
        return value;
    }
}

inline vector<3, uint8_t> convert_rgb565(uint16_t value) {
    uint8_t red = static_cast<uint8_t>((static_cast<float>((value >> 11) & 0x1FU) / 31.0f) * 255.0f);
    uint8_t green = static_cast<uint8_t>((static_cast<float>((value >> 5) & 0x3FU) / 63.0f) * 255.0f);
    uint8_t blue = static_cast<uint8_t>((static_cast<float>((value >> 0) & 0x1FU) / 31.0f) * 255.0f);

    return make_vector(red, green, blue);
}

template <typename T> T next_power_of_two(T value) {
    --value;
    for(size_t i = 1; i < sizeof(value) * 8; ++i) {
        value |= value >> i;
    }
    ++value;
    return value;
}

}
