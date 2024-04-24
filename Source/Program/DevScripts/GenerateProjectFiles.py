#/usr/bin/python
import argparse
import os.path
import platform
import re
import shutil
import subprocess

from libs.SourceFiles import gather_source
from libs.visualstudio import select_generator_visualstudio, select_arch_visualstudio

source_dir = os.path.join(
    os.getcwd()
)
build_dir = os.path.join(source_dir, "Build")


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
    proc = subprocess.run(f'{source_dir}/3rdparty/cmake/cmake --version', shell=True, stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL)
    if proc and proc.returncode == 0:
        cmake_version = parse_cmake_version(proc.stdout.decode('utf-8'))
        if cmake_version >= CMakeVersion(3, 25, 0):
            return f'{source_dir}/3rdparty/cmake/cmake'

    if cmake_version is None:
        raise RuntimeError(f'Could not find cmake')
    else:
        raise RuntimeError(f'CMake version is too old: required 3.25.0, found {cmake_version}')


def select_generator(args):
    if args.mingw:
        return 'MinGW Makefiles'
    elif args.unixmake:
        return 'Unix Makefiles'
    elif args.nmake:
        return 'NMake Makefiles'
    elif args.xcode:
        return 'Xcode'
    elif args.msys:
        return 'MSYS Makefiles'
    elif args.vs:
        vs_version = int(args.vs)
        return select_generator_visualstudio(source_dir, vs_version)

    if platform.system() == "Windows":
        return select_generator_visualstudio(source_dir)
    elif platform.system() == "Linux":
        return 'Unix Makefiles'
    elif platform.system() == "Darwin":
        return 'Xcode'


def generate(cmake, generator, arch, args):
    if arch is not None:
        command = f'{cmake} -B "{build_dir}" -S "{source_dir}" -G "{generator}" -A {arch}'
    else:
        command = f'{cmake} -B "{build_dir}" -S "{source_dir}" -G "{generator}"'
    if args.verbose:
        print(f'Running {command}')
        subprocess.run(command, shell=True, check=True)
    else:
        print(f'Running cmake for project files generation...')
        subprocess.run(command, shell=True, check=True, stdout=subprocess.DEVNULL)


def clean():
    global build_dir
    print(f'Cleaning CMake related files in {build_dir}...')
    if os.path.isfile(os.path.join(build_dir, 'CMakeCache.txt')):
        os.remove(os.path.join(build_dir, 'CMakeCache.txt'))
    if os.path.isdir(os.path.join(build_dir, 'CMakeFiles')):
        shutil.rmtree(os.path.join(build_dir, 'CMakeFiles'))


def main():
    global source_dir
    global build_dir

    if os.path.isfile(os.path.join(source_dir, 'GenerateProjectFiles.py')):
        source_dir = os.path.join(
            source_dir,
            '..', '..', '..'
        )
        build_dir = os.path.join(source_dir, "Build")

    parser = argparse.ArgumentParser(
        description='Generate project files for KoalaEngine'
    )

    parser.add_argument('--vs', action='store', required=False, help='Specify Visual Studio Version')
    parser.add_argument('--mingw', action='store_true', required=False, help='Use MinGW Makefiles')
    parser.add_argument('--unixmake', action='store_true', required=False, help='Use Unix Makefiles')
    parser.add_argument('--nmake', action='store_true', required=False, help='Use NMake Makefiles')
    parser.add_argument('--xcode', action='store_true', required=False, help='Use Xcode Makefiles')
    parser.add_argument('--msys', action='store_true', required=False, help='Use MSYS Makefiles')
    parser.add_argument('--arch', action='store', required=False, help='Specify arch (Visual Studio Only)')
    parser.add_argument('--noclean', action='store_true', required=False, help='Donot clean build directory before generate project files')
    parser.add_argument('--no_gather_files', action='store_true', required=False, help='Donot Gather source files and update SourceFiles.gen.cmake file(s)')
    parser.add_argument('--verbose', action='store_true', required=False, help='Verbose mode')
    parser.add_argument('--nogen', action='store_true', required=False, help='Do not generate project files!')

    args = parser.parse_args()
    cmake = find_cmake()
    generator = select_generator(args)
    arch = None
    if generator.startswith('Visual Studio'):
        arch = select_arch_visualstudio(args)

    print(f'Generating project files for {generator}')
    if not args.noclean:
        clean()
    if not args.no_gather_files:
        gather_source(source_dir, args)
    if not args.nogen:
        generate(cmake, generator, arch, args)
    print('Finished')


if __name__ == '__main__':
    main()
