#include "Sudoku.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>

using namespace std;

// Método para verificar se um valor é válido em uma posição específica
bool Sudoku::isValidValue(int row, int col, int value) {
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
void Sudoku::validationFunction() {
    auto inicioMs  = std::chrono::high_resolution_clock::now();
    auto inicioNs  = inicioMs;
    std::clock_t inicioCpu = std::clock();
    
    lock_guard<mutex> lock(mtx);
    isThreadValid = true;
    
    int celulasVerificadas = 0;
    int conflitosEncontrados = 0;
    
    // Verificar se a matriz é válida
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            int value = matriz[row][col];
            if (value != 0) {
                celulasVerificadas++;
                // Temporariamente remover o valor para verificar
                matriz[row][col] = 0;
                if (!isValidValue(row, col, value)) {
                    isThreadValid = false;
                    conflitosEncontrados++;
                }
                // Restaurar o valor
                matriz[row][col] = value;
            }
        }
    }
    
    auto fimMs = std::chrono::high_resolution_clock::now();
    auto fimNs = fimMs;
    std::clock_t fimCpu = std::clock();
    
    auto duracaoMs = chrono::duration_cast<chrono::milliseconds>(fimMs - inicioMs);
    auto duracaoNs = chrono::duration_cast<chrono::nanoseconds>(fimNs - inicioNs);
    
    // Armazenar estatísticas
    ultimasStats.tempoExecucao = duracaoMs;
    ultimasStats.tempoEmNs = duracaoNs;
    ultimasStats.cpuTicks = fimCpu - inicioCpu;
    ultimasStats.numCelulasVerificadas = celulasVerificadas;
    ultimasStats.numConflitosEncontrados = conflitosEncontrados;
    ultimasStats.numThreadsUsadas = 1;
    
    validacaoConcluida.store(true);
    cv.notify_all();
    
    cout << "\n[Thread de Validação] Concluída em " << duracaoMs.count() 
         << "ms, " << duracaoNs.count() << "ns, verificadas " << celulasVerificadas << " células, encontrados " 
         << conflitosEncontrados << " conflitos." << endl;
}

// Função para validar linhas
bool Sudoku::validarLinhas(int startRow, int endRow) {
    // Detecta incompletude nesta linha
    for (int row = startRow; row < endRow; row++) {
        for (int col = 0; col < 9; col++) {
            if (matriz[row][col] == 0) {
                lock_guard<mutex> lock(mtx);
                cout << "[Thread " << this_thread::get_id()
                     << "] Linhas " << startRow+1 << "-" << endRow
                     << ": INCOMPLETO (célula vazia em " 
                     << row+1 << "," << col+1 << ")" << endl;
                return false;  // sinaliza “incompleto” como falso
            }
        }
    }
    auto inicio = chrono::high_resolution_clock::now();
    
    bool valido = true;
    int celulasVerificadas = 0;
    int conflitosEncontrados = 0;
    
    for (int row = startRow; row < endRow; row++) {
        vector<bool> numeros(10, false); // Índices 1-9 para valores
        for (int col = 0; col < 9; col++) {
            int valor = matriz[row][col];
            celulasVerificadas++;
            if (valor != 0) {
                if (numeros[valor]) {
                    valido = false;
                    conflitosEncontrados++;
                }
                numeros[valor] = true;
            }
        }
    }
    
    auto fim = chrono::high_resolution_clock::now();
    auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
    
    lock_guard<mutex> lock(mtx);
    cout << "[Thread " << this_thread::get_id() << "] Validação de linhas " 
         << startRow+1 << "-" << endRow << " concluída em " << duracao.count() 
         << "ms, " << (valido ? "válido" : "inválido") << ", conflitos: " 
         << conflitosEncontrados << endl;
    
    return valido;
}

