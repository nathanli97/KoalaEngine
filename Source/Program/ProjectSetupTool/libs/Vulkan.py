import os

from libs import Logger, Global


def handle_vulkan_not_found():
    print('Failed')
    Logger.error('Vulkan SDK is not found in your system. Tips: Check VK_SDK_PATH and VULKAN_SDK environment '
                 'variables.')
    raise RuntimeError('Vulkan SDK is not found.')


def find_vulkan():
    print(f'Checking Vulkan SDK...\t\t', end='')
    possible_vulkan_env_name = ('VK_SDK_PATH', 'VULKAN_SDK')

    sdk_path = None

    for env_name in possible_vulkan_env_name:
        if env_name in os.environ:
            sdk_path = os.environ[env_name]

    if sdk_path is None:
        handle_vulkan_not_found()

    if not os.path.isfile(
            os.path.join(sdk_path, 'include', 'vulkan', 'vulkan.h')
    ):
        sdk_path = None
        handle_vulkan_not_found()

    print(f'Found at {sdk_path}')

    if Global.args.env_cache:
        with open(os.path.join(Global.source_dir, 'BuildEnv.gen.cmake'), 'wt', encoding='utf-8') as file:
            lines = [
                'macro(set_koala_build_env)\n',
                f'\tset(ENV{{VULKAN_SDK}} "{sdk_path.replace('\\', '/')}")\n',
                'endmacro()\n'
            ]
            file.writelines(lines)

    return sdk_path
