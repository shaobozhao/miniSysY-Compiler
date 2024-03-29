FROM gcc:10.2
WORKDIR /app/
COPY compiler.cpp lexer.cpp parser.cpp public.h ./
RUN g++ compiler.cpp -o compiler
RUN chmod +x compiler
