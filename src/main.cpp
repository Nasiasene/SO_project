#include <iostream>
#include <limits>
#include "Sudoku/Sudoku.h"

using namespace std;

void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

Dificuldade escolherDificuldade() {
    int escolha = 0;
    
    while (escolha < 1 || escolha > 3) {
        cout << "Escolha o nível de dificuldade:" << endl;
        cout << "1. Fácil (50% preenchido)" << endl;
        cout << "2. Médio (20% preenchido)" << endl;
        cout << "3. Difícil (10% preenchido)" << endl;
        cout << "Sua escolha (1-3): ";
        cin >> escolha;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            escolha = 0;
        }
    }
    
    switch (escolha) {
        case 1: return Dificuldade::FACIL;
        case 2: return Dificuldade::MEDIO;
        case 3: return Dificuldade::DIFICIL;
        default: return Dificuldade::MEDIO; // Nunca deve chegar aqui
    }
}

void jogarSudoku() {
    limparTela();
    cout << "===== JOGO DE SUDOKU =====" << endl;
    
    // Escolher dificuldade
    Dificuldade nivel = escolherDificuldade();
    
    // Criar jogo
    Sudoku jogo(nivel);
    
    bool jogando = true;
    
    while (jogando) {
        limparTela();
        
        // Mostrar o tabuleiro
        jogo.imprimirMatriz();
        
        // Menu de opções
        cout << "\nOpções:" << endl;
        cout << "1. Inserir valor" << endl;
        cout << "2. Verificar tabuleiro" << endl;
        cout << "3. Completar tabuleiro" << endl;
        cout << "4. Novo jogo" << endl;
        cout << "5. Sair" << endl;
        
        int opcao = 0;
        cout << "\nEscolha uma opção (1-5): ";
        cin >> opcao;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        switch (opcao) {
            case 1: {
                int linha, coluna, valor;
                
                cout << "Digite a linha (1-9): ";
                cin >> linha;
                
                cout << "Digite a coluna (1-9): ";
                cin >> coluna;
                
                cout << "Digite o valor (1-9): ";
                cin >> valor;
                
                if (cin.fail() || linha < 1 || linha > 9 || coluna < 1 || coluna > 9 || valor < 1 || valor > 9) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Valores inválidos! Pressione Enter para continuar...";
                    cin.get();
                    cin.get();
                } else {
                    // Converter de 1-9 para 0-8 para uso interno
                    jogo.setValor(linha - 1, coluna - 1, valor);
                }
                break;
            }
            case 2: {
                // Verificar tabuleiro
                int escolhaVerificacao = 0;
                cout << "Como deseja verificar o tabuleiro?" << endl;
                cout << "1. Verificação simples (1 thread)" << endl;
                cout << "2. Verificação paralela (11 threads)" << endl;
                cout << "3. Verificação paralela detalhada (27 threads)" << endl;
                cout << "Sua escolha (1-3): ";
                cin >> escolhaVerificacao;

                if (cin.fail() || escolhaVerificacao < 1 || escolhaVerificacao > 3) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Opção inválida! Usando verificação simples." << endl;
                    escolhaVerificacao = 1;
                }

                if (escolhaVerificacao == 1) {
                    // Verificação simples
                    jogo.iniciarValidacao();
                    cout << "\nVerificação: O tabuleiro é "
                        << (jogo.isValidThread() ? "válido" : "inválido") << endl;
                }
                else if (escolhaVerificacao == 2) {
                    // Verificação paralela estática (11 threads)
                    jogo.iniciarValidacaoParalela();
                }
                else {
                    // Verificação paralela detalhada (27 threads)
                    jogo.iniciarValidacaoParalelaDetalhada();
                }

                // Mostrar log detalhado após a verificação
                jogo.imprimirLogValidacao();
                cout << "\nPressione Enter para continuar...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
            }
            case 3: {
                // Completar tabuleiro
                int escolhaCompletar = 0;
                cout << "Como deseja completar o tabuleiro?" << endl;
                cout << "1. Com valores válidos (solução correta)" << endl;
                cout << "2. Com valores aleatórios (pode ser inválido)" << endl;
                cout << "Sua escolha (1-2): ";
                cin >> escolhaCompletar;
                
                if (cin.fail() || (escolhaCompletar != 1 && escolhaCompletar != 2)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Opção inválida! Pressione Enter para continuar...";
                    cin.get();
                    cin.get();
                } else {
                    // Completar com valores válidos (true) ou aleatórios (false)
                    jogo.completarMatrizAtual(escolhaCompletar == 1);
                }
                break;
            }
            case 4: {
                nivel = escolherDificuldade();
                // Criar um novo jogo com o nível selecionado
                jogo = Sudoku(nivel);
                break;
            }
            case 5:
                jogando = false;
                break;
            default:
                cout << "Opção inválida! Pressione Enter para continuar...";
                cin.get();
                cin.get();
                break;
        }
    }
    
    cout << "Obrigado por jogar Sudoku!" << endl;
}

int main() {
    jogarSudoku();
    return 0;
}
