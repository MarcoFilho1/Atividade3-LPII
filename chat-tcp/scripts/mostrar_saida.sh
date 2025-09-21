#!/usr/bin/env bash
set -euo pipefail

# Uso:
#   ./scripts/mostrar_saida.sh [threads] [msgs_por_thread]
# Exemplo:
#   ./scripts/mostrar_saida.sh 8 1000
#
#

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
OUT_DIR="$ROOT_DIR/out"
LOG_DIR="$ROOT_DIR/logs"

THREADS="${1:-8}"
MSGS="${2:-1000}"
LOGFILE="${LOG_DIR}/app.log"

mkdir -p "$BUILD_DIR" "$OUT_DIR" "$LOG_DIR"

# (re)construir se necessário
if [[ ! -x "$BUILD_DIR/log_stress" ]]; then
  echo "[info] Construindo projeto em $BUILD_DIR ..."
  ( cd "$BUILD_DIR" && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . -j )
fi

STAMP="$(date +%F_%H-%M-%S)"
OUTFILE="$OUT_DIR/log_stress_stdout-${STAMP}.txt"

echo "[info] Rodando: ./log_stress ${THREADS} ${MSGS} ${LOGFILE}"
echo "[info] Salvando stdout em: $OUTFILE"
echo

# stdbuf força flush de linha pra acompanhar em tempo real
stdbuf -oL -eL "$BUILD_DIR/log_stress" "$THREADS" "$MSGS" "$LOGFILE" | tee "$OUTFILE"

echo
echo "[ok] Terminado. Saída do programa: $OUTFILE"
echo "[ok] Logs da libtslog (arquivo): $LOGFILE"
