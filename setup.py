from distutils.core import setup
from setuptools import Extension
from Cython.Build import cythonize

setup(
    ext_modules=cythonize([
        Extension("coro",
                  language="c++",
                  sources=[
                      "src/coro.pyx",
                      "src/lwan-array.c",
                      "src/lwan-coro.c"
                  ]),
        Extension("loop",
                  language="c++",
                  sources=[
                      "src/loop.pyx",
                      "src/lwan-array.c",
                      "src/lwan-coro.c"
                  ])
    ])
)
