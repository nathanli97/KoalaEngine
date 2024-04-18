import argparse
import os
import platform
import re
import subprocess
import urllib.request


def download_7zip(root):
    print(f'Downloading 7-Zip....')
    download_dir = os.path.join(root, 'DownloadCache')
    trd_dir = os.path.join(root, '3rdparty')

    dir_7z = os.path.join(root, '3rdparty', '7zip')

    if not os.path.exists(download_dir):
        os.makedirs(download_dir)

    if not os.path.exists(dir_7z):
        os.makedirs(dir_7z)

    file_7zr = os.path.join(dir_7z, '7zr.exe')
    file_7z = os.path.join(dir_7z, '7z2404-extra.7z')

    urllib.request.urlretrieve('https://www.7-zip.org/a/7zr.exe', file_7zr)
    urllib.request.urlretrieve('https://www.7-zip.org/a/7z2404-extra.7z', file_7z)

    subprocess.run(f'{file_7zr} x -y "{file_7z}" -o"{dir_7z}"', stdout=subprocess.DEVNULL)


def unzip(root, zipped_file_path, to_path):
    print(f'Unzipping {zipped_file_path}...')
    if platform.system() == 'Windows':
        dir_7z = os.path.join(root, '3rdparty', '7zip')
        file_7za = os.path.join(dir_7z, '7za.exe')

        if not os.path.isfile(file_7za):
            download_7zip(root)
    else:
        file_7za = '7za'

    subprocess.run(f'{file_7za} x -y "{zipped_file_path}" -o"{to_path}"', stdout=subprocess.DEVNULL)


def process_deps(dep_file, root):
    download_dir = os.path.join(root, 'DownloadCache')

    if not os.path.exists(download_dir):
        os.makedirs(download_dir)

    trd_dir = os.path.join(root, '3rdparty')
    with open(dep_file, 'r') as f:
        deps = f.readlines()

        for dep in deps:
            if dep.startswith('#'):
                continue
            dep = dep.replace('\n', '')
            dep = dep.replace('\r', '')
            m = re.match(r'(.+?)\|(.+?)\|(.+?)\|(.+)$', dep)

            if m:
                name = m.group(1)
                ver = m.group(2)
                filename = m.group(3)
                url = m.group(4)

                dep_file_download = os.path.join(download_dir, filename)
                dep_dir = os.path.join(trd_dir, f'{name}-{ver}')

                if os.path.isfile(os.path.join(dep_dir, f'.{ver}')):
                    continue

                print(f'Downloading {url}...')
                urllib.request.urlretrieve(url, dep_file_download)
                unzip(root, dep_file_download, trd_dir)

                with open(os.path.join(dep_dir, f'.{ver}'), 'w'):
                    pass


def main():
    parser = argparse.ArgumentParser(
        prog='DependenciesSync'
    )

    parser.add_argument('dep_file')
    parser.add_argument('root')

    args = parser.parse_args()
    process_deps(args.dep_file, args.root)


if __name__ == '__main__':
    main()