// Função para validar colunas
bool Sudoku::validarColunas(int startCol, int endCol) {
    // Detecta incompletude neste bloco de colunas
    for (int col = startCol; col < endCol; col++) {
        for (int row = 0; row < 9; row++) {
            if (matriz[row][col] == 0) {
                lock_guard<mutex> lock(mtx);
                cout << "[Thread " << this_thread::get_id()
                     << "] Colunas " << startCol+1 << "-" << endCol
                     << ": INCOMPLETO (célula vazia em "
                     << row+1 << "," << col+1 << ")" << endl;
                return false;
            }
        }
    }
    
    auto inicio = chrono::high_resolution_clock::now();
    
    bool valido = true;
    int celulasVerificadas = 0;
    int conflitosEncontrados = 0;
    
    for (int col = startCol; col < endCol; col++) {
        vector<bool> numeros(10, false); // Índices 1-9 para valores
        for (int row = 0; row < 9; row++) {
            int valor = matriz[row][col];
            celulasVerificadas++;
            if (valor != 0) {
                if (numeros[valor]) {
                    valido = false;
                    conflitosEncontrados++;
                }
                numeros[valor] = true;
            }
        }
    }
    
    auto fim = chrono::high_resolution_clock::now();
    auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
    
    lock_guard<mutex> lock(mtx);
    cout << "[Thread " << this_thread::get_id() << "] Validação de colunas " 
         << startCol+1 << "-" << endCol << " concluída em " << duracao.count() 
         << "ms, " << (valido ? "válido" : "inválido") << ", conflitos: " 
         << conflitosEncontrados << endl;
    
    return valido;
}

// Função para validar blocos 3x3
bool Sudoku::validarBlocos(int startBlock, int endBlock) {
    // Detecta incompletude neste bloco de sub-quadros
    for (int block = startBlock; block < endBlock; block++) {
        int startRow = (block / 3) * 3;
        int startCol = (block % 3) * 3;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (matriz[startRow + i][startCol + j] == 0) {
                    lock_guard<mutex> lock(mtx);
                    cout << "[Thread " << this_thread::get_id()
                         << "] Bloco " << block+1
                         << ": INCOMPLETO (célula vazia em "
                         << (startRow + i +1) << ","
                         << (startCol + j +1) << ")" << endl;
                    return false;
                }
            }
        }
    }

    auto inicio = chrono::high_resolution_clock::now();
    
    bool valido = true;
    int celulasVerificadas = 0;
    int conflitosEncontrados = 0;
    
    for (int block = startBlock; block < endBlock; block++) {
        int startRow = (block / 3) * 3;
        int startCol = (block % 3) * 3;
        
        vector<bool> numeros(10, false); // Índices 1-9 para valores
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int valor = matriz[startRow + i][startCol + j];
                celulasVerificadas++;
                if (valor != 0) {
                    if (numeros[valor]) {
                        valido = false;
                        conflitosEncontrados++;
                    }
                    numeros[valor] = true;
                }
            }
        }
    }
    
    auto fim = chrono::high_resolution_clock::now();
    auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
    
    lock_guard<mutex> lock(mtx);
    cout << "[Thread " << this_thread::get_id() << "] Validação de blocos " 
         << startBlock+1 << "-" << endBlock << " concluída em " << duracao.count() 
         << "ms, " << (valido ? "válido" : "inválido") << ", conflitos: " 
         << conflitosEncontrados << endl;
    
    return valido;
}

// Método para iniciar validação paralela com 11 threads
void Sudoku::iniciarValidacaoParalela() {
    // 1) Finaliza eventuais threads antigas
    for (auto& t : threadsValidacao) {
        if (t.joinable()) t.join();
    }
    threadsValidacao.clear();
    futurosValidacao.clear();

    // 2) Marca início da medição
    auto inicioMs = chrono::high_resolution_clock::now();
    auto inicioNs = inicioMs;
    std::clock_t inicioCpu = std::clock();
    
    cout << "\n[Sistema] Iniciando validação paralela com 11 threads..." << endl;

    // 3) Flags de controle
    validacaoConcluida.store(false);
    isThreadValid = true;

    // 4) Thread única para colunas (0–9)
    {
        packaged_task<bool()> task(
            bind(&Sudoku::validarColunas, this, 0, 9)
        );
        futurosValidacao.push_back(task.get_future());
        threadsValidacao.emplace_back(std::move(task));
    }

    // 5) Thread única para linhas (0–9)
    {
        packaged_task<bool()> task(
            bind(&Sudoku::validarLinhas, this, 0, 9)
        );
        futurosValidacao.push_back(task.get_future());
        threadsValidacao.emplace_back(std::move(task));
    }

    // 6) Nove threads, uma para cada bloco 3×3
    for (int block = 0; block < 9; block++) {
        packaged_task<bool()> task(
            bind(&Sudoku::validarBlocos, this, block, block + 1)
        );
        futurosValidacao.push_back(task.get_future());
        threadsValidacao.emplace_back(std::move(task));
    }

    // 7) Coleta resultados
    for (auto& fut : futurosValidacao) {
        if (!fut.get()) {
            isThreadValid = false;
        }
    }

    // 8) Garante join em todas as threads
    for (auto& t : threadsValidacao) {
        if (t.joinable()) t.join();
    }

    // 9) Estatísticas e relatório
    auto fimMs = chrono::high_resolution_clock::now();
    auto fimNs = fimMs;
    std::clock_t fimCpu = std::clock();
    
    auto duracaoMs = chrono::duration_cast<chrono::milliseconds>(fimMs - inicioMs);
    auto duracaoNs = chrono::duration_cast<chrono::nanoseconds>(fimNs - inicioNs);
    
    ultimasStats.tempoExecucao         = duracaoMs;
    ultimasStats.tempoEmNs             = duracaoNs;
    ultimasStats.cpuTicks              = fimCpu - inicioCpu;
    // 1 thread columnas + 1 thread linhas + 9 threads blocos
    ultimasStats.numThreadsUsadas     = 11;
    // 81 células validadas 3×, mas você pode ajustar conforme queira
    ultimasStats.numCelulasVerificadas = 9 * 9 * 3;

    validacaoConcluida.store(true);

    cout << "[Sistema] Validação paralela concluída em "
         << duracaoMs.count() << "ms, " << duracaoNs.count() << "ns, verificadas "
        << ultimasStats.numCelulasVerificadas << " células, encontrados " 
         << ultimasStats.numConflitosEncontrados << " conflitos." << endl;
    cout << "[Sistema] Resultado: O tabuleiro é "
         << (isThreadValid ? "válido" : "inválido") << endl;
}

