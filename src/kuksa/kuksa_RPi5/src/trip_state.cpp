#include "../inc/trip_state.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sys/stat.h>

#include "../inc/can_to_kuksa_publisher.hpp"
#include "../inc/interface_kuksa_client.hpp"
#include "../inc/signals.hpp"

namespace {
constexpr const char *kPersistDir = "/var/lib/can-to-kuksa";
constexpr const char *kPersistFile = "/var/lib/can-to-kuksa/odometer.dat";
constexpr double kFlushEveryKm = 0.01;
constexpr double kMaxSampleGapSeconds = 5.0;

double secondsSince(std::chrono::steady_clock::time_point since)
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - since).count();
}
} // namespace

TripState &TripState::instance()
{
    static TripState state;
    return state;
}

void TripState::loadPersistedDistance()
{
    std::ifstream in(kPersistFile);
    if (!in.is_open())
        return;

    double value = 0.0;
    if (in >> value && value >= 0.0)
        _lifetimeDistanceKm = value;
}

void TripState::init()
{
    loadPersistedDistance();
    _lifetimeDistanceKmAtLastFlush = _lifetimeDistanceKm;
    _tripStart = std::chrono::steady_clock::now();
}

void TripState::flushPersistence()
{
    ::mkdir(kPersistDir, 0755);

    std::ofstream out(kPersistFile, std::ios::trunc);
    if (!out.is_open())
        return;

    out << _lifetimeDistanceKm;
    _lifetimeDistanceKmAtLastFlush = _lifetimeDistanceKm;
}

void TripState::maybeFlushPersistence()
{
    if (_lifetimeDistanceKm - _lifetimeDistanceKmAtLastFlush >= kFlushEveryKm)
        flushPersistence();
}

void TripState::shutdown()
{
    flushPersistence();
}

void TripState::onSpeedSample(IKuksaClient &kuksa, double rps, double speedPublished)
{
    const auto now = std::chrono::steady_clock::now();

    if (_hasSpeedSample) {
        const double dt = secondsSince(_lastSpeedSample);
        if (dt > 0.0 && dt < kMaxSampleGapSeconds) {
            const double distanceDeltaKm = std::abs(rps) * WHEEL_PERIMETER * dt / 1000.0;
            _tripDistanceKm += distanceDeltaKm;
            _lifetimeDistanceKm += distanceDeltaKm;
        }
    }
    _lastSpeedSample = now;
    _hasSpeedSample = true;

    _tripMaxSpeed = std::max(_tripMaxSpeed, std::abs(speedPublished));

    publishSpeedRelated(kuksa);
    maybeFlushPersistence();
}

void TripState::onPowerSample(IKuksaClient &kuksa, double powerW)
{
    const auto now = std::chrono::steady_clock::now();

    if (_hasPowerSample) {
        const double dt = secondsSince(_lastPowerSample);
        if (dt > 0.0 && dt < kMaxSampleGapSeconds && powerW > 0.0) {
            _tripEnergyUsedWh += powerW * dt / 3600.0;
        }
    }
    _lastPowerSample = now;
    _hasPowerSample = true;

    publishEnergyRelated(kuksa);
}

void TripState::publishSpeedRelated(IKuksaClient &kuksa)
{
    const double elapsedHours = secondsSince(_tripStart) / 3600.0;
    const double avgSpeedKmh = elapsedHours > 0.0 ? _tripDistanceKm / elapsedHours : 0.0;
    // Match Vehicle.Speed's m/h-scaled convention (real km/h * 1000).
    const double avgSpeedPublished = avgSpeedKmh * 1000.0;
    const std::uint32_t tripDurationSeconds =
        static_cast<std::uint32_t>(secondsSince(_tripStart));

    kuksa.publishFloat(sig::VEHICLE_TRAVELED_DISTANCE, static_cast<float>(_lifetimeDistanceKm));
    kuksa.publishFloat(sig::VEHICLE_TRAVELED_DISTANCE_SINCE_START, static_cast<float>(_tripDistanceKm));
    kuksa.publishFloat(sig::VEHICLE_AVG_SPEED, static_cast<float>(avgSpeedPublished));
    kuksa.publishFloat(sig::VEHICLE_MAX_SPEED_TRIP, static_cast<float>(_tripMaxSpeed));
    kuksa.publishUint32(sig::VEHICLE_TRIP_DURATION, tripDurationSeconds);
}

void TripState::publishEnergyRelated(IKuksaClient &kuksa)
{
    const double energyPerKmWh = _tripDistanceKm > 0.001 ? _tripEnergyUsedWh / _tripDistanceKm : 0.0;

    kuksa.publishFloat(sig::VEHICLE_TOTAL_ENERGY_USED_TRIP, static_cast<float>(_tripEnergyUsedWh));
    kuksa.publishFloat(sig::VEHICLE_ENERGY_PER_KM_TRIP, static_cast<float>(energyPerKmWh));
}
