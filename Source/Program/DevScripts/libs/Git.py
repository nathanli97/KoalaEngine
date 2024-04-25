import os
import platform
import subprocess

from libs import Global, Lib7Zip, Download

git = ''


def available():
    return len(git) > 0


def find_git(force_local_git=False):
    global git
    git_exe = os.path.join(Global.source_dir, '3rdparty', 'Git', 'cmd', 'git.exe')
    if not force_local_git:
        proc = subprocess.run('git -v', shell=True, capture_output=True)
        if proc is not None and proc.returncode == 0:
            git = 'git'
    if os.path.isfile(git_exe):
        git = git_exe


def run_git(cmd: str or list, cwd=Global.source_dir):
    command = ''
    if isinstance(cmd, list):
        command = ' '.join(cmd)
    else:
        command = cmd
    assert available()
    proc = subprocess.run(f'{git} {command}', shell=True, capture_output=True, check=True, cwd=cwd)
    return proc.stdout.decode('utf-8')


def is_git_repo(path: str):
    return os.path.isfile(os.path.join(path, '.git', 'HEAD'))


def get_commit(path: str) -> str:
    commit = run_git('rev-parse HEAD', cwd=path)
    return commit.strip()


def fetch(path: str):
    return run_git('fetch -P', cwd=path)


def checkout(path: str, commit_or_branch: str):
    return run_git(f'checkout {commit_or_branch}', cwd=path)


def setup_git(args):
    global git
    if not args.download_all:
        find_git()
        if available():
            return
        if platform.platform() != 'Windows':
            raise RuntimeError('Git is not found in your system')  #Currently auto-downloaded git only works on Windows.
    Lib7Zip.setup(args)
    print('Setting up Git for windows...')
    git_url = 'https://github.com/git-for-windows/git/releases/download/v2.44.0.windows.1/MinGit-2.44.0-busybox-64-bit.zip'
    path = Download.download_to_cache(git_url, 'MinGit-2.44.0-busybox-64-bit.zip')
    dst_path = os.path.join(Global.source_dir, '3rdparty', 'Git')
    Lib7Zip.unzip_file(path, dst_path)
    git = os.path.join(dst_path, 'cmd', 'git.exe')
    assert os.path.isfile(git)
