import os
import stat

source_dir = os.path.join(
    os.getcwd()
)
build_dir = os.path.join(source_dir, "Build")


def set_source_dir(in_source_dir):
    global source_dir
    global build_dir
    source_dir = in_source_dir
    build_dir = os.path.join(source_dir, "Build")


def set_build_dir(in_build_dir):
    global build_dir
    build_dir = in_build_dir


def on_rm_error(func, path, _):
    os.chmod(path, stat.S_IWRITE)
    os.unlink(path)