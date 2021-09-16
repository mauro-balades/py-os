try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

with open("README.md", "r") as readme_file:
    readme = readme_file.read()

packages = [
    "click",
    "termcolor"
]

setup(
    name='py-os',
    version='1.0.0',
    description="This a 'fake' operating system made for CUI learing purposes",
    author='Status',
    url='https://github.com/maubg-debug/py-os/',
    packages=[
        'src'
    ],
    entry_points={
        "console_scripts": ["py-os=src.main:start"],
    },
    install_requires=packages,
    license="GNU",
    zip_safe=False,
    keywords=['py-os'],
    long_description=readme,
    long_description_content_type="text/markdown",
)