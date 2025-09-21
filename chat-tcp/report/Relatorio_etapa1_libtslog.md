# Relatório — Etapa 1 (libtslog + Arquitetura)

**Aluno:** Marco Antonio de Vasconcelos Souza Filho  
**Matrícula:** 20230047533
**Disciplina:** LPII-251  

## Objetivo
Projetar e validar a **libtslog** (thread-safe) para uso no servidor de chat TCP.

## Desenho de Concorrência
- Produtores ? `BoundedQueue<LogRecord>` (monitor com semáforos + mutex) ? consumidor único.
- `Logger` com thread dedicada; `sinks` protegidos por `sinks_mtx_`; `level_` e `run_` atomics.

## Análise de Riscos (com auxílio de IA)
- **Race na lista de sinks**: Mitigado com `sinks_mtx_`.
- **Perda em shutdown**: Sentinela `Level::off`, `join` e `flush`.
- **Deadlock fila/logger**: Locks de escopo reduzido; worker formata fora do lock de sinks.
- **Starvation de produtores**: Fila com capacidade configurável; possível `try_push`/timeout em evolução.
- **Interleaving de linhas**: Escrita centralizada na thread do logger.

## Testes
- `log_stress` com N threads × M mensagens; checar ausência de linhas truncadas/misturadas.
- Rotação de arquivo com `rotate_bytes` pequeno.

## Próximos Passos
- Criar headers de `TcpServer`, `ClientSession`, `MessageRouter`, `ClientRegistry`.
- Integrar `libtslog` nos pontos de socket (accept/connect/disconnect/erro).

## Conclusão
A **libtslog** atende aos requisitos de threads, exclusão mútua, semáforos, monitores, sockets (planejados), 
e estabelece base sólida para a Etapa 2/3.
