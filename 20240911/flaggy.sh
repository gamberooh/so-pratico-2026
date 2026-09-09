#!/usr/bin/env bash
FLAG="/tmp/flaky_test.flag"

if [ ! -f "$FLAG" ]; then
    echo "[TEST-CHILD] Primo tentativo: simulo errore!"
    touch "$FLAG"
    exit 1
else
    echo "[TEST-CHILD] Secondo tentativo: ora ho successo!"
    rm -f "$FLAG"
    exit 0
fi
