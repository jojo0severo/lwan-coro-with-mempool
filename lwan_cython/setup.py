from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
    ext_modules=cythonize([
        Extension("lwan_coro",
                  language="c++",
                  sources=["event_loop.pyx", "lwan/lwan-array.c", "lwan/lwan-coro.c"],
                  extra_compile_args=["-pthread"],
                  extra_link_args=["-pthread"]
                  )
    ])
)
