#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <thread>
#include <mutex>

class Sudoku {
private:
    std::vector<std::vector<int>> matriz;
    std::thread validationThread;
    bool isThreadValid;
    std::mutex mtx;

    // Método para verificar se um valor é válido em uma posição específica
    bool isValidValue(int row, int col, int value) {
        // Verificar linha
        for (int i = 0; i < 9; i++) {
            if (matriz[row][i] == value && i != col) {
                return false;
            }
        }

        // Verificar coluna
        for (int i = 0; i < 9; i++) {
            if (matriz[i][col] == value && i != row) {
                return false;
            }
        }

        // Verificar bloco 3x3
        int startRow = row - row % 3;
        int startCol = col - col % 3;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (matriz[i + startRow][j + startCol] == value && 
                    (i + startRow != row || j + startCol != col)) {
                    return false;
                }
            }
        }

        return true;
    }

    // Função que será executada pela thread de validação
    void validationFunction() {
        std::lock_guard<std::mutex> lock(mtx);
        isThreadValid = true;
        
        // Verificar se a matriz é válida
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                int value = matriz[row][col];
                if (value != 0) {
                    // Temporariamente remover o valor para verificar
                    matriz[row][col] = 0;
                    if (!isValidValue(row, col, value)) {
                        isThreadValid = false;
                    }
                    // Restaurar o valor
                    matriz[row][col] = value;
                }
            }
        }
    }

public:
    // Construtor
    Sudoku() : isThreadValid(false) {
        // Inicializar a matriz 9x9 com zeros
        matriz.resize(9, std::vector<int>(9, 0));
    }

    // Destrutor - verifica se a thread é válida
    ~Sudoku() {
        if (validationThread.joinable()) {
            validationThread.join();
            std::cout << "Thread de validação finalizada. Resultado: " 
                      << (isThreadValid ? "Matriz válida" : "Matriz inválida") << std::endl;
        }
    }

    // Método para colocar um valor na matriz
    bool setValor(int row, int col, int value) {
        if (row < 0 || row >= 9 || col < 0 || col >= 9 || value < 0 || value > 9) {
            std::cout << "Posição ou valor inválido!" << std::endl;
            return false;
        }

        matriz[row][col] = value;
        return true;
    }

    // Método para obter um valor da matriz
    int getValor(int row, int col) {
        if (row < 0 || row >= 9 || col < 0 || col >= 9) {
            std::cout << "Posição inválida!" << std::endl;
            return -1;
        }
        return matriz[row][col];
    }

    // Método para iniciar a thread de validação
    void iniciarValidacao() {
        if (validationThread.joinable()) {
            validationThread.join();
        }
        validationThread = std::thread(&Sudoku::validationFunction, this);
    }

    // Método para verificar se a thread é válida
    bool isValidThread() {
        if (validationThread.joinable()) {
            validationThread.join();
        }
        return isThreadValid;
    }

    // Método para completar a matriz (válida ou inválida)
    void completarMatriz(bool gerarMatrizValida) {
        if (gerarMatrizValida) {
            // Algoritmo para gerar uma matriz válida de Sudoku
            std::random_device rd;
            std::mt19937 g(rd());
            
            // Limpar a matriz
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    matriz[i][j] = 0;
                }
            }
            
            // Preencher a matriz usando backtracking
            preencherMatrizValida(0, 0);
        } else {
            // Gerar uma matriz inválida (com conflitos)
            std::random_device rd;
            std::mt19937 g(rd());
            std::uniform_int_distribution<int> dist(1, 9);
            
            // Preencher com valores aleatórios
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    matriz[i][j] = dist(g);
                }
            }
        }
    }

    // Método auxiliar para preencher a matriz válida usando backtracking
    bool preencherMatrizValida(int row, int col) {
        // Se chegamos ao final da matriz, retornamos true
        if (row == 9) {
            return true;
        }
        
        // Se chegamos ao final da linha, vamos para a próxima
        if (col == 9) {
            return preencherMatrizValida(row + 1, 0);
        }
        
        // Se a célula já tem um valor, vamos para a próxima
        if (matriz[row][col] != 0) {
            return preencherMatrizValida(row, col + 1);
        }
        
        // Tentar valores de 1 a 9
        std::vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(values.begin(), values.end(), g);
        
        for (int val : values) {
            if (isValidValue(row, col, val)) {
                matriz[row][col] = val;
                
                if (preencherMatrizValida(row, col + 1)) {
                    return true;
                }
                
                // Se não conseguimos preencher, voltamos atrás
                matriz[row][col] = 0;
            }
        }
        
        return false;
    }

    // Método para imprimir a matriz
    void imprimirMatriz() {
        std::cout << "Estado atual do Sudoku:" << std::endl;
        for (int i = 0; i < 9; i++) {
            if (i % 3 == 0 && i != 0) {
                std::cout << "------+-------+------" << std::endl;
            }
            for (int j = 0; j < 9; j++) {
                if (j % 3 == 0 && j != 0) {
                    std::cout << "| ";
                }
                if (matriz[i][j] == 0) {
                    std::cout << ". ";
                } else {
                    std::cout << matriz[i][j] << " ";
                }
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    Sudoku sudoku;
    
    // Exemplo de uso
    std::cout << "Criando um novo jogo de Sudoku..." << std::endl;
    
    // Completar a matriz com valores válidos
    std::cout << "Gerando uma matriz válida de Sudoku..." << std::endl;
    sudoku.completarMatriz(true);
    sudoku.imprimirMatriz();
    
    // Iniciar a thread de validação
    sudoku.iniciarValidacao();
    std::cout << "A matriz é " << (sudoku.isValidThread() ? "válida" : "inválida") << std::endl;
    
    // Modificar alguns valores para testar
    std::cout << "\nModificando alguns valores..." << std::endl;
    sudoku.setValor(0, 0, 5);
    sudoku.imprimirMatriz();
    
    // Iniciar a thread de validação novamente
    sudoku.iniciarValidacao();
    std::cout << "A matriz é " << (sudoku.isValidThread() ? "válida" : "inválida") << std::endl;
    
    // Exemplo de matriz inválida
    std::cout << "\nGerando uma matriz inválida de Sudoku..." << std::endl;
    sudoku.completarMatriz(false);
    sudoku.imprimirMatriz();
    
    // Iniciar a thread de validação
    sudoku.iniciarValidacao();
    std::cout << "A matriz é " << (sudoku.isValidThread() ? "válida" : "inválida") << std::endl;
    
    return 0;
}
