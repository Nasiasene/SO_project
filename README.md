# Sudoku Game com Multithreading

Um jogo de Sudoku implementado em C++ com recursos avançados de multithreading para a disciplina de Sistemas Operacionais.

## Do que se trata o projeto

Este projeto implementa um jogo de Sudoku com foco em conceitos de Sistemas Operacionais, particularmente multithreading e paralelismo. O jogo permite ao usuário:

- Jogar Sudoku em diferentes níveis de dificuldade
- Verificar a validade do tabuleiro usando uma única thread ou múltiplas threads em paralelo
- Visualizar estatísticas detalhadas de desempenho das operações de validação
- Comparar o desempenho entre abordagens sequenciais e paralelas

O objetivo principal é demonstrar conceitos importantes de SO como:
- Criação e gerenciamento de threads
- Sincronização usando mutex
- Variáveis atômicas para operações thread-safe
- Estratégias de paralelização de tarefas
- Medição de desempenho

## Interface do Jogo

```
Estado atual do Sudoku:   
    1 2 3   4 5 6   7 8 9      
  -------------------------
1 | . . . | . . . | . . . |
2 | . . . | . . . | . . . |
3 | . . . | 8 . . | 4 . . |
  |-------+-------+-------|
4 | . . . | . . . | . . . |
5 | 5 . 2 | . . . | . . 9 |
6 | . . . | . . . | 5 . . |
  |-------+-------+-------|
7 | 6 . . | . . . | . . . |
8 | . . . | 5 . . | . . . |
9 | 3 . . | . . . | . . . |
  -------------------------

Opções:
1. Inserir valor
2. Verificar tabuleiro
3. Completar tabuleiro
4. Novo jogo
5. Sair
```

## Instruções de Compilação e Execução

### Dependências

- Compilador C++ com suporte a C++11
- Biblioteca pthread

### Compilação Local

```bash
g++ -std=c++11 -pthread main.cpp Sudoku.cpp -o sudoku_game
```

### Execução Local

```bash
./sudoku_game
```

### Execução com Docker

```bash
docker build -t sudoku-game .
docker run -it sudoku-game
```

### Execução na MV

```bash
ssh -i /path/to/key.pem ec2-user@18.191.154.157
```

```bash
screen -r sudoku_resolver
```

## Instruções de Uso

1. Ao iniciar o jogo, você verá um tabuleiro de Sudoku parcialmente preenchido.
2. Use o menu para interagir com o jogo:
   - **Inserir valor**: Permite adicionar um número em uma posição específica do tabuleiro.
   - **Verificar tabuleiro**: Verifica se o tabuleiro atual é válido, oferecendo opções para usar uma única thread ou múltiplas threads em paralelo. Após a verificação, um log detalhado é exibido com estatísticas de desempenho.
   - **Completar tabuleiro**: Preenche automaticamente o tabuleiro com valores válidos ou inválidos (conforme escolha do usuário).
   - **Novo jogo**: Inicia um novo jogo com diferentes níveis de dificuldade (fácil, médio, difícil).
   - **Sair**: Encerra o jogo.

## Arquivos do Projeto

- `Sudoku.h`: Definição da classe com atributos relacionados a threads
- `Sudoku.cpp`: Implementação dos métodos de validação multithread
- `main.cpp`: Interface interativa com opções para teste de threads
- `Dockerfile`: Configuração para containerização do jogo

## Contribuições

Este projeto foi desenvolvido por:

- **Davi Nasiasene**: Desenvolveu o esqueleto do código e a base da classe Sudoku.
- **Leonardo Chianca**: Deixou a aplicação mais amigável, melhorando logs e a jogabilidade em si.
- **Ian Bittencourt**: Implementou os métodos de verificação com o uso de threads.