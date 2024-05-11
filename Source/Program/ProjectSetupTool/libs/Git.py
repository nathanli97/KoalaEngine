import os
import subprocess
from urllib import request

from libs import Global

git = ''


def available():
    return len(git) > 0


def find_git():
    global git
    proc = subprocess.run('git --version', shell=True, capture_output=True)
    if proc is not None and proc.returncode == 0:
        git = 'git'


def run_git(cmd: str or list, cwd=Global.source_dir, capture_output=True):
    proxies = request.getproxies()
    for proxy in proxies:
        if proxy == 'http':
            os.environ['http_proxy'] = proxies[proxy]
        elif proxy == 'https':
            os.environ['https_proxy'] = proxies[proxy]
    command = ''
    if isinstance(cmd, list):
        command = ' '.join(cmd)
    else:
        command = cmd
    assert available()
    if capture_output:
        proc = subprocess.run(f'{git} {command}', shell=True, capture_output=True, check=True, cwd=cwd)
        return proc.stdout.decode('utf-8')
    else:
        proc = subprocess.run(f'{git} {command}', shell=True, check=True, cwd=cwd)
        return None


def is_git_repo(path: str):
    return os.path.isfile(os.path.join(path, '.git', 'HEAD'))


def get_commit(path: str) -> str:
    commit = run_git('rev-parse HEAD', cwd=path)
    return commit.strip()


def clone(run_on_path: str, url: str, name: str):
    run_git(f'clone {url} {name}', run_on_path, capture_output=False)


def fetch(path: str):
    return run_git('fetch -P', cwd=path)


def checkout(path: str, commit_or_branch: str):
    ret = run_git(f'checkout {commit_or_branch}', cwd=path)
    run_git(f'reset --hard', cwd=path)
    return ret


def setup_git():
    global git
    if available():
        return
    find_git()
    assert available()
