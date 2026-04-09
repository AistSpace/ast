from setuptools import setup, find_packages

setup(
    name="astpy",
    version="1.0.0",
    packages=find_packages(),
    python_requires=">=3.7",
    description="Python bindings for Ast aerospace library",
    author="AistSpace",
    author_email="aistspace@users.noreply.github.com",
    url="https://github.com/space-ast/ast",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: Astronomy",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
    ],
)
