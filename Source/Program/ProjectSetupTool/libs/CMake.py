import re
import subprocess

from libs import Global

cmake = None


class CMakeVersion:
    major = 0
    minor = 0
    patch = 0

    def __init__(self, major, minor, patch):
        self.major = major
        self.minor = minor
        self.patch = patch

    def __eq__(self, other):
        return self.major == other.major and self.minor == other.minor and self.patch == other.patch

    def __gt__(self, other):
        return self.major == other.major and self.minor == other.minor and self.patch > other.patch or \
            self.major == other.major and self.minor > other.minor or \
            self.major > other.major

    def __lt__(self, other):
        return not self.__eq__(other) and not self.__gt__(other)

    def __str__(self):
        return f'{self.major}.{self.minor}.{self.patch}'

    def __ge__(self, other):
        return self.__gt__(other) or self.__eq__(other)


def parse_cmake_version(in_version_output: str) -> CMakeVersion:
    m = re.match(r'cmake version (\d+)\.(\d+)\.(\d+)', in_version_output)
    if m is None:
        return CMakeVersion(0, 0, 0)  #Unknown version
    return CMakeVersion(int(m.group(1)), int(m.group(2)), int(m.group(3)))


def find_cmake():
    cmake_version = None
    proc = subprocess.run('cmake --version', shell=True, capture_output=True)
    if proc is not None and proc.returncode == 0:
        cmake_version = parse_cmake_version(proc.stdout.decode('utf-8'))
        if cmake_version >= CMakeVersion(3, 25, 0):
            return 'cmake'

    if cmake_version is None:
        raise RuntimeError(f'Could not find cmake')
    else:
        raise RuntimeError(f'CMake version is too old: required 3.25.0, found {cmake_version}')


def available():
    return cmake is not None


def run_cmake(cmd: str or list, cwd=Global.source_dir, capture_output=True, stdout=None, stderr=None):
    if isinstance(cmd, list):
        command = ' '.join(cmd)
    else:
        command = cmd
    assert available()

    if capture_output:
        proc = subprocess.run(f'{cmake} {command}', shell=True, capture_output=True, check=True, cwd=cwd, stdout=stdout, stderr=stderr)
        return proc.stdout.decode('utf-8')
    else:
        proc = subprocess.run(f'{cmake} {command}', shell=True, check=True, cwd=cwd, stdout=stdout, stderr=stderr)
        return None


def setup_cmake():
    global cmake
    if available():
        return
    cmake = find_cmake()
