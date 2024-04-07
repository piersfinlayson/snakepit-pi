#!/usr/bin/bash
set -e

# Build snakepit-pi
docker build . -f Dockerfile.build -t snakepit-pi-build


