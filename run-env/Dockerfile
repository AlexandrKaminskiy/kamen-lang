FROM --platform=linux/amd64 debian:bullseye

RUN apt update && apt install -y \
    build-essential \
    nasm \
    libcairo2-dev \
    pkg-config \
    && apt clean

WORKDIR /build
COPY .. .
RUN rm -rf *.o
RUN chmod +x build.sh