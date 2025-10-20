# syntax=docker/dockerfile:1
FROM ubuntu:24.04

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    libclang-20-dev \
    && rm -rf /var/lib/apt/lists/*

COPY ./build/linux-clang/bin/unfuckify /usr/bin/unfuckify
