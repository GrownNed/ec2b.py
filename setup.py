from distutils.core import Extension, setup

setup(
    name="ec2b",
    version="1.0.0",
    description="Python bindings for Ec2b",
    author="Mero",
    ext_modules=[
        Extension(
            "ec2b",
            sources=["ec2b.cpp", "aes.c"],
            extra_compile_args=["-std=c++11"],
        )
    ],
)
