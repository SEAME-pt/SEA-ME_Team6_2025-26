#include <gtest/gtest.h>
#include "temperatureprovider.hpp"
#include <QtCore>
#include <QtTest>


//Constructor initialization

TEST(TemperatureProviderTest, ConstructorInit)
{
    TemperatureProvider tp;

    EXPECT_EQ(tp.currTemperature().toStdString(), "0");
}


//checks temperature changes

TEST(TemperatureProviderTest, TemperatureChanges)
{
    TemperatureProvider tp;
    QSignalSpy spy(&tp, &TemperatureProvider::temperatureChanged);

    tp.setTemperature(25.567);
    EXPECT_EQ(tp.currTemperature().toStdString(),
              QString::number(25.567, 'f', 1).toStdString());
    EXPECT_EQ(spy.count(), 1);

    tp.setTemperature(13.436);
    EXPECT_EQ(tp.currTemperature().toStdString(),
              QString::number(13.436, 'f', 1).toStdString());
    EXPECT_EQ(spy.count(), 2);
}


// checks that setting the same temperature again does not emit a new change signal.
TEST(TemperatureProviderTest, SameTemperatureTwice)
{
    TemperatureProvider tp;
    QSignalSpy spy(&tp, &TemperatureProvider::temperatureChanged);

    tp.setTemperature(25.0);
    EXPECT_EQ(tp.currTemperature().toStdString(), "25.0");
    EXPECT_EQ(spy.count(), 1);

    tp.setTemperature(25.0);
    EXPECT_EQ(tp.currTemperature().toStdString(), "25.0");
    EXPECT_EQ(spy.count(), 1);
}


// checks that identical numeric values are not treated as a new temperature.
TEST(TemperatureProviderTest, FuzzyComparePreventsSignal)
{
    TemperatureProvider tp;
    QSignalSpy spy(&tp, &TemperatureProvider::temperatureChanged);

    tp.setTemperature(20.0);
    EXPECT_EQ(spy.count(), 1);

    tp.setTemperature(20.0);
    EXPECT_EQ(spy.count(), 1);
}

// checks that the temperature string is always rounded and displayed with exactly one decimal.
TEST(TemperatureProviderTest, TemperatureFormatting)
{
    TemperatureProvider tp;

    tp.setTemperature(7.3333);
    EXPECT_EQ(tp.currTemperature().toStdString(), "7.3");

    tp.setTemperature(7.3999);
    EXPECT_EQ(tp.currTemperature().toStdString(), "7.4");
}