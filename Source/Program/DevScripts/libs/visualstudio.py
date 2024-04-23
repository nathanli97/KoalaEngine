import os
import subprocess

from libs.utils import download_file

vswhere_url = "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe"


def find_latest_vs(source_dir):
    proc = subprocess.run(f'"{source_dir}/3rdparty/vswhere/vswhere.exe"', capture_output=True)
    latest_vs_version = 0
    for line in proc.stdout.decode('utf-8').split('\n'):
        line = line.replace('\r', '')
        if line.startswith('catalog_productLineVersion:'):
            version = line[len('catalog_productLineVersion:'):]
            while version.startswith(' '):
                version = version[1:]
            if int(version) > latest_vs_version:
                latest_vs_version = int(version)
    return latest_vs_version


def select_generator_visualstudio(source_dir, given_version=None):
    # Find VS
    if given_version is None:
        if not os.path.isfile(os.path.join(source_dir, "3rdparty", "vswhere", "vswhere.exe")):
            os.makedirs(os.path.join(source_dir, "3rdparty", "vswhere"), exist_ok=True)
            download_file(
                vswhere_url,
                os.path.join(source_dir, "3rdparty", "vswhere", "vswhere.exe")
            )
        vs_version = find_latest_vs(source_dir)
        if vs_version == 0:
            raise RuntimeError(f"Cannot find VisualStudio in your computer!")
    else:
        vs_version = given_version
    if vs_version == 2022:
        return 'Visual Studio 17 2022'
    elif vs_version == 2019:
        return 'Visual Studio 16 2019'
    elif vs_version == 2015:
        return 'Visual Studio 15 2015'
    else:
        raise RuntimeError(f"Unsupported VS version: {vs_version}")
