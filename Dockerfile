FROM gcc:10.2
WORKDIR /app/
COPY compiler.cpp lexer.cpp grammar.cpp ./
RUN g++ compiler.cpp -o compiler
RUN chmod +x compiler
