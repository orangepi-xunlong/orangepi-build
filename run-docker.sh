#!/bin/bash

IMAGE_NAME="orangepi6plus-builder"

echo "Building Docker image: $IMAGE_NAME ..."
docker build -t "$IMAGE_NAME" .
echo "Starting build container..."
echo "Running: ./build.sh GITEE_SERVER=yes $@"

docker run --privileged --rm -it \
    -v "$(pwd)":/workdir \
    "$IMAGE_NAME" \
    ./build.sh GITEE_SERVER=yes "$@"