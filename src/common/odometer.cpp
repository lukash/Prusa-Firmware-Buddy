// odometer_s.cpp

#include <cmath>

#include "odometer.hpp"
#include "cmath_ext.h"
#include "eeprom.h"

// translation table to get eevar from axis index
static constexpr enum eevar_id eevars[] = {
    EEVAR_ODOMETER_X,
    EEVAR_ODOMETER_Y,
    EEVAR_ODOMETER_Z,
    EEVAR_ODOMETER_E0
};

static_assert(sizeof(eevars) / sizeof(eevars[0]) == Odometer_s::axis_count, "count of axis does not match eeprom");
void Odometer_s::force_to_eeprom() {
    // Note: While running the force_to_eeprom, it's possible a get will
    // temporarily get slightly wrong value. Next time it'll be correct.
    bool changed = false;
    for (size_t i = 0; i < axis_count; ++i) {
        if (trip_xyze[i] != 0) {
            changed = true;
            break;
        }
    }
    if (duration_time != 0)
        changed = true;

    if (!changed)
        return;

    // cast is safe axis_count == axis_t::count_
    for (size_t i = 0; i < axis_count; ++i) {
        eeprom_set_flt(eevars[i], get(axis_t(i)));
        trip_xyze[i] = 0;
    }

    eeprom_set_ui32(EEVAR_ODOMETER_TIME, get_time());
    duration_time = 0;
}

void Odometer_s::add_value(int axis, float value) {
    // Technically, this is only weakly thread safe. Runnig this function from
    // multiple threads will not cause UB, but could still lose one of the
    // updates.
    //
    // This is not a problem, as the updates and storing is called only from
    // marlin. We need the atomics mostly to read them at the same time, while
    // printing.
    float current = trip_xyze[axis];
    /// E axis counts filament used instead of filament moved
    current += (axis == int(axis_t::E)) ? value : std::abs(value);
    trip_xyze[axis] = current;
}

float Odometer_s::get_from_eeprom(axis_t axis) {
    return eeprom_get_flt(eevars[size_t(axis)]);
}

float Odometer_s::get(axis_t axis) {
    return get_from_eeprom(axis) + MAX(0, trip_xyze[size_t(axis)].load());
}
void Odometer_s::add_time(uint32_t value) {
    duration_time += value;
}
uint32_t Odometer_s::get_time() {
    uint32_t time = eeprom_get_ui32(EEVAR_ODOMETER_TIME) + MAX(0ul, duration_time.load());
    return time;
}
