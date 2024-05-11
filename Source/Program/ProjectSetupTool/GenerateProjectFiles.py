#!/usr/bin/python
import argparse
import os.path
import platform
import shutil
import subprocess
import sys

from libs import Global, Dependencies, Logger, Git, Vulkan, CMake
from libs.SourceFiles import gather_source
from libs.VisualStudio import select_generator_visualstudio, select_arch_visualstudio


def select_generator():
    if Global.args.mingw:
        return 'MinGW Makefiles'
    elif Global.args.unixmake:
        return 'Unix Makefiles'
    elif Global.args.nmake:
        return 'NMake Makefiles'
    elif Global.args.xcode:
        return 'Xcode'
    elif Global.args.msys:
        return 'MSYS Makefiles'
    elif Global.args.vs:
        vs_version = int(Global.args.vs)
        return select_generator_visualstudio(vs_version)

    if platform.system() == "Windows":
        return select_generator_visualstudio()
    elif platform.system() == "Linux":
        return 'Unix Makefiles'
    elif platform.system() == "Darwin":
        return 'Xcode'


def generate(generator, arch):
    target_path = os.path.join(Global.source_dir, 'ThirdParty', 'Target')

    if arch is not None:
        command = f'-B "{Global.build_dir}" -S "{Global.source_dir}" -DCMAKE_PREFIX_PATH="{target_path}" -G "{generator}" -A {arch}'
    else:
        command = f'-B "{Global.build_dir}" -S "{Global.source_dir}" -DCMAKE_PREFIX_PATH="{target_path}" -G "{generator}"'
    if Global.args.test:
        command += f' -DBUILD_TESTS=1'
    if os.path.isfile(os.path.join(Global.build_dir, '.clang-format')):
        os.remove(os.path.join(Global.build_dir, '.clang-format'))

    shutil.copy(os.path.join(Global.source_dir, '.clang-format'), os.path.join(Global.build_dir, '.clang-format'))

    if Global.args.env_cache:
        with open(os.path.join(Global.source_dir, 'BuildEnv.gen.cmake'), 'wt', encoding='utf-8') as file:
            lines = [
                'macro(set_koala_build_env)\n',
                f'\tset(CMAKE_PREFIX_PATH "{target_path.replace('\\', '/')}")\n',
            ]
            if Vulkan.available():
                vksdk = Vulkan.get_vksdk()
                lines.append(f'\tset(ENV{{VULKAN_SDK}} "{vksdk.replace('\\', '/')}")\n')
            lines.append('endmacro()\n')
            file.writelines(lines)

    if Global.args.verbose:
        Logger.verbose(f'Running cmake {command}')
        CMake.run_cmake(command)
    else:
        print(f'Running cmake for project files generation...')
        with open(os.path.join(Global.build_dir, 'CMake_ProjectGeneration.log'), 'w', encoding='utf8') as file:
            try:
                CMake.run_cmake(command, stdout=file, stderr=file)
            except subprocess.CalledProcessError:
                Logger.error(
                    f'Failed to generate project files. For more details, see {Global.build_dir}/CMake_ProjectGeneration.log')
                sys.exit(1)


def clean():
    print(f'Cleaning CMake build related files ...')
    if os.path.isfile(os.path.join(Global.build_dir, 'CMakeCache.txt')):
        os.remove(os.path.join(Global.build_dir, 'CMakeCache.txt'))
    if os.path.isdir(os.path.join(Global.build_dir, 'CMakeFiles')):
        shutil.rmtree(os.path.join(Global.build_dir, 'CMakeFiles'), onexc=Global.on_rm_error)
    if os.path.isfile(os.path.join(Global.source_dir, 'BuildEnv.gen.cmake')):
        os.remove(os.path.join(Global.source_dir, 'BuildEnv.gen.cmake'))


def main():
    Global.set_source_dir(os.getcwd())
    CMake.setup_cmake()

    if not os.path.isdir(Global.build_dir):
        os.makedirs(Global.build_dir)

    if os.path.isfile(os.path.join(Global.source_dir, 'GenerateProjectFiles.py')):
        Global.set_source_dir(os.path.join(
            Global.source_dir,
            '..', '..', '..'
        ))

    parser = argparse.ArgumentParser(
        description='Generate project files for KoalaEngine'
    )

    parser.add_argument('--build_dir', action='store', required=False,
                        help='Where project files will be written. Default is "SOURCE_DIE/Build" directory')
    parser.add_argument('--vs', action='store', required=False, help='Specify Visual Studio Version')
    parser.add_argument('--mingw', action='store_true', required=False, help='Use MinGW Makefiles')
    parser.add_argument('--unixmake', action='store_true', required=False, help='Use Unix Makefiles')
    parser.add_argument('--nmake', action='store_true', required=False, help='Use NMake Makefiles')
    parser.add_argument('--xcode', action='store_true', required=False, help='Use Xcode Makefiles')
    parser.add_argument('--msys', action='store_true', required=False, help='Use MSYS Makefiles')
    parser.add_argument('--arch', action='store', required=False, help='Specify arch (Visual Studio Only)')
    parser.add_argument('--noclean', action='store_true', required=False,
                        help='Donot clean build directory before generate project files')
    parser.add_argument('--no_gather_files', action='store_true', required=False,
                        help='Donot Gather source files and update SourceFiles.gen.cmake file(s)')
    parser.add_argument('--verbose', action='store_true', required=False, help='Verbose mode')
    parser.add_argument('--test', action='store_true', required=False, help='Generate project files for unit tests')
    parser.add_argument('--env_cache', action='store_true', required=False, help='Let cmake use cached environment '
                                                                                 'variables from this script')

    args = parser.parse_args()
    Global.set_args(args)
    Git.setup_git()

    if args.verbose:
        Logger.enable_verbose()

    if args.build_dir:
        Global.set_build_dir(args.build_dir)

    Vulkan.setup_vulkan()
    generator = select_generator()
    arch = None

    if generator.startswith('Visual Studio'):
        arch = select_arch_visualstudio()

    if not os.path.isfile(os.path.join(Global.source_dir, 'ThirdParty/.sync_ok')):
        if Dependencies.need_sync_dependencies():
            Logger.error('Need to sync the dependencies before generating project files! Please run Setup.')
            exit(1)
    Logger.info(f'Generating project files for {generator}')
    Logger.info(f'Project files will be written to "{Global.build_dir}"')

    if not args.noclean:
        clean()
    if not args.no_gather_files:
        gather_source()
    generate(generator, arch)
    print('Generation Finished')


if __name__ == '__main__':
    main()
