#!/bin/bash
set -e
cd /home/seame/Documents/SEA-ME_Team6_2025-26
source .venv/bin/activate

ITEMS_DIR="docs/TSF/tsf_implementation/.trudag_items"

echo "=== Removing stale L0_31 items ==="
trudag manage remove-item "ASSERTIONS-ASSERT_L0_31"   2>&1 | grep -v shadows; true
trudag manage remove-item "ASSUMPTIONS-ASSUMP_L0_31"  2>&1 | grep -v shadows; true
trudag manage remove-item "EVIDENCES-EVID_L0_31"      2>&1 | grep -v shadows; true
trudag manage remove-item "EXPECTATIONS-EXPECT_L0_31" 2>&1 | grep -v shadows; true
echo "Lines after remove: $(wc -l < .dotstop.dot) (expect 4 less)"

echo ""
echo "=== Re-adding L0_31 items ==="
trudag manage add-item "$ITEMS_DIR/ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md"   2>&1 | grep -v shadows; true
trudag manage add-item "$ITEMS_DIR/ASSUMPTIONS/ASSUMP_L0_31/ASSUMPTIONS-ASSUMP_L0_31.md" 2>&1 | grep -v shadows; true
trudag manage add-item "$ITEMS_DIR/EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md"         2>&1 | grep -v shadows; true
trudag manage add-item "$ITEMS_DIR/EXPECTATIONS/EXPECT_L0_31/EXPECTATIONS-EXPECT_L0_31.md" 2>&1 | grep -v shadows; true
echo "Lines after add: $(wc -l < .dotstop.dot)"

echo ""
echo "=== Reviewing L0_31 items ==="
trudag manage set-item "ASSERTIONS-ASSERT_L0_31"   2>&1 | grep -v shadows; true
trudag manage set-item "ASSUMPTIONS-ASSUMP_L0_31"  2>&1 | grep -v shadows; true
trudag manage set-item "EVIDENCES-EVID_L0_31"      2>&1 | grep -v shadows; true
trudag manage set-item "EXPECTATIONS-EXPECT_L0_31" 2>&1 | grep -v shadows; true

echo ""
echo "=== Creating L0_31 links ==="
trudag manage create-link "ASSERTIONS-ASSERT_L0_31"   "EVIDENCES-EVID_L0_31"      2>&1 | grep -v shadows; true
trudag manage create-link "EXPECTATIONS-EXPECT_L0_31" "ASSERTIONS-ASSERT_L0_31"   2>&1 | grep -v shadows; true
trudag manage create-link "EXPECTATIONS-EXPECT_L0_31" "ASSUMPTIONS-ASSUMP_L0_31"  2>&1 | grep -v shadows; true

echo ""
echo "=== Verification ==="
echo "Total edges: $(grep -c "\->" .dotstop.dot)"
grep "L0_31" .dotstop.dot | grep "\->"

echo ""
echo "=== Review status check ==="
grep "review_status" "$ITEMS_DIR/ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md"
grep "review_status" "$ITEMS_DIR/EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md"
