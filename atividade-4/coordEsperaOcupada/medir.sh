#!/bin/bash
# Mede insercoes por segundo das tres versoes (3 repeticoes de 3 s cada)
# Uso: ./medir.sh
for imp in 0 1; do
  for m in 0 1 2; do
    for r in 1 2 3; do
      v=$(./espera_ocupada $m 3 $imp 2>&1 >/dev/null | grep "por segundo" | awk '{print $4}')
      echo "imprime=$imp modo=$m rep=$r ips=$v"
    done
  done
done
