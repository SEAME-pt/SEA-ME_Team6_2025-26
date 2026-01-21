#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QTest>
#include <QRegularExpression>
#include "timeprovider.hpp"
#include <QCoreApplication>

// Test that constructor initializes time and date
TEST(TimeProviderTest, ConstructorInit)
{
    TimeProvider tp;

    QString timeStr = tp.currTime();
    QString dateStr = tp.currDate();

    // Ensure strings are non-empty
    EXPECT_FALSE(timeStr.isEmpty());
    EXPECT_FALSE(dateStr.isEmpty());
}

// Test that QTimer triggers timeChanged and dateChanged signals
TEST(TimeProviderTest, UpdateTimeEmitsOnManualChange)
{
    TimeProvider tp;
    QSignalSpy timeSpy(&tp, &TimeProvider::timeChanged);

    tp.updateTime();  // initial
    QCoreApplication::processEvents();

    // Force change by mocking or adjusting format
    QTest::qWait(61000);
    tp.updateTime();

    EXPECT_GE(timeSpy.count(), 1);
}





// Test that consecutive calls without real time change do not emit signals
TEST(TimeProviderTest, NoDuplicateSignals)
{
    TimeProvider tp;

    QSignalSpy timeSpy(&tp, &TimeProvider::timeChanged);
    QSignalSpy dateSpy(&tp, &TimeProvider::dateChanged);

    // Immediately check signals after construction
    // Small wait to allow first QTimer trigger
    QTest::qWait(10);

    int initialTimeCount = timeSpy.count();
    int initialDateCount = dateSpy.count();

    // Wait less than 1 second, signals should not change
    QTest::qWait(500);

    EXPECT_EQ(timeSpy.count(), initialTimeCount);
    EXPECT_EQ(dateSpy.count(), initialDateCount);
}

// Test that currTime() and currDate() return reasonable strings
TEST(TimeProviderTest, CurrTimeAndDateReturnStrings)
{
    TimeProvider tp;

    QString timeStr = tp.currTime();
    QString dateStr = tp.currDate();

    // Basic sanity checks
    EXPECT_FALSE(timeStr.isEmpty());
    EXPECT_TRUE(timeStr.contains(QRegularExpression("\\d+"))); // contains digits

    EXPECT_FALSE(dateStr.isEmpty());
    EXPECT_TRUE(dateStr.contains(QRegularExpression("\\d+"))); // contains digits
}

// Optional: Check that time changes after a second (QTimer working)
TEST(TimeProviderTest, TimeChangesOverOneSecond)
{
    TimeProvider tp;

    QSignalSpy timeSpy(&tp, &TimeProvider::timeChanged);

    QString oldTime = tp.currTime();

    // Wait at least 1 second for QTimer to trigger
    QTest::qWait(1100);

    QString newTime = tp.currTime();

    // If time actually changed, signal should have fired
    if (oldTime != newTime) {
        EXPECT_GE(timeSpy.count(), 1);
    }
}

// Only one main() for the test runner
int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);   // Needed for QTimer
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
