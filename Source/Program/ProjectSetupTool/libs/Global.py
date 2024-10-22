import os
import stat

source_dir = os.path.join(
    os.getcwd()
)
build_dir = os.path.join(source_dir, "Build")
args = None


def set_source_dir(in_source_dir):
    global source_dir
    global build_dir
    source_dir = in_source_dir

    if not os.path.isfile(os.path.join(source_dir, 'CMakeLists.txt')):
        raise RuntimeError('Please run this script in the KoalaEngine\'s directory.')
    build_dir = os.path.join(source_dir, "Build")


def set_build_dir(in_build_dir):
    global build_dir
    build_dir = in_build_dir


def on_rm_error(func, path, _):
    os.chmod(path, stat.S_IWRITE)
    os.unlink(path)


def set_args(in_args):
    global args
    args = in_args
