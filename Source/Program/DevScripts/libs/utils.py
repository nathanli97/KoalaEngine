import urllib.request


def download_file(url, path):
    urllib.request.urlretrieve(url, path)
