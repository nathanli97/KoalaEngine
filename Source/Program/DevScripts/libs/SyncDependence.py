# This file is used to sync 3rdparty dependencies.
import json
import os
import re
import shutil

from libs import Git, Global


class Dependence:
    name: str = ''
    git: str = ''
    commit: str = ''
    branch: str = ''

    @staticmethod
    def from_match(match):
        dep = Dependence()
        groups = match.groups()
        num_groups = len(groups)

        dep.name = groups[0]
        dep.git = groups[1]
        dep.commit = groups[2]

        if num_groups >= 4:
            dep.branch = groups[3]
        return dep

    def sync(self, args):
        root_path = os.path.join(Global.source_dir, '3rdparty')

        if not os.path.isdir(root_path):
            os.makedirs(root_path)

        path = os.path.join(root_path, self.name)

        # clone
        if os.path.isdir(path) and not Git.is_git_repo(path):
            shutil.rmtree(path, onexc=Global.on_rm_error)
            Git.clone(root_path, self.git, self.name)
        elif not os.path.isdir(path):
            Git.clone(root_path, self.git, self.name)

        if len(self.branch) > 0:
            Git.checkout(path, self.branch)
        if len(self.commit) > 0:
            Git.checkout(path, self.commit)


def get_dependencies():
    deps = []
    with open(os.path.join(Global.source_dir, "Dependencies.txt"), encoding='utf-8') as file:
        lines = file.readlines()
        regex_git = (
            r"Name *= *'(.+)' *, *Git *= *'(.+)' *, *Commit *= *'(.+)' *, *Branch *= *'(.+)'",
            r"Name *= *'(.+)' *, *Git *= *'(.+)' *, *Commit *= *'(.+)'"
        )
        for line in lines:
            line = line.replace('\n', '')
            line = line.replace('\r', '')

            while line.startswith(' ') or line.startswith('\t'):
                line = line[1:]
            while line.endswith(' ') or line.endswith('\t'):
                line = line[:-1]

            if line.startswith('#') or len(line) == 0:
                continue

            for regex in regex_git:
                match = re.match(regex, line)
                if match:
                    deps.append(Dependence.from_match(match))
                    break
            if match is None:
                raise RuntimeError('Invalid dependencies.txt file')
    return deps


def sync_dependencies(args):
    print(f'Syncing dependencies...')
    deps = get_dependencies()

    for dep in deps:
        if args.verbose:
            print(f'Syncing dependence {dep.name}...')
        dep.sync(args)
    print(f'Dependencies Sync Complete')


def need_sync_dependencies(args):
    Git.setup_git(args)
    print('Checking for dependencies...')
    deps = get_dependencies()

    for dep in deps:
        path = os.path.join(Global.source_dir, '3rdparty', dep.name)
        if not os.path.isdir(path):
            if args.verbose:
                print(
                    f'[need_sync_dependencies]: Need sync dependencies because {dep.name} repo directory does not exist')
            return True
        if not Git.is_git_repo(path):
            if args.verbose:
                print(
                    f'[need_sync_dependencies]: Need sync dependencies because {dep.name} repo directory is invalid')
            return True
        commit = Git.get_commit(path)
        if commit != dep.commit:
            if args.verbose:
                print(
                    f'[need_sync_dependencies]: Need sync dependencies because {dep.name} commit not match: REQUIRED:{dep.commit} vs LOCAL:{commit}')
            return True
    print('Dependencies check passed')
    return False
