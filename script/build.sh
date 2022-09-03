#!/bin/bash
cd ..
cmake -H./cmake/ -BRelease
cd ./Release && make
