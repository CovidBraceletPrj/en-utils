FROM alpine:3 as base

RUN apk add alpine-sdk
RUN apk add gcc
RUN apk add make
RUN apk add cmake

COPY . /usr/src/en-utils
WORKDIR /usr/src/en-utils


RUN rm -rf build
RUN mkdir build

WORKDIR /usr/src/en-utils/build

RUN cmake -DENABLE_TESTING=Off ..
RUN make

FROM alpine:latest

WORKDIR /en-utils

COPY --from=base /usr/src/en-utils/build/en_utils /usr/bin

ENTRYPOINT ["/usr/bin/en_utils"]
CMD ["--help"]





