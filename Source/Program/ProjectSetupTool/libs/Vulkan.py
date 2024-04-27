import os

from libs import Logger


def find_vulkan():
    print(f'Checking Vulkan SDK...\t\t', end='')
    possible_vulkan_env_name = ('VK_SDK_PATH', 'VULKAN_SDK')

    sdk_path = None

    for env_name in possible_vulkan_env_name:
        if env_name in os.environ:
            sdk_path = os.environ[env_name]

    if not os.path.isfile(
        os.path.join(sdk_path, 'Include', 'vulkan', 'vulkan.h')
    ):
        sdk_path = None

    if not sdk_path:
        print('Failed')
        Logger.error('Vulkan SDK is not found in your system. Tips: Check VK_SDK_PATH and VULKAN_SDK environment '
                     'variables.')
        raise RuntimeError('Vulkan SDK is not found.')

    print(f'Found at {sdk_path}')
    return sdk_path