// Método para iniciar validação paralela detalhada com 27 threads
void Sudoku::iniciarValidacaoParalelaDetalhada() {
    // 1) Finaliza eventuais threads antigas
    for (auto& t : threadsValidacao) {
        if (t.joinable()) t.join();
    }
    threadsValidacao.clear();
    futurosValidacao.clear();

    // 2) Marca início da medição
    auto inicioMs = chrono::high_resolution_clock::now();
    auto inicioNs = inicioMs;
    std::clock_t inicioCpu = std::clock();
    
    cout << "\n[Sistema] Iniciando validação paralela detalhada com 27 threads..." << endl;

    // 3) Flags de controle
    validacaoConcluida.store(false);
    isThreadValid = true;

    // 4) Nove threads, uma para cada LINHA
    for (int row = 0; row < 9; row++) {
        packaged_task<bool()> task(
            bind(&Sudoku::validarLinhas, this, row, row + 1)
        );
        futurosValidacao.push_back(task.get_future());
        threadsValidacao.emplace_back(std::move(task));
    }

    // 5) Nove threads, uma para cada COLUNA
    for (int col = 0; col < 9; col++) {
        packaged_task<bool()> task(
            bind(&Sudoku::validarColunas, this, col, col + 1)
        );
        futurosValidacao.push_back(task.get_future());
        threadsValidacao.emplace_back(std::move(task));
    }

    // 6) Nove threads, uma para cada BLOCO 3×3
    for (int block = 0; block < 9; block++) {
        packaged_task<bool()> task(
            bind(&Sudoku::validarBlocos, this, block, block + 1)
        );
        futurosValidacao.push_back(task.get_future());
        threadsValidacao.emplace_back(std::move(task));
    }

    // 7) Coleta resultados de todas as 27 threads
    for (auto& fut : futurosValidacao) {
        if (!fut.get()) {
            isThreadValid = false;
        }
    }

    // 8) Garante join em todas as threads
    for (auto& t : threadsValidacao) {
        if (t.joinable()) t.join();
    }

    // 9) Estatísticas e relatório
    auto fimMs = chrono::high_resolution_clock::now();
    auto fimNs = fimMs;
    std::clock_t fimCpu = std::clock();
    
    auto duracaoMs = chrono::duration_cast<chrono::milliseconds>(fimMs - inicioMs);
    auto duracaoNs = chrono::duration_cast<chrono::nanoseconds>(fimNs - inicioNs);
    
    ultimasStats.tempoExecucao         = duracaoMs;
    ultimasStats.tempoEmNs             = duracaoNs;
    ultimasStats.cpuTicks              = fimCpu - inicioCpu;
    ultimasStats.numThreadsUsadas      = 27;
    ultimasStats.numCelulasVerificadas = 9 * 9 * 3;  // 243 verificações no total

    validacaoConcluida.store(true);

    cout << "[Sistema] Validação paralela detalhada concluída em "
         << duracaoMs.count() << "ms, " << duracaoNs.count() << "ns, verificadas "
        << ultimasStats.numCelulasVerificadas << " células, encontrados " 
         << ultimasStats.numConflitosEncontrados << " conflitos." << endl;
    cout << "[Sistema] Resultado: O tabuleiro é "
         << (isThreadValid ? "válido" : "inválido") << endl;
}

