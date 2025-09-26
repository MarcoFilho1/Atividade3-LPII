projeto:
  nome: "Atividade 3 – Linguagem de Programação II"
  tema: "Servidor de Chat Multiusuário (TCP)"
  etapa: "Etapa 1 — Logging Concorrente + Arquitetura"
  descricao: >
    Este repositório contém a Etapa 1 do projeto, cujo objetivo final será implementar
    um Servidor de Chat Multiusuário (TCP) concorrente. Nesta entrega, a ênfase está
    apenas no logging thread-safe e na definição inicial da arquitetura.

objetivos:
  - Integrar a biblioteca libtslog para logging concorrente.
  - Criar um teste de estresse em CLI com múltiplas threads gravando logs simultaneamente.
  - Definir a arquitetura inicial (estrutura de pastas, headers de exemplo, diagramas).
  - Marcar a entrega com a tag v1-logging.

estrutura:
  raiz: chat-tcp/
  pastas:
    libs/libtslog: "Biblioteca de logging thread-safe"
    src/common/logging.hpp: "Wrapper para usar a libtslog"
    tools/log_stress.cpp: "CLI de estresse multithread"
    docs/arquitetura.md: "Descrição da arquitetura proposta"
    docs/sequencias.md: "Diagramas de sequência (rascunho)"
    docs/analise_llm.md: "Relatório de análise crítica (IA/LLMs)"
    CMakeLists.txt: "Build com CMake"
    README.md: "Este arquivo"

build:
  cmake:
    passos:
      - "mkdir -p build && cd build"
      - "cmake .. -DCMAKE_BUILD_TYPE=Release"
      - "cmake --build . -j"
      - "./log_stress 8 1000"
  resultado:
    - "Mensagens exibidas no terminal"

teste_logging:
  descricao: >
    O programa log_stress cria várias threads escrevendo no log ao mesmo tempo.
    Isso comprova que a biblioteca é thread-safe e evita corrupção de mensagens.
  exemplo:
    comando: "./log_stress 4 20"
    saida_trecho: |
      Rodando stress test: 4 threads, 20 mensagens cada
      2025-09-26 14:12:01 [INFO ] [T0] msg #0
      2025-09-26 14:12:01 [INFO ] [T1] msg #0
      ...

documentacao:
  - docs/arquitetura.md
  - docs/sequencias.md
  - docs/analise_llm.md
