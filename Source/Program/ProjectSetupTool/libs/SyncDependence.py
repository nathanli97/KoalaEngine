# This file is used to sync 3rdparty dependencies.
import os
import shutil
import yaml

from libs import Git, Global, Logger


class Dependence:
    name: str = ''
    git: str = ''
    commit: str = ''
    branch: str = ''

    def sync(self):
        root_path = os.path.join(Global.source_dir, 'ThirdParty')

        if not os.path.isdir(root_path):
            os.makedirs(root_path)

        repo_path = os.path.join(root_path, self.name)

        # clone
        if os.path.isdir(repo_path) and not Git.is_git_repo(repo_path):
            shutil.rmtree(repo_path, onexc=Global.on_rm_error)
            Git.clone(Global.source_dir, self.git, f'ThirdParty/{self.name}')
        elif not os.path.isdir(repo_path):
            Git.clone(Global.source_dir, self.git, f'ThirdParty/{self.name}')

        if len(self.branch) > 0:
            Git.checkout(repo_path, self.branch)
        if len(self.commit) > 0:
            Git.checkout(repo_path, self.commit)


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
            dependencies.append(dep)
    return dependencies


def sync_dependencies():
    print(f'Syncing dependencies...')
    deps = get_dependencies()

    for dep in deps:
        Logger.verbose(f'Syncing dependence {dep.name}...')
        dep.sync()
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
