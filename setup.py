from distutils.core import setup, Extension

module = Extension("chunk_utils", sources = ["chunk_utils.c"])

setup(
    name = "chunk_utils",
    version = "0.1",
    description = "Podrum's chunk utils",
    ext_modules = [module]
)
