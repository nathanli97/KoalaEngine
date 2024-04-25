import os.path


def download_3rdparty(url, name):
    filename = os.path.basename(url)
    cache_directory = os.path.join(os.path.expanduser('~'), '.cache', 'koala')
    if not os.path.exists(cache_directory):
        os.makedirs(cache_directory)
    file_path = os.path.join(cache_directory, filename)
    if os.path.exists(file_path):
        pass


def check_required_3rdparty(source_dir: str):
    path_libs = os.path.join(source_dir, '3rdparty')
    path_git = os.path.join(source_dir, '.git')

    if os.path.exists(path_git):
        pass
