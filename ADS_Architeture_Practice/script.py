import os

# Files to include
EXTENSIONS = {'.h', '.cpp', '.ui', '.pro', '.pri', 'CMakeLists.txt'}
# Folders to completely ignore
IGNORE_DIRS = {'build', 'release', 'debug', '.git', '.vs', 'Resource'}

output_file = "full_project_code.txt"

with open(output_file, 'w', encoding='utf-8') as outfile:
    for root, dirs, files in os.walk("."):
        # Filter out ignored directories
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS and not d.startswith("build-")]
        
        for file in files:
            ext = os.path.splitext(file)[1]
            if ext in EXTENSIONS:
                path = os.path.join(root, file)
                outfile.write(f"\n{'='*50}\n")
                outfile.write(f"FILE: {path}\n")
                outfile.write(f"{'='*50}\n")
                try:
                    with open(path, 'r', encoding='utf-8') as infile:
                        outfile.write(infile.read())
                except Exception as e:
                    outfile.write(f"Error reading file: {e}\n")

print(f"Done! Upload '{output_file}' to the chat.")