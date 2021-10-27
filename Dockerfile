FROM frolvlad/alpine-gxx
WORKDIR /app/
COPY compiler.cpp ./
RUN g++ compiler.cpp -o compiler
RUN chmod +x compiler
