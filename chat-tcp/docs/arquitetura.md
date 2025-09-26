Objetivo: Servidor de Chat Multiusuário TCP.

Etapa 1: apenas logging concorrente.

Arquitetura proposta:

Server (aceita clientes).

ClientSession (thread por cliente).

ThreadSafeQueue (monitor).

libtslog (logging thread-safe).

log_stress.cpp (teste de estresse).