# PlatformIO resolves the local Bosch archive relative to this library.
Import('env')
from os.path import join, exists
from SCons.Script import Dir

cpu = env.BoardConfig().get('build.mcu', '')
path = join(Dir('.').srcnode().abspath, 'src', cpu)
if not exists(join(path, 'libalgobsec.a')):
    raise RuntimeError(
        f'BSEC3 archive missing for {cpu}. In this library run: '
        f'python3 scripts/install_bsec.py /path/to/bsec_v3-3-0-1.zip --mcu {cpu}. '
        'Supported installer targets: esp32, esp32s2, esp32s3, esp32c3.'
    )
env.Append(LIBPATH=[path], LIBS=['algobsec'])
