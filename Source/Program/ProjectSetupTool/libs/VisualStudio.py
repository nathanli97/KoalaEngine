import os
import platform
import subprocess

from libs import Global
from libs.Download import download_file

vswhere_url = "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe"


def find_latest_vs():
    proc = subprocess.run(f'"{Global.source_dir}/ThirdParty/vswhere/vswhere.exe"', capture_output=True)
    latest_vs_version = 0

    if proc is None or proc.returncode != 0:
        return latest_vs_version

    for line in proc.stdout.decode('utf-8').split('\n'):
        line = line.replace('\r', '')
        if line.startswith('catalog_productLineVersion:'):
            version = line[len('catalog_productLineVersion:'):]
            while version.startswith(' '):
                version = version[1:]
            if int(version) > latest_vs_version:
                latest_vs_version = int(version)
    return latest_vs_version


def select_generator_visualstudio(given_version=None):
    # Find VS
    if given_version is None:
        if not os.path.isfile(os.path.join(Global.source_dir, "ThirdParty", "vswhere", "vswhere.exe")):
            os.makedirs(os.path.join(Global.source_dir, "ThirdParty", "vswhere"), exist_ok=True)
            download_file(
                vswhere_url,
                os.path.join(Global.source_dir, "ThirdParty", "vswhere", "vswhere.exe")
            )
        vs_version = find_latest_vs()
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


def select_arch_visualstudio():
    if Global.args.arch is not None:
        return Global.args.arch

    arch = platform.machine()

    if arch is None:
        return None

    if arch == 'AMD64':
        return 'x64'
    elif arch == 'i386':
        return 'Win32'
    elif arch == 'ARM64':
        return 'ARM64'
