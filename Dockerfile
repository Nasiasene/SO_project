FROM ubuntu:22.04

# Instalar dependências necessárias
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake \
    libncurses5-dev \
    libncursesw5-dev \
    && rm -rf /var/lib/apt/lists/*

# Definir diretório de trabalho
WORKDIR /app

# Copiar os arquivos do projeto
COPY src/Sudoku/Sudoku.h src/Sudoku/Sudoku.cpp /app/src/Sudoku/
COPY src/main.cpp /app/src/
COPY Makefile /app/

# Compilar o programa
RUN make

# Executar o programa quando o container for iniciado
CMD ["./sudoku_game"]
