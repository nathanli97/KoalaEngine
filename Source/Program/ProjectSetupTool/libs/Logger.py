import sys

verbose_output = False


def log(level: str, message: str, file=None):
    print(f'{level}{message}', file=file)


def info(message: str):
    log('', message)


def error(message: str):
    log('Error: ', message, file=sys.stderr)


def verbose(message: str):
    if verbose_output:
        log('Debug: ', message)


def enable_verbose():
    global verbose_output
    verbose_output = True
