  Atividade 3 – Linguagem de Programação II
  tema: Servidor de Chat Multiusuário TCP
  etapa: "Etapa 1 — Logging Concorrente + Arquitetura"
  descricao: >
    Este repositório contém a Etapa 1 do projeto, cujo objetivo final será implementar
    um Servidor de Chat Multiusuário (TCP) concorrente. Nesta entrega, a ênfase está
    apenas no logging thread-safe e na definição inicial da arquitetura.

* objetivos:
  - Integrar a biblioteca libtslog para logging concorrente.
  - Criar um teste de estresse em CLI com múltiplas threads gravando logs simultaneamente.
  - Definir a arquitetura inicial (estrutura de pastas, headers de exemplo, diagramas).
  - Marcar a entrega com a tag v1-logging.

estrutura:
  * raiz: chat-tcp/
  pastas:
    - libs/libtslog: "Biblioteca de logging thread-safe"
    - src/common/logging.hpp: "Wrapper para usar a libtslog"
    - tools/log_stress.cpp: "CLI de estresse multithread"
    - docs/arquitetura.md: "Descrição da arquitetura proposta"
    - docs/sequencias.md: "Diagramas de sequência (rascunho)"
    - docs/analise_llm.md: "Relatório de análise crítica (IA/LLMs)"
    - CMakeLists.txt: "Build com CMake"
    - README.md: "Este arquivo"

build:
  cmake:
    * passos:
      * - "mkdir -p build && cd build"
      * - "cmake .. -DCMAKE_BUILD_TYPE=Release"
      * - "cmake --build . -j"
      * - "./log_stress 8 1000"
  * resultado:
    * - "Mensagens exibidas no terminal"

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



Etapa 2 — Protótipo CLI de Comunicação (v2-cli)

Nesta etapa foi implementado o **cliente/servidor TCP mínimo** com suporte a múltiplos clientes conectados simultaneamente, além de integração com o sistema de logging thread-safe (`libtslog`).

### Objetivos
- Servidor TCP concorrente que aceita múltiplos clientes.
- Cliente CLI que conecta, envia e recebe mensagens.
- Retransmissão (broadcast) das mensagens para todos os clientes conectados.
- Logging integrado (mensagens e eventos registrados).

### Como executar
1. Compile o projeto normalmente:
   ```bash
   cd chat-tcp
   rm -rf build && mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . -j
   
   Inicie o servidor em um terminal utilizando:
   ./chat_server 5555

  Abra outros terminais e inicie clientes:
  ./chat_client 127.0.0.1 5555





  # Etapa 3 — Sistema Completo (v3-final)

Esta etapa conclui o desenvolvimento do **Servidor de Chat Multiusuário TCP**, implementando todas as funcionalidades obrigatórias do tema e demonstrando domínio prático dos conceitos de **programação concorrente**.

---

## ?? Objetivos da Etapa 3
- Concluir as **funcionalidades obrigatórias** do tema: servidor TCP concorrente com múltiplos clientes, threads, exclusão mútua, sincronização e broadcast de mensagens.  
- Integrar a **biblioteca de logging thread-safe (`libtslog`)** a todos os componentes.  
- Implementar **monitor e fila concorrente** para mensagens (com semáforos e variáveis de condição).  
- Adicionar **histórico de mensagens**, **tratamento de encerramento gracioso** e **gerenciamento de recursos**.  
- Entregar **documentação final**:
  - Diagrama de sequência cliente-servidor (`docs/sequencias.md`)
  - Mapeamento requisitos ? código (`docs/mapeamento_requisitos.md`)
  - Relatório de análise de concorrência com IA (`docs/analise_llm.md`)

---

## ?? Funcionalidades Implementadas
- **Servidor TCP concorrente** aceitando múltiplos clientes.
- **Thread de aceitação**, **thread por cliente** e **thread broadcaster** dedicada.
- **Fila monitor** (`ThreadSafeQueue`) com `std::counting_semaphore` e `std::condition_variable`.
- **Exclusão mútua**: `std::mutex` protege a lista de clientes e o histórico.
- **Histórico**: novas conexões recebem as mensagens anteriores.
- **Logging** completo com `libtslog`: conexões, mensagens, desconexões, broadcast e shutdown.
- **Encerramento gracioso (Ctrl+C)**: sinaliza `running=false`, acorda threads e fecha sockets.
- **Cliente CLI**: conecta, envia e recebe mensagens formatadas por linha.
- **Build** via CMake em Linux/Windows.

---

## ?? Como Executar

### Compilação
```bash
cd chat-tcp
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j