// Método para obter estatísticas da última validação
ValidacaoStats Sudoku::getUltimasStats() const {
    return ultimasStats;
}

// Método para imprimir log detalhado da validação
void Sudoku::imprimirLogValidacao() {
    cout << "\n===== LOG DETALHADO DE VALIDAÇÃO =====" << endl;
    cout << "Status: " << (isThreadValid ? "VÁLIDO" : "INVÁLIDO/INCOMPLETO") << endl;
    cout << "Tempo de execução em ms: " << ultimasStats.tempoExecucao.count() << " ms" << endl;
    cout << "Tempo de execução em ns: " << ultimasStats.tempoEmNs.count() << " ns" << endl;
    cout << "CPU ticks: " << ultimasStats.cpuTicks << endl;
    cout << "Células verificadas: "      << ultimasStats.numCelulasVerificadas << endl;
    cout << "Conflitos encontrados: "    << ultimasStats.numConflitosEncontrados << endl;
    cout << "Threads utilizadas: "      << ultimasStats.numThreadsUsadas << endl;
    cout << "=======================================" << endl;
}

// Construtor
Sudoku::Sudoku() : isThreadValid(false), validacaoConcluida(false) {
    // Inicializar a matriz 9x9 com zeros
    matriz.resize(9, vector<int>(9, 0));
    
    // Inicializar estatísticas
    ultimasStats.tempoExecucao = chrono::milliseconds(0);
    ultimasStats.tempoEmNs = chrono::nanoseconds(0);
    ultimasStats.cpuTicks = 0;
    ultimasStats.numCelulasVerificadas = 0;
    ultimasStats.numConflitosEncontrados = 0;
    ultimasStats.numThreadsUsadas = 0;
}

// Construtor com nível de dificuldade
Sudoku::Sudoku(Dificuldade nivel) : isThreadValid(false), validacaoConcluida(false) {
    // Inicializar a matriz 9x9 com zeros
    matriz.resize(9, vector<int>(9, 0));
    
    // Inicializar estatísticas
    ultimasStats.tempoExecucao = chrono::milliseconds(0);
    ultimasStats.numCelulasVerificadas = 0;
    ultimasStats.numConflitosEncontrados = 0;
    ultimasStats.numThreadsUsadas = 0;
    
    inicializarJogo(nivel);
}   

// Construtor de movimento
Sudoku::Sudoku(Sudoku&& other) noexcept 
    : matriz(std::move(other.matriz)),
      validationThread(std::move(other.validationThread)),
      isThreadValid(other.isThreadValid),
      validacaoConcluida(false),
      ultimasStats(other.ultimasStats) {
    // Não podemos mover std::atomic, então inicializamos com o valor da outra
    validacaoConcluida.store(other.validacaoConcluida.load());
}

// Operador de atribuição de movimento
Sudoku& Sudoku::operator=(Sudoku&& other) noexcept {
    if (this != &other) {
        // Finalizar a thread atual se estiver em execução
        if (validationThread.joinable()) {
            validationThread.join();
        }
        
        // Mover os dados
        matriz = std::move(other.matriz);
        validationThread = std::move(other.validationThread);
        isThreadValid = other.isThreadValid;
        // Não podemos atribuir std::atomic, então usamos store/load
        validacaoConcluida.store(other.validacaoConcluida.load());
        ultimasStats = other.ultimasStats;
    }
    return *this;
}

// Destrutor - verifica se a thread é válida
Sudoku::~Sudoku() {
    if (validationThread.joinable()) {
        validationThread.join();
        cout << "Thread de validação finalizada. Resultado: " 
             << (isThreadValid ? "Matriz válida" : "Matriz inválida") << endl;
    }
}

// Método para colocar um valor na matriz
bool Sudoku::setValor(int row, int col, int value) {
    if (row < 0 || row >= 9 || col < 0 || col >= 9 || value < 0 || value > 9) {
        cout << "Posição ou valor inválido!" << endl;
        return false;
    }

    matriz[row][col] = value;
    return true;
}

// Método para obter um valor da matriz
int Sudoku::getValor(int row, int col) {
    if (row < 0 || row >= 9 || col < 0 || col >= 9) {
        cout << "Posição inválida!" << endl;
        return -1;
    }
    return matriz[row][col];
}

bool Sudoku::isComplete() const {
    for (const auto& row : matriz) {
        for (int v : row) {
            if (v == 0) {
                return false;
            }
        }
    }
    return true;
}

