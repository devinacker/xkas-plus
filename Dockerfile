FROM alpine
RUN apk update
RUN apk add g++ make
COPY / /src/
WORKDIR /src
RUN ls -l
RUN make

FROM alpine
COPY --from=0 /src/xkas /xkas
CMD ["/xkas"]
