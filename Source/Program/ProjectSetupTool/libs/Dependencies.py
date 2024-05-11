# This file is used to sync 3rdparty dependencies.
import multiprocessing
import os
import shutil
import subprocess

import yaml

from libs import Git, Global, Logger, CMake


class Dependence:
    name: str = ''                        # Required. The Name of package.
    git: str = ''                         # Required. The Git url of package
    commit: str = ''                      # Required. The commit SHA of package git repo
    branch: str = ''                      # Optional. The branch name of package git repo
    build_type: str = 'RelWithDebInfo'    # Optional. The build type when compiling this package. Possible values: Debug, Release, RelWithDebInfo
    nobuild: bool = False                 # Optional. When true, Do not build this package.
    install = list()                      # Optional. When nobuild==True, Copying the directories in this list to target dir.
    cmake_build_config = list()           # Optional. Additional cmake build options.   (The CMAKE configuration stage)
    cmake_install_config = list()         # Optional. Additional cmake install options. (The CMAKE build and install stage)
    cmake_custom_configuration_cmd = None # Optional. Override the cmake configuration command
    cmake_custom_build_cmd = None         # Optional. Override the cmake build command

    def sync(self):
        root_path = os.path.join(Global.source_dir, 'ThirdParty', 'Source')

        if not os.path.isdir(root_path):
            os.makedirs(root_path)

        repo_path = os.path.join(root_path, self.name)

        # clone
        if os.path.isdir(repo_path) and not Git.is_git_repo(repo_path):
            shutil.rmtree(repo_path, onexc=Global.on_rm_error)
            Git.clone(Global.source_dir, self.git, f'ThirdParty/Source/{self.name}')
        elif not os.path.isdir(repo_path):
            Git.clone(Global.source_dir, self.git, f'ThirdParty/Source/{self.name}')

        if len(self.branch) > 0:
            Git.checkout(repo_path, self.branch)
        if len(self.commit) > 0:
            Git.checkout(repo_path, self.commit)

    def build(self):
        source_path = os.path.join(Global.source_dir, 'ThirdParty', 'Source', self.name)
        build_path = os.path.join(Global.source_dir, 'ThirdParty', 'Build', self.name)
        target_path = os.path.join(Global.source_dir, 'ThirdParty', 'Target')

        if self.nobuild:
            print(f'The package "{self.name}" not need for build. Skipping building.')

            if len(self.install) > 0:
                for i in self.install:
                    print(f'Installing: {os.path.join(source_path, i)} => {target_path}')
                    shutil.copytree(os.path.join(source_path, i), os.path.join(target_path, i), dirs_exist_ok=True)
            return

        required_commit = Git.get_commit(source_path)

        if not os.path.isdir(build_path):
            os.makedirs(build_path)

        if not os.path.isdir(target_path):
            os.makedirs(target_path)

        build_commit = None
        if os.path.isfile(os.path.join(build_path, '.build_ok')):
            with open(os.path.join(build_path, '.build_ok'), 'rt', encoding='utf-8') as f:
                build_commit = f.readline()

        if not build_commit or build_commit != required_commit:
            configuration_cmd = ['-DCMAKE_BUILD_TYPE=RelWithDebInfo', f'-DCMAKE_INSTALL_PREFIX:PATH={target_path}']
            if len(self.cmake_build_config) > 0:
                configuration_cmd.extend(self.cmake_build_config)
            configuration_cmd.append(source_path)

            install_cmd = ['--build', '.', '--target', 'install', f'--parallel {multiprocessing.cpu_count()}']

            if len(self.cmake_install_config) > 0:
                install_cmd.extend(self.cmake_install_config)

            print(f'!!! Configuring dependence {self.name} at commit {required_commit}...')
            try:
                CMake.run_cmake(
                    configuration_cmd, cwd=build_path, capture_output=False)
            except subprocess.CalledProcessError:
                raise RuntimeError(f'Configure dependence {self.name} failed.')

            print(f'!!! Building dependence {self.name} at commit {required_commit}...')
            try:
                CMake.run_cmake(install_cmd, cwd=build_path,
                                capture_output=False)
            except subprocess.CalledProcessError:
                raise RuntimeError(f'Build dependence {self.name} failed.')

        with open(os.path.join(build_path, '.build_ok'), 'wt', encoding='utf-8') as f:
            f.write(self.commit)


def get_dependencies():
    dependencies = []
    with open(os.path.join(Global.source_dir, "Dependencies.yaml"), encoding='utf-8') as file:
        deps = yaml.load(file, Loader=yaml.FullLoader)
        if 'Dependencies' not in deps:
            raise RuntimeError('Invalid Dependencies.yaml. Excepting \'Dependencies\' root node.')
        for name in deps['Dependencies']:
            item = deps['Dependencies'][name]
            dep = Dependence()
            dep.name = name
            if 'git' not in item:
                raise RuntimeError(f'Invalid Dependencies.yaml. Excepting \'git\' node in dependency "{name}".')
            dep.git = item['git']
            if 'commit' in item:
                dep.commit = item['commit']
            if 'branch' in item:
                dep.branch = item['branch']
            if 'cmake_build_config' in item:
                dep.cmake_build_config = item['cmake_build_config']
            if 'cmake_install_config' in item:
                dep.cmake_install_config = item['cmake_install_config']
            if 'cmake_custom_configuration_cmd' in item:
                dep.cmake_custom_configuration_cmd = item['cmake_custom_configuration_cmd']
            if 'cmake_custom_build_cmd' in item:
                dep.cmake_custom_build_cmd = item['cmake_custom_build_cmd']
            if 'nobuild' in item:
                dep.nobuild = item['nobuild']
            if 'install' in item:
                dep.install = item['install']
            dependencies.append(dep)
    return dependencies


def sync_dependencies():
    print(f'Syncing dependencies...')
    deps = get_dependencies()

    for dep in deps:
        Logger.verbose(f'Syncing dependence {dep.name}...')
        dep.sync()
    for dep in deps:
        Logger.verbose(f'Building dependence {dep.name}...')
        dep.build()
    print(f'Dependencies Sync Complete')


def need_sync_dependencies():
    deps = get_dependencies()

    for dep in deps:
        path = os.path.join(Global.source_dir, 'ThirdParty', dep.name)
        if not os.path.isdir(path):
            Logger.verbose(
                f'[need_sync_dependencies]: Need sync dependencies because {dep.name} repo directory does not exist')
            return True
        if not Git.is_git_repo(path):
            Logger.verbose(
                f'[need_sync_dependencies]: Need sync dependencies because {dep.name} repo directory is invalid')
            return True
        commit = Git.get_commit(path)
        if commit != dep.commit:
            Logger.verbose(
                f'[need_sync_dependencies]: Need sync dependencies because {dep.name} commit not match: REQUIRED:{dep.commit} vs LOCAL:{commit}')
            return True
    return False
