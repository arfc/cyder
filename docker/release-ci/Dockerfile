
FROM cyclus/cyclus:stable

COPY . /cyder
WORKDIR /cyder
RUN mkdir Release
WORKDIR /cyder/Release
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j2 && make install

