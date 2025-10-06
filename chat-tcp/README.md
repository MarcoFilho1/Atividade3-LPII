  Atividade 3 � Linguagem de Programa��o II
  tema: Servidor de Chat Multiusu�rio TCP
  etapa: "Etapa 1 � Logging Concorrente + Arquitetura"
  descricao: >
    Este reposit�rio cont�m a Etapa 1 do projeto, cujo objetivo final ser� implementar
    um Servidor de Chat Multiusu�rio (TCP) concorrente. Nesta entrega, a �nfase est�
    apenas no logging thread-safe e na defini��o inicial da arquitetura.

* objetivos:
  - Integrar a biblioteca libtslog para logging concorrente.
  - Criar um teste de estresse em CLI com m�ltiplas threads gravando logs simultaneamente.
  - Definir a arquitetura inicial (estrutura de pastas, headers de exemplo, diagramas).
  - Marcar a entrega com a tag v1-logging.

estrutura:
  * raiz: chat-tcp/
  pastas:
    - libs/libtslog: "Biblioteca de logging thread-safe"
    - src/common/logging.hpp: "Wrapper para usar a libtslog"
    - tools/log_stress.cpp: "CLI de estresse multithread"
    - docs/arquitetura.md: "Descri��o da arquitetura proposta"
    - docs/sequencias.md: "Diagramas de sequ�ncia (rascunho)"
    - docs/analise_llm.md: "Relat�rio de an�lise cr�tica (IA/LLMs)"
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
    O programa log_stress cria v�rias threads escrevendo no log ao mesmo tempo.
    Isso comprova que a biblioteca � thread-safe e evita corrup��o de mensagens.
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



Etapa 2 � Prot�tipo CLI de Comunica��o (v2-cli)

Nesta etapa foi implementado o **cliente/servidor TCP m�nimo** com suporte a m�ltiplos clientes conectados simultaneamente, al�m de integra��o com o sistema de logging thread-safe (`libtslog`).

### Objetivos
- Servidor TCP concorrente que aceita m�ltiplos clientes.
- Cliente CLI que conecta, envia e recebe mensagens.
- Retransmiss�o (broadcast) das mensagens para todos os clientes conectados.
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





  # Etapa 3 � Sistema Completo (v3-final)

Esta etapa conclui o desenvolvimento do **Servidor de Chat Multiusu�rio TCP**, implementando todas as funcionalidades obrigat�rias do tema e demonstrando dom�nio pr�tico dos conceitos de **programa��o concorrente**.

---

## ?? Objetivos da Etapa 3
- Concluir as **funcionalidades obrigat�rias** do tema: servidor TCP concorrente com m�ltiplos clientes, threads, exclus�o m�tua, sincroniza��o e broadcast de mensagens.  
- Integrar a **biblioteca de logging thread-safe (`libtslog`)** a todos os componentes.  
- Implementar **monitor e fila concorrente** para mensagens (com sem�foros e vari�veis de condi��o).  
- Adicionar **hist�rico de mensagens**, **tratamento de encerramento gracioso** e **gerenciamento de recursos**.  
- Entregar **documenta��o final**:
  - Diagrama de sequ�ncia cliente-servidor (`docs/sequencias.md`)
  - Mapeamento requisitos ? c�digo (`docs/mapeamento_requisitos.md`)
  - Relat�rio de an�lise de concorr�ncia com IA (`docs/analise_llm.md`)

---

## ?? Funcionalidades Implementadas
- **Servidor TCP concorrente** aceitando m�ltiplos clientes.
- **Thread de aceita��o**, **thread por cliente** e **thread broadcaster** dedicada.
- **Fila monitor** (`ThreadSafeQueue`) com `std::counting_semaphore` e `std::condition_variable`.
- **Exclus�o m�tua**: `std::mutex` protege a lista de clientes e o hist�rico.
- **Hist�rico**: novas conex�es recebem as mensagens anteriores.
- **Logging** completo com `libtslog`: conex�es, mensagens, desconex�es, broadcast e shutdown.
- **Encerramento gracioso (Ctrl+C)**: sinaliza `running=false`, acorda threads e fecha sockets.
- **Cliente CLI**: conecta, envia e recebe mensagens formatadas por linha.
- **Build** via CMake em Linux/Windows.

---

## ?? Como Executar

### Compila��o
```bash
cd chat-tcp
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j

