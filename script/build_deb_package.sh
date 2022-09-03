#!/bin/bash
DEBCONF_DEBUG=developer
export DEBCONF_DEBUG

cd ..
cmake -H./cmake/ -BRelease && cd ./Release && make deb_package
