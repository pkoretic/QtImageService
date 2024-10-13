FROM alpine:latest AS build

RUN apk add --no-cache qt6-qtbase-dev qt6-qthttpserver-dev make cmake g++

RUN mkdir -p /app
COPY CMakeLists.txt /app
COPY main.cpp /app

WORKDIR /app/
RUN cmake . && make -j$(nproc)

FROM alpine:latest
COPY --from=build /app/QtImageService /app/QtImageService
RUN apk add --no-cache qt6-qthttpserver qt6-qtimageformats qt6-qtsvg
WORKDIR /app/

EXPOSE 8080
ENTRYPOINT ["/app/QtImageService"]
