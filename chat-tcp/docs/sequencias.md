sequenceDiagram
    participant CLI as CLI (log_stress)
    participant Logger as libtslog::Logger
    CLI->>Logger: v�rias threads chamam .info()
    Logger->>Logger: mutex garante exclus�o m�tua
    Logger-->>CLI: escrita ordenada no arquivo/log