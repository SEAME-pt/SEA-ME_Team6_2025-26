#include <gtest/gtest.h>
#include "speedprovider.hpp"
#include <QtCore>
#include <QtTest>

// int argc = 0;
// char *argv[] = {nullptr};

// Constructor
TEST(SpeedProviderTest, ConstructorInit) {
    SpeedProvider sp;

    EXPECT_EQ(sp.currSpeed().toStdString(), "0.0");
}

// Speed Within Bounds
TEST(SpeedProviderTest, SpeedWithinBounds) {
    SpeedProvider sp;
    QSignalSpy spy(&sp, &SpeedProvider::speedChanged);

    sp.setSpeed(25.567);
    EXPECT_EQ(sp.currSpeed().toStdString(), "25.6");
    EXPECT_EQ(spy.count(), 1);

    sp.setSpeed(13.436);
    EXPECT_EQ(sp.currSpeed().toStdString(), "13.4");
    EXPECT_EQ(spy.count(), 2);
}

// Speed Above Max
TEST(SpeedProviderTest, SpeedAboveMax) {
    SpeedProvider sp;
    QSignalSpy spy(&sp, &SpeedProvider::speedChanged);

    sp.setSpeed(40.099);
    EXPECT_EQ(sp.currSpeed().toStdString(), "40.0");
    EXPECT_EQ(spy.count(), 1);

    sp.setSpeed(74.212);
    EXPECT_EQ(sp.currSpeed().toStdString(), "40.0");
    EXPECT_EQ(spy.count(), 1);
}

// Speed Bellow Min
TEST(SpeedProviderTest, SpeedBellowMin) {
    SpeedProvider sp;
    QSignalSpy spy(&sp, &SpeedProvider::speedChanged);

    sp.setSpeed(-0.091);
    EXPECT_EQ(sp.currSpeed().toStdString(), "0.0");
    EXPECT_EQ(spy.count(), 0);

    sp.setSpeed(-43.874);
    EXPECT_EQ(sp.currSpeed().toStdString(), "0.0");
    EXPECT_EQ(spy.count(), 0);

    sp.setSpeed(10.0);
    EXPECT_EQ(sp.currSpeed().toStdString(), "10.0");
    EXPECT_EQ(spy.count(), 1);

    sp.setSpeed(-10.0);
    EXPECT_EQ(sp.currSpeed().toStdString(), "0.0");
    EXPECT_EQ(spy.count(), 2);
}

// Same Speed Twice
TEST(SpeedProviderTest, SameSpeedTwice) {
    SpeedProvider sp;
    QSignalSpy spy(&sp, &SpeedProvider::speedChanged);

    sp.setSpeed(25.0);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(sp.currSpeed().toStdString(), "25.0");

    sp.setSpeed(25);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(sp.currSpeed().toStdString(), "25.0");
}

// Speed Formatting
TEST(SpeedProviderTest, SpeedFormatting) {
    SpeedProvider sp;

    sp.setSpeed(7.3333);
    EXPECT_EQ(sp.currSpeed().toStdString(), "7.3");

    sp.setSpeed(7.3999);
    EXPECT_EQ(sp.currSpeed().toStdString(), "7.4");
}