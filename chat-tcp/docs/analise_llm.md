Prompt usado: “Analise riscos de race conditions em um logger multithread em C++”.

Riscos identificados:

Se não houver mutex, mensagens se corrompem.

Deadlock se tentar logar dentro do próprio mutex.

Mitigação:

Uso de std::mutex e RAII.

Logger centralizado (singleton thread-safe)