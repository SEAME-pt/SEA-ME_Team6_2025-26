#!/bin/bash

# Script para fazer o setup do Google Test

echo "=== Setup do Google Test ==="

# Criar diretório para bibliotecas externas
mkdir -p external

# Clonar o Google Test
if [ ! -d "external/googletest" ]; then
    echo "Clonando Google Test..."
    git clone https://github.com/google/googletest.git external/googletest
    cd external/googletest
    git checkout release-1.12.1
    cd ../..
    echo "Google Test clonado com sucesso!"
else
    echo "Google Test já existe!"
fi

echo "=== Setup completo! ==="
