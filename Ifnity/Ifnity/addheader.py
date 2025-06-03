import subprocess
import os
from datetime import datetime

# Header constants
BANNER_LINE = "//------------------ IFNITY ENGINE SOURCE -------------------//"
COPYRIGHT_LINE = "// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio"
LICENSE_LINE = "// Licensed under the MIT License. See LICENSE file for details."
MODIFIED_PREFIX = "// Last modified:"

def get_git_user():
    try:
        return subprocess.check_output(['git', 'config', 'user.name'], encoding='utf-8').strip()
    except Exception as e:
        print(f"[⚠️] Could not get Git user: {e}")
        return "Unknown User"

def get_modified_files():
    try:
        output = subprocess.check_output(['git', 'diff', '--name-only', 'HEAD'], encoding='utf-8')
        return [line.strip().replace("\\", "/") for line in output.splitlines()]
    except Exception as e:
        print(f"[⚠️] Could not get modified files from Git: {e}")
        return []

def clean_git_path(path):
    idx = path.lower().find("src/")
    return path[idx:] if idx != -1 else path

def update_header_in_file(full_path, username):
    if not os.path.isfile(full_path):
        print(f"[⛔] Skipping non-existent file: {full_path}")
        return

    try:
        with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"[❌] Failed to read {full_path}: {e}")
        return

    while lines and (
        lines[0].strip() in [BANNER_LINE, COPYRIGHT_LINE, LICENSE_LINE] or
        lines[0].startswith(MODIFIED_PREFIX)
    ):
        lines.pop(0)

    date_str = datetime.today().strftime('%Y-%m-%d')
    header = [
        BANNER_LINE + "\n",
        COPYRIGHT_LINE + "\n",
        LICENSE_LINE + "\n",
        f"{MODIFIED_PREFIX} {date_str} by {username}\n",
        "\n"
    ]

    try:
        with open(full_path, 'w', encoding='utf-8', errors='ignore') as f:
            f.writelines(header + lines)
        print(f"[✅] Header updated in: {full_path}")
    except Exception as e:
        print(f"[❌] Failed to write {full_path}: {e}")

def process_changed_files():
    username = get_git_user()
    modified_files = get_modified_files()

    if not modified_files:
        print("[ℹ️] No modified files detected.")
        return

    print("\n📦 Git reports these modified files:")
    for f in modified_files:
        print("   →", f)

    print("\n🛠️  Processing eligible .cpp/.hpp files under src/:\n")

    for file in modified_files:
        clean_path = os.path.normpath(file).replace("\\", "/")
        path_from_src = clean_git_path(clean_path)

        if not path_from_src.lower().endswith(('.cpp', '.hpp')):
            print(f"[🚫] Ignored: {path_from_src} (not .cpp/.hpp)")
            continue

        full_path = os.path.abspath(path_from_src)
        update_header_in_file(full_path, username)

if __name__ == "__main__":
    process_changed_files()
    input("\nPress ENTER to exit...")
