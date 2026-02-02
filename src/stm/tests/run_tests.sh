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
# If GoogleTest is not present, try to run the setup script to fetch it
if [ ! -f "../external/googletest/CMakeLists.txt" ]; then
    echo "GoogleTest not found. Running setup_gtest.sh..."
    ../setup_gtest.sh || {
        echo "❌ Falha ao configurar GoogleTest. Execute ../setup_gtest.sh manualmente." 
        exit 1
    }
fi

# Run cmake and show output in case of errors (don't hide errors)
cmake ..
if [ $? -ne 0 ]; then
    echo "\n❌ CMake configuration failed. See output above."
    exit 1
fi

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
echo "🔬 [TESTE 1/5] Speedometer Tests"
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
echo "⚙️  [TESTE 2/5] Motor Control Tests"
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
echo "🚨 [TESTE 3/5] Emergency Stop Tests"
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

# Executar IIS2MDC Tests
echo "🧭 [TESTE 4/5] IIS2MDC Tests"
echo "------------------------------------------"
./iis2mdc_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ IIS2MDC: PASSOU"
    ((passed_tests++))
else
    echo "❌ IIS2MDC: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Executar HTS221 Tests
echo "🌡️  [TESTE 5/7] HTS221 Tests"
echo "------------------------------------------"
./hts221_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ HTS221: PASSOU"
    ((passed_tests++))
else
    echo "❌ HTS221: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Executar LPS22HH Tests
echo "🌬️  [TESTE 6/7] LPS22HH Tests"
echo "------------------------------------------"
./lps22hh_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ LPS22HH: PASSOU"
    ((passed_tests++))
else
    echo "❌ LPS22HH: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Executar ISM330DHCX Tests
echo "🛰️  [TESTE 7/8] ISM330DHCX Tests"
echo "------------------------------------------"
./ism330dhcx_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ ISM330DHCX: PASSOU"
    ((passed_tests++))
else
    echo "❌ ISM330DHCX: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Executar VEML6030 Tests
echo "💡 [TESTE 8/8] VEML6030 Tests"
echo "------------------------------------------"
./veml6030_tests
result=$?
if [ $result -eq 0 ]; then
    echo "✅ VEML6030: PASSOU"
    ((passed_tests++))
else
    echo "❌ VEML6030: FALHOU"
    ((failed_tests++))
fi
((total_tests++))
echo ""

# Resumo final
echo "=========================================="
echo "[8/8] Resumo dos Testes"
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
