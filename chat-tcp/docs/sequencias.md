sequenceDiagram
    participant CLI as CLI (log_stress)
    participant Logger as libtslog::Logger
    CLI->>Logger: várias threads chamam .info()
    Logger->>Logger: mutex garante exclusão mútua
    Logger-->>CLI: escrita ordenada no arquivo/log