1. def main():
    parser = argparse.ArgumentParser(description="TSF Manager - Semi-Automated Generation")
    parser.add_argument('--detect-new', action='store_true', help='Detect and process new requirements')
    args = parser.parse_args()

    if args.detect_new:
        print("🔍 Detecting new TSF IDs...")
        new_ids = detect_new_ids()

        if not new_ids:
            print("✅ No new IDs found. All requirements have corresponding files.")
            return

        print(f"📋 Found new IDs: {', '.join(new_ids)}")

        for id_str in new_ids:
            requirement_text = get_requirement_text(id_str)
            print(f"\n🎯 Processing {id_str}: {requirement_text[:100]}...")


2. def detect_new_ids():
    """Detect new L0-X IDs from requirements table that don't have corresponding files."""
    if not REQUIREMENTS_TABLE.exists():
        print(f"❌ Requirements table not found: {REQUIREMENTS_TABLE}")
        return []

    with open(REQUIREMENTS_TABLE, 'r', encoding='utf-8') as f:
        content = f.read()

    # Find all L0-X in table
    ids_in_table = set(re.findall(r'\bL0-\d+\b', content))

    new_ids = []




    def detect_new_ids():

    ..... 

    for id_str in sorted(ids_in_table, key=lambda x: int(x.split('-')[1])):
        # Check if all 4 files exist
        expect_file = EXPECTATIONS_DIR / f"EXPECT-{id_str}.md"
        assert_file = ASSERTIONS_DIR / f"ASSERT-{id_str}.md"
        evid_file = EVIDENCES_DIR / f"EVID-{id_str}.md"
        assum_file = ASSUMPTIONS_DIR / f"ASSUMP-{id_str}.md"

        if not (expect_file.exists() and assert_file.exists() and evid_file.exists() and assum_file.exists()):
            new_ids.append(id_str)

    return new_ids


3. 
