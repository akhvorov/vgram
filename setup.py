# import os
# import re
# import sys
# import platform
# import subprocess
#
# from setuptools import setup, Extension
# from setuptools.command.build_ext import build_ext
# from distutils.version import LooseVersion
#
#
# class CMakeExtension(Extension):
#     def __init__(self, name, sourcedir=''):
#         Extension.__init__(self, name, sources=[])
#         self.sourcedir = os.path.abspath(sourcedir)
#
#
# class CMakeBuild(build_ext):
#     def run(self):
#         try:
#             out = subprocess.check_output(['cmake', '--version'])
#         except OSError:
#             raise RuntimeError("CMake must be installed to build the following extensions: " +
#                                ", ".join(e.name for e in self.extensions))
#
#         if platform.system() == "Windows":
#             cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
#             if cmake_version < '3.1.0':
#                 raise RuntimeError("CMake >= 3.1.0 is required on Windows")
#
#         for ext in self.extensions:
#             self.build_extension(ext)
#
#     def build_extension(self, ext):
#         extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
#         cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
#                       '-DPYTHON_EXECUTABLE=' + sys.executable]
#
#         cfg = 'Debug' if self.debug else 'Release'
#         build_args = ['--config', cfg]
#
#         if platform.system() == "Windows":
#             cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
#             if sys.maxsize > 2**32:
#                 cmake_args += ['-A', 'x64']
#             build_args += ['--', '/m']
#         else:
#             cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
#             build_args += ['--', '-j2']
#
#         env = os.environ.copy()
#         env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
#                                                               self.distribution.get_version())
#         if not os.path.exists(self.build_temp):
#             os.makedirs(self.build_temp)
#         subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
#         subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)
#
# setup(
#     name='vgram',
#     version='0.1.7',
#     author='Khvorov Aleksandr',
#     author_email='khvorov.aleksandr@gmail.com',
#     description='V-gram builder library',
#     long_description='Dictionary of v-gram construction',
#     url='https://github.com/akhvorov/vgram',
#     ext_modules=[CMakeExtension('vgram')],  # ext_modules=[CMakeExtension('vgram')],
#     cmdclass=dict(build_ext=CMakeBuild),
#     #zip_safe=False,
#     #packages=['core'],
#     install_requires=['msgpack']
# )

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools

__version__ = '0.2.0'


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


ext_modules = [
    Extension(
        'vgram',
        [
            'src/main/cpp/int_dict.cc',
            'src/main/cpp/int_dict_p.cc',

            'src/main/cpp/stat_dict_p.cc',

            'src/main/cpp/fast_random.cc',

            'src/main/cpp/int_vgram_builder_p.cc',

            'src/main/cpp/seq_coder.cc',

            'src/main/cpp/list_dict.cc',
            'src/main/cpp/vgram_builder.cc',

            'src/main/cpp/main.cc',
            'src/main/cpp/py_vgram_builder.cc',
            'src/main/cpp/base_tokenizer.cc',
            'src/main/cpp/char_tokenizer.cc'
         ],
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True)
        ],
        language='c++'
    ),
]


# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cc') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True


def cpp_flag(compiler):
    """Return the -std=c++[11/14] compiler flag.
    The c++14 is prefered over c++11 (when it is available).
    """
    if has_flag(compiler, '-std=c++14'):
        return '-std=c++14'
    elif has_flag(compiler, '-std=c++11'):
        return '-std=c++11'
    else:
        raise RuntimeError('Unsupported compiler -- at least C++11 support '
                           'is needed!')


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': [],
    }

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)

setup(
    name='vgram',
    version=__version__,
    author='Khvorov Aleksandr',
    author_email='khvorov.aleksandr@gmail.com',
    description='V-gram builder library',
    long_description='Dictionary of v-gram construction',
    url='https://github.com/akhvorov/vgram',

    ext_modules=ext_modules,
    install_requires=['pybind11', 'msgpack'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)