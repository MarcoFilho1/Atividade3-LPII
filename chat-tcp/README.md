Atividade3-LPII
Chat TCP — Etapa 1: libtslog + Arquitetura

Este repositório entrega a Etapa 1 do projeto: uma biblioteca de logging concorrente (thread-safe) chamada libtslog, um CLI de estresse para validar concorrência e a arquitetura inicial para as próximas etapas do Servidor de Chat Multiusuário (TCP).

----------------------------------------------------------------

[ Estrutura do projeto ]

chat-tcp/
├── CMakeLists.txt                 # Script de build (CMake) do projeto
├── README.md                      # Este arquivo (versão .md); esta versão .txt está em anexo
├── include/
│   └── tslog/
│       └── tslog.hpp              # Header público da libtslog (API, tipos e monitor da fila)
├── src/
│   └── tslog/
│       ├── logger.cpp             # Thread do logger + formatação de linhas/tempo
│       └── sinks.cpp              # Implementações dos "sinks" (console/arquivo)
├── examples/
│   └── log_stress.cpp             # CLI de estresse (várias threads produzindo logs)
├── docs/
│   └── arquitetura.md             # Diagramas/visão inicial da arquitetura do Chat TCP
├── report/
│   └── Relatorio_Etapa1_libtslog.md  # Relatório (análise crítica de concorrência)
└── scripts/
    └── mostrar_saida.sh           # Script que roda o exemplo e salva o stdout em arquivo

O que cada pasta/arquivo contém:

- CMakeLists.txt
  Define a compilação da biblioteca tslog e do executável log_stress. Ajusta padrão C++20, warnings e instalação opcional.

- include/tslog/tslog.hpp
  API pública da libtslog:
    * enum Level e to_string(Level) — níveis de log (trace, debug, info, …).
    * LogRecord/SourceLoc — metadados de cada log (arquivo/linha, thread, timestamp).
    * Sink (interface), ConsoleSink, FileSink — destinos de log.
    * BoundedQueue<T> — monitor que encapsula fila limitada com semáforos + mutex.
    * Logger — thread dedicada consumindo a fila e escrevendo nos sinks; shutdown() limpo.
    * Macros TSLOG_INFO, TSLOG_WARN, etc., que capturam automaticamente arquivo/linha.

- src/tslog/logger.cpp
  Loop da thread do logger (worker_loop), formatação de timestamp/linha e escrita serializada nos sinks (garante atomicidade por linha e evita interleaving).

- src/tslog/sinks.cpp
  Implementações separadas dos sinks. Mantido isolado para reduzir recompilações quando o header muda.

- examples/log_stress.cpp
  Programa de teste: cria N threads que produzem M mensagens cada uma. Ajuda a:
    * Verificar concorrência (sem linhas truncadas/misturadas).
    * Exercitar rotação de arquivo (FileSink com limite de bytes).
    * Medir desempenho básico sob pressão.

- docs/arquitetura.md
  Visão de alto nível do Chat TCP para as Etapas 2/3:
    * Componentes: TcpServer, ClientSession, MessageRouter (monitor), ClientRegistry (monitor), libtslog.
    * Diagramas de sequência (Mermaid) do fluxo de log e do cliente-servidor.

- report/Relatorio_Etapa1_libtslog.md
  Relatório da Etapa 1 com análise crítica (race conditions, deadlocks, starvation), justificativas de design e próximos passos.
  (Quando necessário, gere o PDF: pandoc report/Relatorio_Etapa1_libtslog.md -o report/Relatorio_Etapa1_libtslog.pdf).

- scripts/mostrar_saida.sh
  Script de conveniência que:
    1) (Re)constrói o projeto se precisar,
    2) Executa log_stress,
    3) Mostra a saída na tela e salva em out/log_stress_stdout-<timestamp>.txt.
  Os logs da libtslog sempre irão para logs/app.log.

----------------------------------------------------------------

[ Requisitos de build ]

- CMake >= 3.20
- C++20 (g++ 12+ ou clang 14+)
- Linux (testado no Ubuntu 22.04).

Instalação típica (Ubuntu):
  sudo apt update
  sudo apt install -y build-essential cmake g++

Se aparecer erro com <semaphore>/<std::jthread>, instale e use o g++-12:
  sudo apt install -y g++-12
  export CC=gcc-12 CXX=g++-12

----------------------------------------------------------------

[ Como compilar e rodar (CMake) ]

Na raiz do projeto:

  mkdir -p build && cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release
  cmake --build . -j

Executar o teste de estresse:

  ./log_stress 8 1000 logs/app.log

Alternativa (script que salva a saída):
  chmod +x scripts/mostrar_saida.sh
  ./scripts/mostrar_saida.sh 8 1000
  -> cria out/log_stress_stdout-YYYY-MM-DD_HH-MM-SS.txt
     e escreve logs também em logs/app.log

Compilação sem CMake (linha única, opcional):
  g++ -std=gnu++20 -O2 -pthread -Iinclude \
    src/tslog/logger.cpp src/tslog/sinks.cpp examples/log_stress.cpp \
    -o build/log_stress
  ./build/log_stress 8 1000 logs/app.log

----------------------------------------------------------------