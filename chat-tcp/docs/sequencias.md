# Diagrama de Sequ�ncia � Etapa 3 (Servidor de Chat TCP)

O diagrama abaixo mostra o fluxo de mensagens entre clientes, servidor, fila monitor e thread broadcaster:

```mermaid
sequenceDiagram
    participant C1 as Cliente 1
    participant S as Servidor
    participant Q as Fila (Monitor)
    participant B as Broadcaster

    C1->>S: connect()
    S-->>C1: hist�rico
    C1->>S: "Ol�\n"
    S->>Q: push("Ol�\n")
    B->>Q: pop()
    B->>S: lock(clients) + broadcast("Ol�\n")
    S-->>C1: "Ol�\n"
    S-->>C2: "Ol�\n"