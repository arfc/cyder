
FROM cyclus/cyclus:stable

COPY . /recyle
WORKDIR /recyle
RUN mkdir Release
WORKDIR /recyle/Release
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j2 && make install

