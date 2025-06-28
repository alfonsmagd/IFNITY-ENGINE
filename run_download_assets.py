import os
import urllib.request
import zipfile

ASSETS_URL = "https://github.com/alfonsmagd/IFNITY-ENGINE/releases/download/v1.0.0/data.zip"
ZIP_FILENAME = "data.zip"
SCRIPT_DIR = os.path.dirname(__file__)
DATA_DIR = os.path.join(SCRIPT_DIR, "data")
ZIP_PATH = os.path.join(SCRIPT_DIR, ZIP_FILENAME)

def download_file(url, dest_path):
    print(f"Downloading from: {url}")
    urllib.request.urlretrieve(url, dest_path)
    print("Download completed.")

def unzip_file(zip_path, extract_to, zip_has_data_folder):
    print(f"Extracting to: {extract_to}")
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        if zip_has_data_folder:
            # Descomprimir directamente en el SCRIPT_DIR
            zip_ref.extractall(extract_to)
        else:
            # Si no tiene carpeta data/, extraer en DATA_DIR
            os.makedirs(DATA_DIR, exist_ok=True)
            zip_ref.extractall(DATA_DIR)
    print("Extraction completed.")

def zip_contains_data_folder(zip_path):
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        for member in zip_ref.namelist():
            # Verifica si existe 'data/' en el zip
            if member.startswith('data/'):
                return True
    return False

def list_files(path):
    print("\nAvailable data files:")
    for root, _, files in os.walk(path):
        for file in files:
            full_path = os.path.relpath(os.path.join(root, file), path)
            print(f"- {full_path}")

def main():
    if os.path.exists(DATA_DIR) and any(os.scandir(DATA_DIR)):
        print("Data folder already exists. Skipping download and extraction.")
    else:
        download_file(ASSETS_URL, ZIP_PATH)
        has_data_folder = zip_contains_data_folder(ZIP_PATH)
        if has_data_folder:
            unzip_file(ZIP_PATH, SCRIPT_DIR, zip_has_data_folder=True)
        else:
            unzip_file(ZIP_PATH, SCRIPT_DIR, zip_has_data_folder=False)
        os.remove(ZIP_PATH)
        print("ZIP file removed.")

    list_files(DATA_DIR)

if __name__ == "__main__":
    main()
