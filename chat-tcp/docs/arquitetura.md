Objetivo: Servidor de Chat Multiusu�rio TCP.

Etapa 1: apenas logging concorrente.

Arquitetura proposta:

Server (aceita clientes).

ClientSession (thread por cliente).

ThreadSafeQueue (monitor).

libtslog (logging thread-safe).

log_stress.cpp (teste de estresse).