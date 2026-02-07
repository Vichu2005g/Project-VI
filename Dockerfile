FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY CMakeLists.txt ./
COPY src/ ./src/
COPY Models/ ./Models/

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/build/Project-VI /usr/local/bin/Project-VI

EXPOSE 8080

CMD ["/usr/local/bin/Project-VI"]

