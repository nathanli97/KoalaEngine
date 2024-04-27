import argparse
import os
import shutil

from libs import SyncDependence, Global, Logger, Git, Vulkan


def main():
    Global.set_source_dir(os.getcwd())

    thirdparty_path = os.path.join(Global.source_dir, 'ThirdParty')

    parser = argparse.ArgumentParser(
        description='Sync ThirdParty Dependencies for KoalaEngine'
    )

    parser.add_argument('--clean', action='store_true', required=False,
                        help='Clean third party libraries before sync dependencies')
    parser.add_argument('--verbose', action='store_true', required=False, help='Verbose mode')

    args = parser.parse_args()
    Git.setup_git()

    if args.verbose:
        Logger.enable_verbose()

    # Currently we doesn't need 
    # Vulkan.find_vulkan()

    if args.clean:
        shutil.rmtree('ThirdParty', onexc=Global.on_rm_error)

    if not os.path.isdir(thirdparty_path):
        os.makedirs(thirdparty_path)

    SyncDependence.sync_dependencies(args)

    with open(os.path.join(thirdparty_path, '.sync_ok'), 'w') as f:
        pass


if __name__ == '__main__':
    main()