// Método para iniciar a thread de validação
void Sudoku::iniciarValidacao() {
    if (validationThread.joinable()) {
        validationThread.join();
    }
    validationThread = thread(&Sudoku::validationFunction, this);
}

// Método para verificar se a thread é válida
bool Sudoku::isValidThread() {
    if (validationThread.joinable()) {
        validationThread.join();
    }
    return isThreadValid;
}

// Método para completar a matriz (válida ou inválida)
void Sudoku::completarMatriz(bool gerarMatrizValida) {
    if (gerarMatrizValida) {
        // Algoritmo para gerar uma matriz válida de Sudoku
        random_device rd;
        mt19937 g(rd());
        
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
        random_device rd;
        mt19937 g(rd());
        uniform_int_distribution<int> dist(1, 9);
        
        // Preencher com valores aleatórios
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                matriz[i][j] = dist(g);
            }
        }
    }
}

// Método para completar a matriz atual mantendo os valores existentes
void Sudoku::completarMatrizAtual(bool gerarMatrizValida) {
    if (gerarMatrizValida) {
        // Fazer uma cópia da matriz atual para preservar os valores existentes
        vector<vector<int>> matrizOriginal = matriz;
        
        // Tentar completar a matriz de forma válida usando backtracking
        bool sucesso = preencherMatrizValida(0, 0);
        
        if (!sucesso) {
            cout << "Não foi possível completar a matriz de forma válida com os valores atuais." << endl;
            // Restaurar a matriz original
            matriz = matrizOriginal;
        }
    } else {
        // Gerar valores aleatórios para as células vazias
        random_device rd;
        mt19937 g(rd());
        uniform_int_distribution<int> dist(1, 9);
        
        // Preencher apenas as células vazias com valores aleatórios
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (matriz[i][j] == 0) {
                    matriz[i][j] = dist(g);
                }
            }
        }
    }
}

// Método para inicializar o jogo com um nível de dificuldade
void Sudoku::inicializarJogo(Dificuldade nivel) {
    // Primeiro, gerar uma matriz válida completa
    completarMatriz(true);
    
    // Depois, remover números de acordo com a dificuldade
    ajustarDificuldade(nivel);
}

// Método para remover números da matriz de acordo com a dificuldade
void Sudoku::ajustarDificuldade(Dificuldade nivel) {
    int celulasParaRemover = 0;
    int totalCelulas = 9 * 9; // 81 células no total
    
    // Definir quantas células remover baseado na dificuldade
    switch (nivel) {
        case Dificuldade::FACIL:
            // Manter ~50% preenchido, remover ~50%
            celulasParaRemover = static_cast<int>(totalCelulas * 0.5);
            break;
        case Dificuldade::MEDIO:
            // Manter ~20% preenchido, remover ~80%
            celulasParaRemover = static_cast<int>(totalCelulas * 0.8);
            break;
        case Dificuldade::DIFICIL:
            // Manter ~10% preenchido, remover ~90%
            celulasParaRemover = static_cast<int>(totalCelulas * 0.9);
            break;
    }
    
    // Criar uma lista de todas as posições
    vector<pair<int, int>> posicoes;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            posicoes.push_back(make_pair(i, j));
        }
    }
    
    // Embaralhar as posições
    random_device rd;
    mt19937 g(rd());
    shuffle(posicoes.begin(), posicoes.end(), g);
    
    // Remover os números das primeiras 'celulasParaRemover' posições
    for (int i = 0; i < celulasParaRemover && i < posicoes.size(); i++) {
        int row = posicoes[i].first;
        int col = posicoes[i].second;
        matriz[row][col] = 0; // 0 representa célula vazia
    }
}

// Método auxiliar para preencher a matriz válida usando backtracking
bool Sudoku::preencherMatrizValida(int row, int col) {
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
    vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);
    
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
void Sudoku::imprimirMatriz() {
    cout << "Estado atual do Sudoku:" << endl;
    cout << "    1 2 3   4 5 6   7 8 9" << endl;  // Índices das colunas
    cout << "  -------------------------" << endl;
    for (int i = 0; i < 9; i++) {
        cout << i + 1 << " | "; // Índice da linha (1-9)
        for (int j = 0; j < 9; j++) {
            if (matriz[i][j] == 0) {
                cout << ". ";
            } else {
                cout << matriz[i][j] << " ";
            }
            if (j % 3 == 2 && j < 8) {
                cout << "| ";
            }
        }
        cout << "|" << endl;
        if (i % 3 == 2 && i < 8) {
            cout << "  |-------+-------+-------|" << endl;
        }
    }
    cout << "  -------------------------" << endl;
}
