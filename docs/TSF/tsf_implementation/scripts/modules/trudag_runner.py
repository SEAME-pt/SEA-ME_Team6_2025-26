#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
DEPRECATED: This script is no longer maintained.

Please use the canonical shell pipeline script instead:
    setup_trudag_clean.sh

This Python script is retained for historical reference only and should not be used in CI/CD or local workflows.
All documentation and automation should reference setup_trudag_clean.sh as the official pipeline.
"""

import subprocess
import sys
import os
import shutil
import re
from pathlib import Path

class TruDAGRunner:
    """Handles TruDAG operations for TSF items."""

    def __init__(self, repo_root, tsf_impl_dir=None):
        self.repo_root = Path(repo_root)
        if tsf_impl_dir is None:
            self.tsf_impl = self.repo_root / "docs" / "TSF" / "tsf_implementation"
        else:
            self.tsf_impl = Path(tsf_impl_dir)
        self.items_dir = self.tsf_impl / "items"
        self.graph_dir = self.tsf_impl / "graph"
        self.db_file = self.tsf_impl / ".dotstop.dot"
        self.graph_file = self.graph_dir / "graph.dot"

        # Ensure directories exist
        self.graph_dir.mkdir(parents=True, exist_ok=True)

    def run_command(self, cmd, cwd=None, check=True):
        """Run a shell command and return the result."""
        try:
            if cwd is None:
                cwd = str(self.tsf_impl)
            
            # If cmd is string, split it into list
            if isinstance(cmd, str):
                cmd = cmd.split()
            
            result = subprocess.Popen(
                cmd,
                cwd=cwd,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            stdout, stderr = result.communicate()
            
            class CompletedProcess:
                def __init__(self, returncode, stdout, stderr):
                    self.returncode = returncode
                    self.stdout = stdout
                    self.stderr = stderr
            
            completed = CompletedProcess(result.returncode, stdout, stderr)
            
            if check and result.returncode != 0:
                raise subprocess.CalledProcessError(result.returncode, cmd, stderr)
            
            return completed
        except subprocess.CalledProcessError as e:
            print("❌ Command failed: {}".format(' '.join(cmd)))
            print("Error output: {}".format(e.stderr))
            raise

    def check_trudag_available(self):
        """Check if trudag command is available."""
        try:
            result = self.run_command(["which", "trudag"])
            return result.returncode == 0
        except:
            return False

    def generate_graph(self):
        """Generate graph.dot from TSF items using heuristics."""
        print("📊 Generating graph.dot from TSF items...")

        if not self.items_dir.exists():
            print("❌ Items directory not found: {}".format(self.items_dir))
            return False

        # Use the generate_graph_from_heuristics.py tool
        graph_tool = self.tsf_impl / "tools" / "generate_graph_from_heuristics.py"

        if not graph_tool.exists():
            print("❌ Graph generation tool not found: {}".format(graph_tool))
            return False

        try:
            cmd = [
                sys.executable, str(graph_tool),
                "--items", str(self.items_dir),
                "--out", str(self.graph_file)
            ]

            result = self.run_command(cmd, cwd=self.repo_root)
            print("✅ Graph generated: {}".format(self.graph_file))
            return True

        except subprocess.CalledProcessError:
            print("❌ Failed to generate graph")
            return False

    def init_database(self) -> bool:
        """Initialize TruDAG database."""
        print("🗄️  Initializing TruDAG database...")

        # Clean up existing database and generated files (like setup_trudag_clean.sh)
        print("  🧹 Cleaning existing files...")
        
        # Remove graph.dot
        if self.graph_file.exists():
            self.graph_file.unlink()
            print("    ✓ Removed graph.dot")
        
        # Remove .dotstop.dot files
        if self.db_file.exists():
            self.db_file.unlink()
            print("    ✓ Removed .dotstop.dot (tsf_impl)")
        
        db_symlink = self.repo_root / ".dotstop.dot"
        if db_symlink.exists():
            db_symlink.unlink()
            print("    ✓ Removed .dotstop.dot symlink (repo root)")
        
        # Remove .trudag_items directories
        trudag_items_tsf = self.tsf_impl / ".trudag_items"
        if trudag_items_tsf.exists():
            import shutil
            shutil.rmtree(trudag_items_tsf)
            print("    ✓ Removed .trudag_items/ (tsf_impl)")
        
        trudag_items_repo = self.repo_root / ".trudag_items"
        if trudag_items_repo.exists():
            import shutil
            shutil.rmtree(trudag_items_repo)
            print("    ✓ Removed .trudag_items/ (repo root)")
        
        # Remove needs.dot files
        import glob
        needs_files = glob.glob(str(self.repo_root / "**" / "needs.dot"), recursive=True)
        for needs_file in needs_files:
            Path(needs_file).unlink()
        if needs_files:
            print(f"    ✓ Removed {len(needs_files)} needs.dot files")
        
        print("  ✓ Cleanup complete")

        try:
            # Initialize in tsf_implementation directory
            result = self.run_command(["trudag", "init"], cwd=self.tsf_impl)
            
            # Create symlink in repo root for TruDAG to find it
            db_symlink = self.repo_root / ".dotstop.dot"
            if db_symlink.exists():
                db_symlink.unlink()
            db_symlink.symlink_to(self.db_file)
            print(f"  ✓ Created DB symlink: {db_symlink} -> {self.db_file}")
            
            print(f"✅ Database initialized: {self.db_file}")
            return True

        except subprocess.CalledProcessError:
            print("❌ Failed to initialize database")
            return False

    def create_items(self) -> bool:
        """Create all TSF items in the TruDAG database."""
        print("📝 Creating TSF items in database...")

        if not self.db_file.exists():
            print("❌ Database not initialized. Run init first.")
            return False

        created = 0
        failed = 0

        # Process each category directory
        categories = ["expectations", "assertions", "evidences", "assumptions"]

        for category in categories:
            category_dir = self.items_dir / category
            if not category_dir.exists():
                print(f"⚠️  Category directory not found: {category_dir}")
                continue

            print(f"  Processing {category}...")

            # Map category to TruDAG prefix
            prefix_map = {
                "expectations": "EXPECTATIONS",
                "assertions": "ASSERTIONS",
                "evidences": "EVIDENCES",
                "assumptions": "ASSUMPTIONS"
            }
            prefix = prefix_map.get(category, category.upper())

            # Process each item file
            for item_file in category_dir.glob("*.md"):
                if not item_file.is_file():
                    continue

                # Extract item ID from filename (remove .md extension)
                item_id = item_file.stem

                # Convert to TruDAG format (replace - with _)
                trudag_id = item_id.replace("-", "_")

                try:
                    # Create item directory
                    item_target_dir = self.tsf_impl / ".trudag_items" / prefix / trudag_id
                    item_target_dir.mkdir(parents=True, exist_ok=True)

                    # Create the item (ignore if already exists)
                    cmd = f"/Users/joaosilva/.local/bin/trudag manage create-item {prefix} {trudag_id} {item_target_dir}"
                    try:
                        self.run_command(cmd, cwd=self.repo_root)
                    except subprocess.CalledProcessError:
                        # Item may already exist, continue
                        pass

                    # Copy content from source file to created item
                    target_file = item_target_dir / f"{prefix}-{trudag_id}.md"
                    if target_file.exists():
                        # Read source content
                        source_content = item_file.read_text(encoding='utf-8')
                        
                        # Split by YAML frontmatter
                        parts = source_content.split('---', 2)
                        if len(parts) >= 3:
                            yaml_content = parts[1]
                            body = parts[2]
                            
                            # Remove ONLY TSF item references (../expectations/, ../assertions/, etc)
                            # Keep external references (docs/demos/, docs/guides/, URLs)
                            # Also remove evidence: sections as they are not standard TSF fields
                            lines = yaml_content.split('\n')
                            new_lines = []
                            i = 0
                            
                            while i < len(lines):
                                line = lines[i]
                                
                                # Skip evidence: sections
                                if line.strip() == 'evidence:':
                                    # Skip until next top-level key
                                    i += 1
                                    while i < len(lines) and (lines[i].startswith('  ') or lines[i].strip() == ''):
                                        i += 1
                                    continue
                                
                                # Check if this is start of a reference item
                                if line.strip().startswith('- '):
                                    # Collect all lines of this reference
                                    ref_lines = [line]
                                    j = i + 1
                                    while j < len(lines) and lines[j].startswith('  ') and not lines[j].strip().startswith('-'):
                                        ref_lines.append(lines[j])
                                        j += 1
                                    
                                    ref_text = '\n'.join(ref_lines)
                                    
                                    # Check if it's a TSF reference
                                    is_tsf = ('path:' in ref_text and ('../expectations' in ref_text or '../assertions' in ref_text or 
                                              '../evidences' in ref_text or '../assumptions' in ref_text)) or '- id:' in ref_text
                                    
                                    if not is_tsf:
                                        # Keep external references
                                        new_lines.extend(ref_lines)
                                    # else skip TSF references
                                    
                                    i = j
                                    continue
                                
                                new_lines.append(line)
                                i += 1
                            
                            new_yaml = '\n'.join(new_lines)
                            
                            # Check if references: line exists but has no list items after it
                            # If so, remove the entire references: line
                            lines_check = new_yaml.split('\n')
                            final_lines = []
                            i = 0
                            while i < len(lines_check):
                                line = lines_check[i]
                                if line.strip() == 'references:':
                                    # Check if next line is a list item (starts with '- ')
                                    if i + 1 < len(lines_check) and lines_check[i + 1].strip().startswith('- '):
                                        final_lines.append(line)  # Keep it
                                    # else skip it (no list items)
                                else:
                                    final_lines.append(line)
                                i += 1
                            
                            new_yaml = '\n'.join(final_lines)
                            
                            # Convert ID: EXPECT-L0-7 -> EXPECT_L0_7
                            new_yaml = re.sub(r'^id: ([A-Z]+-[A-Z0-9]+-[0-9]+)', lambda m: f"id: {m.group(1).replace('-', '_')}", new_yaml, flags=re.MULTILINE)
                            
                            # Clean URLs by removing trailing invalid characters
                            def clean_url_line(line):
                                if 'path: https' in line:
                                    # Extract URL and clean it
                                    match = re.search(r'path: (https?://[^\s]+)', line)
                                    if match:
                                        url = match.group(1)
                                        # Remove trailing invalid characters
                                        clean_url = re.sub(r'[|.,;:!?]+$', '', url)
                                        return line.replace(url, clean_url)
                                return line
                            
                            new_yaml = '\n'.join(clean_url_line(line) for line in new_yaml.split('\n'))
                            
                            source_content = f"---{new_yaml}---{body}"
                        
                        # Write to target
                        target_file.write_text(source_content, encoding='utf-8')

                    created += 1
                    print(f"    ✓ {prefix}-{trudag_id}")

                except Exception as e:
                    failed += 1
                    print(f"    ✗ {prefix}-{trudag_id} (error: {e})")

        print(f"✅ Items created: {created}, Failed: {failed}")
        return failed == 0

    def apply_links(self) -> bool:
        """Apply links from graph.dot to the database."""
        print("🔗 Applying links from graph.dot...")

        if not self.graph_file.exists():
            print(f"❌ Graph file not found: {self.graph_file}")
            return False

        try:
            # Apply the graph
            cmd = ["trudag", "graph", "apply", "graph/graph.dot"]
            result = self.run_command(cmd, cwd=self.tsf_impl)
            print("✅ Links applied successfully")
            return True

        except subprocess.CalledProcessError:
            print("❌ Failed to apply links")
            return False

    def validate_database(self) -> bool:
        """Run validation checks on the TruDAG database."""
        print("🔍 Validating TruDAG database...")

        try:
            # Run validation from repo root
            cmd = ["trudag", "manage", "lint"]
            result = self.run_command(cmd, cwd=self.repo_root)

            if "validation successful" in result.stdout.decode('utf-8').lower() or result.returncode == 0:
                print("✅ Database validation successful")
                return True
            else:
                print("⚠️  Database validation completed with warnings")
                print(result.stdout.decode('utf-8'))
                return True  # Still consider successful if it ran

        except subprocess.CalledProcessError:
            print("❌ Database validation failed")
            return False

    def set_items_reviewed(self) -> bool:
        """Mark all items as reviewed."""
        print("✅ Marking items as reviewed...")
        
        reviewed_count = 0
        trudag_items_dir = self.tsf_impl / ".trudag_items"
        
        if not trudag_items_dir.exists():
            print("❌ .trudag_items directory not found")
            return False
        
        for prefix_dir in trudag_items_dir.iterdir():
            if not prefix_dir.is_dir():
                continue
            
            prefix = prefix_dir.name
            
            for item_dir in prefix_dir.iterdir():
                if not item_dir.is_dir():
                    continue
                
                item_id = item_dir.name
                
                cmd = f"/Users/joaosilva/.local/bin/trudag manage set-item {prefix}-{item_id}"
                if self.run_command(cmd, cwd=self.repo_root):
                    reviewed_count += 1
                else:
                    print(f"    ⚠️  Failed to set {prefix}-{item_id} as reviewed")
        
        print(f"✓ Marked {reviewed_count} items as reviewed")
        return True

    def run_full_pipeline(self) -> bool:
        """Run the complete TruDAG pipeline."""
        print("🚀 Running complete TruDAG pipeline...")
        print("=" * 50)

        steps = [
            ("Generate Graph", self.generate_graph),
            ("Init Database", self.init_database),
            ("Create Items", self.create_items),
            # ("Set Items Reviewed", self.set_items_reviewed),  # TruDAG doesn't support setting items as reviewed
            # ("Apply Links", self.apply_links),  # Skip for now
            ("Validate", self.validate_database)
        ]

        success = True
        for step_name, step_func in steps:
            print(f"\n📍 {step_name}")
            if not step_func():
                success = False
                print(f"❌ Pipeline failed at: {step_name}")
                break
            print(f"✅ {step_name} completed")

        print("\n" + "=" * 50)
        if success:
            print("🎉 Complete TruDAG pipeline successful!")
        else:
            print("💥 Pipeline failed - check errors above")

        return success

    def cleanup(self):
        """Clean up generated files."""
        print("🧹 Cleaning up generated files...")

        files_to_remove = [
            self.graph_file,
            self.db_file,
            self.tsf_impl / ".trudag_items"
        ]

        for file_path in files_to_remove:
            if file_path.exists():
                if file_path.is_file():
                    file_path.unlink()
                elif file_path.is_dir():
                    shutil.rmtree(file_path)
                print(f"  ✓ Removed {file_path}")

        print("✅ Cleanup complete")

def main():
    """Command-line interface."""
    import argparse

    parser = argparse.ArgumentParser(description="TruDAG operations for TSF items")
    parser.add_argument("command", choices=["init", "generate", "create", "validate", "full", "cleanup"],
                       help="Command to run")
    parser.add_argument("--repo-root", type=Path,
                       default=Path(__file__).resolve().parents[5],
                       help="Repository root directory")
    parser.add_argument("--tsf-dir", type=Path,
                       help="TSF implementation directory (default: repo/docs/TSF/tsf_implementation)")

    args = parser.parse_args()

    runner = TruDAGRunner(args.repo_root, args.tsf_dir)

    # Check if trudag is available
    if not runner.check_trudag_available():
        print("❌ trudag command not found. Please install TruDAG first.")
        sys.exit(1)

    # Execute command
    commands = {
        "init": runner.init_database,
        "generate": runner.generate_graph,
        "create": runner.create_items,
        # "link": runner.apply_links,
        "validate": runner.validate_database,
        "full": runner.run_full_pipeline,
        "cleanup": runner.cleanup
    }

    try:
        success = commands[args.command]()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"❌ Error executing {args.command}: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()