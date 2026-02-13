# Builder
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libasio-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY CMakeLists.txt ./
COPY src/ ./src/
COPY Models/ ./Models/
COPY third_party/ ./third_party/
COPY src/frontend/ ./frontend/
COPY src/utils/ ./src/utils/
COPY third_party/ ./third_party/  

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build


#Run time
FROM ubuntu:22.04

WORKDIR /app
RUN mkdir -p /app/data

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/frontend /app/frontend
COPY --from=builder /build/build/Project-VI /usr/local/bin/Project-VI

EXPOSE 8080

CMD ["/usr/local/bin/Project-VI"]
