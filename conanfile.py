from conans import ConanFile, CMake

class ConanPackage(ConanFile):
    name = 'network-monitor'
    version = "0.1.0"

    generators = 'cmake_find_package'

    requires = [
        ('boost/1.74.0'),
        ('openssl/1.1.1t'),
        ('cpr/1.9.3'),
        ('jsoncpp/1.9.5')

    ]

    default_options = (
        'boost:shared=False',
    )