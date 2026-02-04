import sys
import time
import os
import shutil
from pathlib import Path

def watch_and_sync(src_dir, dest_dir, extensions={".qml", ".js", ".mjs"}):
    print(f"Starting QML Watcher...")
    print(f"Source: {src_dir}")
    print(f"Dest:   {dest_dir}")
    
    src_path = Path(src_dir)
    dest_path = Path(dest_dir)
    
    if not src_path.exists():
        print("Error: Source directory does not exist.")
        return

    # Initial sync
    print("Performing initial sync...")
    for file in src_path.rglob('*'):
        if file.suffix in extensions:
            rel_path = file.relative_to(src_path)
            target = dest_path / rel_path
            if not target.exists() or file.stat().st_mtime > target.stat().st_mtime:
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(file, target)
                print(f"Synced: {rel_path}")

    print("Watching for changes (Ctrl+C to stop)...")
    last_mtime = time.time()
    
    try:
        while True:
            time.sleep(1)
            for file in src_path.rglob('*'):
                if file.suffix in extensions:
                    if file.stat().st_mtime > last_mtime:
                        rel_path = file.relative_to(src_path)
                        target = dest_path / rel_path
                        target.parent.mkdir(parents=True, exist_ok=True)
                        shutil.copy2(file, target)
                        print(f"Updated: {rel_path}")
            last_mtime = time.time()
    except KeyboardInterrupt:
        print("\nStopping watcher.")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python qml_watcher.py <src_dir> <dest_dir>")
        sys.exit(1)
    
    watch_and_sync(sys.argv[1], sys.argv[2])
