#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <future>
#include <string>

using std::vector;
using std::string;

// Enum para os níveis de dificuldade
enum class Dificuldade {
    FACIL,   // ~50% preenchido
    MEDIO,   // ~20% preenchido
    DIFICIL  // ~10% preenchido
};

// Struct para armazenar estatísticas de validação
struct ValidacaoStats {
    std::chrono::milliseconds tempoExecucao;
    int numCelulasVerificadas;
    int numConflitosEncontrados;
    int numThreadsUsadas;
};

class Sudoku {
private:
    vector<vector<int>> matriz;
    std::thread validationThread;
    bool isThreadValid;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> validacaoConcluida;
    ValidacaoStats ultimasStats;
    
    // Vetor de threads para validação paralela
    vector<std::thread> threadsValidacao;
    vector<std::future<bool>> futurosValidacao;

    // Método para verificar se um valor é válido em uma posição específica
    bool isValidValue(int row, int col, int value);

    // Função que será executada pela thread de validação
    void validationFunction();
    
    // Função para validar uma parte da matriz (para paralelismo)
    bool validarParteMatriz(int startRow, int endRow);
    
    // Função para validar linhas
    bool validarLinhas(int startRow, int endRow);
    
    // Função para validar colunas
    bool validarColunas(int startCol, int endCol);
    
    // Função para validar blocos 3x3
    bool validarBlocos(int startBlock, int endBlock);

    // Método auxiliar para preencher a matriz válida usando backtracking
    bool preencherMatrizValida(int row, int col);
    
    // Método para remover números da matriz de acordo com a dificuldade
    void ajustarDificuldade(Dificuldade nivel);

public:
    // Construtor
    Sudoku();
    
    // Construtor com nível de dificuldade
    Sudoku(Dificuldade nivel);
    
    // Construtor de movimento
    Sudoku(Sudoku&& other) noexcept;
    
    // Operador de atribuição de movimento
    Sudoku& operator=(Sudoku&& other) noexcept;
    
    // Desabilitar cópia
    Sudoku(const Sudoku&) = delete;
    Sudoku& operator=(const Sudoku&) = delete;

    // Destrutor - verifica se a thread é válida
    ~Sudoku();

    // Método para colocar um valor na matriz
    bool setValor(int row, int col, int value);

    // Método para obter um valor da matriz
    int getValor(int row, int col);

    // Método para retornar true se não houver nenhuma célula vazia (valor 0)
    bool isComplete() const;

    // Método para iniciar a thread de validação
    void iniciarValidacao();
    
    // Método para iniciar validação paralela com múltiplas threads
    void iniciarValidacaoParalela();

    // Método para verificar se a thread é válida
    bool isValidThread();
    
    // Método para obter estatísticas da última validação
    ValidacaoStats getUltimasStats() const;

    // Método para completar a matriz (válida ou inválida)
    void completarMatriz(bool gerarMatrizValida);
    
    // Método para completar a matriz atual mantendo os valores existentes
    void completarMatrizAtual(bool gerarMatrizValida);
    
    // Método para inicializar o jogo com um nível de dificuldade
    void inicializarJogo(Dificuldade nivel);

    // Método para imprimir a matriz
    void imprimirMatriz();
    
    // Método para imprimir log detalhado da validação
    void imprimirLogValidacao();
};

#endif // SUDOKU_H
