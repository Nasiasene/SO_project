#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>
#include <thread>
#include <mutex>

// Enum para os níveis de dificuldade
enum class Dificuldade {
    FACIL,   // ~50% preenchido
    MEDIO,   // ~20% preenchido
    DIFICIL  // ~10% preenchido
};

class Sudoku {
private:
    std::vector<std::vector<int>> matriz;
    std::thread validationThread;
    bool isThreadValid;
    std::mutex mtx;

    // Método para verificar se um valor é válido em uma posição específica
    bool isValidValue(int row, int col, int value);

    // Função que será executada pela thread de validação
    void validationFunction();

    // Método auxiliar para preencher a matriz válida usando backtracking
    bool preencherMatrizValida(int row, int col);
    
    // Método para remover números da matriz de acordo com a dificuldade
    void ajustarDificuldade(Dificuldade nivel);

public:
    // Construtor
    Sudoku();
    
    // Construtor com nível de dificuldade
    Sudoku(Dificuldade nivel);

    // Destrutor - verifica se a thread é válida
    ~Sudoku();

    // Método para colocar um valor na matriz
    bool setValor(int row, int col, int value);

    // Método para obter um valor da matriz
    int getValor(int row, int col);

    // Método para iniciar a thread de validação
    void iniciarValidacao();

    // Método para verificar se a thread é válida
    bool isValidThread();

    // Método para completar a matriz (válida ou inválida)
    void completarMatriz(bool gerarMatrizValida);
    
    // Método para inicializar o jogo com um nível de dificuldade
    void inicializarJogo(Dificuldade nivel);

    // Método para imprimir a matriz
    void imprimirMatriz();
};

#endif // SUDOKU_H
