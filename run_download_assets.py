import os
import urllib.request
import zipfile


ASSETS_URL = "https://github.com/alfonsmagd/IFNITY-ENGINE/releases/download/v1.0-assets/IfnitiDataScenes.zip"
ZIP_FILENAME = "IfnitiDataScenes.zip"
SCRIPT_DIR = os.path.dirname(__file__)
DATA_DIR = os.path.join(SCRIPT_DIR, "data")
ZIP_PATH = os.path.join(DATA_DIR, ZIP_FILENAME)

def data_exists(path):
    return os.path.exists(path) and any(os.scandir(path))

def download_file(url, dest_path):
    print(f"Downloading from: {url}")
    urllib.request.urlretrieve(url, dest_path)
    print("Download completed.")

def unzip_file(zip_path, extract_to):
    print(f"Extracting to: {extract_to}")
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(extract_to)
    print("Extraction completed.")

def list_files(path):
    print("\nAvailable data files:")
    for root, _, files in os.walk(path):
        for file in files:
            full_path = os.path.relpath(os.path.join(root, file), path)
            print(f"- {full_path}")

def main():
    if data_exists(DATA_DIR):
        print("Data already exists. Skipping download.")
    else:
        os.makedirs(DATA_DIR, exist_ok=True)
        download_file(ASSETS_URL, ZIP_PATH)
        unzip_file(ZIP_PATH, DATA_DIR)
        os.remove(ZIP_PATH)
        print("ZIP file removed.")

    list_files(DATA_DIR)


if __name__ == "__main__":
    main()