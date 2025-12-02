#!/bin/bash

# Script to set scores for all TSF items

cd /Volumes/Important_Docs/42/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation

# Assertions: score 0.5
for item in .trudag_items/ASSERTIONS/*/item.md; do
    id=$(basename $(dirname $item))
    trudag manage set-item ASSERTIONS-$id score 0.5
done

# Assumptions: score 0.6
for item in .trudag_items/ASSUMPTIONS/*/item.md; do
    id=$(basename $(dirname $item))
    trudag manage set-item ASSUMPTIONS-$id score 0.6
done

# Evidences: score 1.0
for item in .trudag_items/EVIDENCES/*/item.md; do
    id=$(basename $(dirname $item))
    trudag manage set-item EVIDENCES-$id score 1.0
done

# Expectations: score 0.5
for item in .trudag_items/EXPECTATIONS/*/item.md; do
    id=$(basename $(dirname $item))
    trudag manage set-item EXPECTATIONS-$id score 0.5
done