# Relat�rio � Etapa 1 (libtslog + Arquitetura)

**Aluno:** Marco Antonio de Vasconcelos Souza Filho  
**Matr�cula:** 20230047533
**Disciplina:** LPII-251  

## Objetivo
Projetar e validar a **libtslog** (thread-safe) para uso no servidor de chat TCP.

## Desenho de Concorr�ncia
- Produtores ? `BoundedQueue<LogRecord>` (monitor com sem�foros + mutex) ? consumidor �nico.
- `Logger` com thread dedicada; `sinks` protegidos por `sinks_mtx_`; `level_` e `run_` atomics.

## An�lise de Riscos (com aux�lio de IA)
- **Race na lista de sinks**: Mitigado com `sinks_mtx_`.
- **Perda em shutdown**: Sentinela `Level::off`, `join` e `flush`.
- **Deadlock fila/logger**: Locks de escopo reduzido; worker formata fora do lock de sinks.
- **Starvation de produtores**: Fila com capacidade configur�vel; poss�vel `try_push`/timeout em evolu��o.
- **Interleaving de linhas**: Escrita centralizada na thread do logger.

## Testes
- `log_stress` com N threads � M mensagens; checar aus�ncia de linhas truncadas/misturadas.
- Rota��o de arquivo com `rotate_bytes` pequeno.

## Pr�ximos Passos
- Criar headers de `TcpServer`, `ClientSession`, `MessageRouter`, `ClientRegistry`.
- Integrar `libtslog` nos pontos de socket (accept/connect/disconnect/erro).

## Conclus�o
A **libtslog** atende aos requisitos de threads, exclus�o m�tua, sem�foros, monitores, sockets (planejados), 
e estabelece base s�lida para a Etapa 2/3.
