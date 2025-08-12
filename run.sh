#!/bin/sh
set -e

./kl $1
mv out.asm ./run-env
cd run-env
docker compose up