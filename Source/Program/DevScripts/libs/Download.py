import os
import urllib.request

from libs import Global


# Download file from 'url' to 'path'.
def download_file(url, path):
    urllib.request.urlretrieve(url, path)


# Download file to local cache.
# Return: Path to downloaded file.
# Arg 'url'     : The URL to download. This function will automatically extract filename from URL.
# Arg 'filename': Optional. If given, override downloaded file name.
# Arg 'force'   : Optional. TRUE=If file already exists in cache, override them.
def download_to_cache(url, filename=None, force=False) -> str:
    cache_directory = os.path.join(Global.source_dir, 'DownloadCache')
    if not os.path.exists(cache_directory):
        os.makedirs(cache_directory)
    if filename is None:
        filename = url.split('/')[-1]

    filepath = os.path.join(cache_directory, filename)
    if not os.path.isfile(filepath) or force:
        download_file(url, filepath)
    return filepath
