#!/bin/bash
# Remove as filas criadas pelos testes
for n in processo_a processo_b processo_a_corrigido processo_b_corrigido; do pkill -x $n 2>/dev/null; done
rm -f /dev/mqueue/fila-mirelle-*
