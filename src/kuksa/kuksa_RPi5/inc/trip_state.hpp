#pragma once
#include <chrono>

struct IKuksaClient;

// Aggregates distance/energy/speed/duration across CAN samples and
// publishes the derived VSS trip + lifetime-odometer signals.
// Trip boundary = process lifetime (a new trip starts each time the
// publisher starts up). Lifetime odometer persists across restarts.
class TripState
{
public:
    static TripState &instance();

    void init();
    void shutdown();

    // rps: wheel rotations/sec (signed), used for distance integration.
    // speedPublished: the same value being published as Vehicle.Speed
    // (m/h-scaled), so MaxSpeedTrip/AverageSpeed stay consistent with it.
    void onSpeedSample(IKuksaClient &kuksa, double rps, double speedPublished);

    // powerW: positive = discharging (battery power being used).
    void onPowerSample(IKuksaClient &kuksa, double powerW);

private:
    TripState() = default;

    void publishSpeedRelated(IKuksaClient &kuksa);
    void publishEnergyRelated(IKuksaClient &kuksa);
    void maybeFlushPersistence();
    void flushPersistence();
    void loadPersistedDistance();

    bool _hasSpeedSample = false;
    bool _hasPowerSample = false;

    double _lifetimeDistanceKm = 0.0;
    double _lifetimeDistanceKmAtLastFlush = 0.0;
    double _tripDistanceKm = 0.0;
    double _tripMaxSpeed = 0.0;
    double _tripEnergyUsedWh = 0.0;

    std::chrono::steady_clock::time_point _tripStart;
    std::chrono::steady_clock::time_point _lastSpeedSample;
    std::chrono::steady_clock::time_point _lastPowerSample;
};
