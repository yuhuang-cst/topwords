"""
@Author: Yu Huang
@Email: yuhuang-cst@foxmail.com
"""

from setuptools import setup, find_packages, Extension
import sys

if sys.version_info.major != 3:
    raise RuntimeError('TopWORDS requires Python 3')

# Set this to True to enable building extensions using Cython.
# Set it to False to build extensions from the C file (that
# was previously created using Cython).
USE_CYTHON = False

install_requires = open('requirements.txt').read().splitlines()
cmdclass = {}
include_dirs = ['topwords']

if USE_CYTHON:
    from Cython.Distutils import build_ext
    install_requires.append('cython >= 0.26.1')
    sources = ['topwords/py2cpp.pyx', 'topwords/topwords_lib.cpp']
    cmdclass['build_ext'] = build_ext
else:
    sources = ['topwords/py2cpp.by_cython.cpp', 'topwords/topwords_lib.cpp']

long_description = '''
This is an implementation of TopWORDS. TopWORDS is 
a unigram language model, which assumes words are independent and 
the probability of a sentence is the cumprod of probablitie of words.
We use EM algorithm to optimize parameters.

Reference: 
[1] Deng, K. (2016). On the unsupervised analysis of domain-specific Chinese texts.
'''

setup(
    name='topwords',
    description='An implementation of TopWORDS',
    long_description=long_description,
    version='1.0.0',
    author='Yu Huang',
    author_email='yuhuang-cst@foxmail.com',
    packages=find_packages(),
    zip_safe=False,
    url='https://github.com/yuhuang-cst/topwords',
    license='LICENSE',
    install_requires=install_requires,
    cmdclass = cmdclass,
    ext_modules=[
        Extension(
            'topwords/py2cpp',
            sources=sources,
            include_dirs=include_dirs,
            depends=["topwords/topwords_lib.h"],
            language='c++',
            extra_compile_args=['-std=c++17', '-O2', '-Wall', '-fopenmp'],
            extra_link_args=['-std=c++17', '-fopenmp'],
        ),
    ],
)

