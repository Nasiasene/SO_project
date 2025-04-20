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
COPY Sudoku.h Sudoku.cpp main.cpp /app/

# Compilar o programa
RUN g++ -std=c++11 -pthread main.cpp Sudoku.cpp -o sudoku_game

# Executar o programa quando o container for iniciado
CMD ["./sudoku_game"]
