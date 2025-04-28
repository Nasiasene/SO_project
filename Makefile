# Compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++11 -pthread

# Nome do executável
TARGET = sudoku_game

# Arquivos fonte
SRCS = main.cpp Sudoku.cpp

# Regra padrão
all: $(TARGET)

# Regra para compilar o programa
$(TARGET): $(SRCS)
	@$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Regra para limpar arquivos gerados
clean:
	@rm -f $(TARGET)

# Marca os alvos que não geram arquivos
.PHONY: all clean