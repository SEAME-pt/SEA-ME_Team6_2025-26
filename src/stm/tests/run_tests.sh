#!/bin/bash

# Script para compilar e executar todos os testes do STM32 Team6

echo "=========================================="
echo "  STM32 Unit Tests - Google Test"
echo "=========================================="
echo ""

# Criar diretório de build se não existir
if [ ! -d "build" ]; then
    echo "[1/4] Criando diretório build..."
    mkdir -p build
fi

# Compilar os testes
echo "[2/4] Compilando testes..."
cd build
cmake .. > /dev/null 2>&1
make

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ Erro na compilação!"
    exit 1
fi

echo ""
echo "[3/4] Executando testes..."
echo "=========================================="
echo ""

# Contador de resultados
total_tests=0
passed_tests=0
failed_tests=0

# Executar Speedometer Tests
echo "🔬 [TESTE 1/3] Speedometer Tests"
echo "------------------------------------------"
./speedometer_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ Speedometer: PASSOU"
    ((passed_tests++))
else
    echo "❌ Speedometer: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Executar Motor Control Tests
echo "⚙️  [TESTE 2/3] Motor Control Tests"
echo "------------------------------------------"
./motor_control_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ Motor Control: PASSOU"
    ((passed_tests++))
else
    echo "❌ Motor Control: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Executar Emergency Stop Tests
echo "🚨 [TESTE 3/3] Emergency Stop Tests"
echo "------------------------------------------"
./emergency_stop_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ Emergency Stop: PASSOU"
    ((passed_tests++))
else
    echo "❌ Emergency Stop: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Resumo final
echo "=========================================="
echo "[4/4] Resumo dos Testes"
echo "=========================================="
echo "Total de suítes: $total_tests"
echo "Passou: $passed_tests"
echo "Falhou: $failed_tests"
echo ""

if [ $failed_tests -eq 0 ]; then
    echo "✅ SUCESSO: Todos os testes passaram!"
    echo "=========================================="
    exit 0
else
    echo "❌ FALHA: $failed_tests suite(s) falharam!"
    echo "=========================================="
    exit 1
fi
