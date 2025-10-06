# Diagrama de Sequência — Etapa 3 (Servidor de Chat TCP)

O diagrama abaixo mostra o fluxo de mensagens entre clientes, servidor, fila monitor e thread broadcaster:

```mermaid
sequenceDiagram
    participant C1 as Cliente 1
    participant S as Servidor
    participant Q as Fila (Monitor)
    participant B as Broadcaster

    C1->>S: connect()
    S-->>C1: histórico
    C1->>S: "Olá\n"
    S->>Q: push("Olá\n")
    B->>Q: pop()
    B->>S: lock(clients) + broadcast("Olá\n")
    S-->>C1: "Olá\n"
    S-->>C2: "Olá\n"