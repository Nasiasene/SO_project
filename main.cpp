#include <iostream>
#include "Sudoku.h"

void demonstrarDificuldade(Dificuldade nivel) {
    std::string nivelStr;
    switch (nivel) {
        case Dificuldade::FACIL:
            nivelStr = "FÁCIL";
            break;
        case Dificuldade::MEDIO:
            nivelStr = "MÉDIO";
            break;
        case Dificuldade::DIFICIL:
            nivelStr = "DIFÍCIL";
            break;
    }
    
    std::cout << "\n\n===== DEMONSTRAÇÃO DO NÍVEL " << nivelStr << " =====" << std::endl;
    
    // Criar Sudoku com o nível especificado
    Sudoku sudoku(nivel);
    
    // Mostrar o tabuleiro inicial
    std::cout << "Tabuleiro inicial (nível " << nivelStr << "):" << std::endl;
    sudoku.imprimirMatriz();
    
    // Validar o tabuleiro
    sudoku.iniciarValidacao();
    std::cout << "O tabuleiro é " << (sudoku.isValidThread() ? "válido" : "inválido") << std::endl;
}

int main() {
    std::cout << "===== JOGO DE SUDOKU COM NÍVEIS DE DIFICULDADE =====" << std::endl;
    
    // Demonstrar os três níveis de dificuldade
    demonstrarDificuldade(Dificuldade::FACIL);
    demonstrarDificuldade(Dificuldade::MEDIO);
    demonstrarDificuldade(Dificuldade::DIFICIL);
    
    // Exemplo de uso interativo (opcional)
    std::cout << "\n\n===== EXEMPLO DE JOGO INTERATIVO =====" << std::endl;
    Sudoku jogoInterativo(Dificuldade::MEDIO);
    std::cout << "Jogo no nível MÉDIO criado:" << std::endl;
    jogoInterativo.imprimirMatriz();
    
    // Exemplo de inserção de um valor
    std::cout << "\nInserindo valor 5 na posição (0,0):" << std::endl;
    jogoInterativo.setValor(0, 0, 5);
    jogoInterativo.imprimirMatriz();
    
    // Validar o tabuleiro após modificação
    jogoInterativo.iniciarValidacao();
    std::cout << "Após modificação, o tabuleiro é " 
              << (jogoInterativo.isValidThread() ? "válido" : "inválido") << std::endl;
    
    return 0;
}
