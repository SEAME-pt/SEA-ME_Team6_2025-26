#!/bin/bash

# Script para fazer o setup do Google Test

echo "=== Setup do Google Test ==="

# Criar diretório para bibliotecas externas
mkdir -p external

# Clonar o Google Test (ou atualizar se estiver incompleto)
if [ ! -d "external/googletest" ] || [ ! -f "external/googletest/CMakeLists.txt" ]; then
    echo "Clonando Google Test..."
    rm -rf external/googletest
    git clone https://github.com/google/googletest.git external/googletest
    if [ $? -ne 0 ]; then
        echo "Falha ao clonar googletest. Verifique a conexão de rede ou clone manualmente."
        exit 1
    fi
    cd external/googletest || exit 1
    git checkout release-1.12.1 || true
    cd ../..
    echo "Google Test clonado com sucesso!"
else
    echo "Google Test já existe e parece completo." 
fi

echo "=== Setup completo! ==="
