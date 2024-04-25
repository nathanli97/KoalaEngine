import os
import platform
import subprocess

from libs.Download import *
from libs import Global

cmd_7za = ''


# Find path of 7za executable
def find_7za(source_dir: str):
    proc = subprocess.run('7za', shell=True, capture_output=True)
    if proc is not None and proc.returncode != 0:
        return '7za'
    path = os.path.join(source_dir, '3rdparty', '7zip', '7za')
    path_exe = os.path.join(source_dir, '3rdparty', '7zip', '7za.exe')
    if os.path.isfile(path):
        return path
    if os.path.isfile(path_exe):
        return path_exe
    return None


def _unzip_file(cmd: str, src: str, dst: str):
    assert len(cmd) > 0
    subprocess.run(f'{cmd} x -y "{src}" -o"{dst}"', shell=True, capture_output=True, check=True)


# Unzip files using 7zip.
# src: where is the zipped file?
# dst: where the unzipped files stores?
def unzip_file(src: str, dst: str):
    _unzip_file(cmd_7za, src, dst)


# Setup 7zip environment
# It will download 7zr(a small standalone version of 7zip, it can only unzip 7z files)
# and download full version of 7zip(7z2404-extra.7z) to cache dir, unzip it with 7zr.
def setup_windows(args):
    global cmd_7za
    if args.verbose:
        print('Setting up 7zip for windows...')
    url = 'https://www.7-zip.org/a/7z2404-extra.7z'
    print(f'Setting up 7zip...')
    path = os.path.join(Global.source_dir, '3rdparty', '7zip')
    if not os.path.isdir(path):
        os.makedirs(path)
    path_7zr = os.path.join(path, '7zr.exe')
    download_file('https://www.7-zip.org/a/7zr.exe', path_7zr)
    full_7zip = download_to_cache(url)
    if not os.path.isfile(full_7zip):
        download_file(url, full_7zip)
    _unzip_file(path_7zr, full_7zip, path)
    assert os.path.isfile(os.path.join(path, '7za.exe'))
    cmd_7za = os.path.join(path, '7za.exe')


def setup(args):
    global cmd_7za
    if args.verbose:
        print('Setting up 7zip...')
    if not args.download_all:
        proc = subprocess.run('7za', shell=True, capture_output=True)
        if proc is not None and proc.returncode == 0:
            if args.verbose:
                print('7zip is found in SYSTEM PATH')
            cmd_7za = '7za'
            return
    if platform.system() == "Windows":
        setup_windows(args)
        return

    raise RuntimeError('No 7zip is available')
