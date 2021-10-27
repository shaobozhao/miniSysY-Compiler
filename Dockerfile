FROM gcc:10.2
WORKDIR /app/
COPY compiler.cpp ./
RUN g++ compiler.cpp -o compiler
RUN chmod +x compiler
