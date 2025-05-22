#!/bin/bash

SHELL_EXEC=./shell
TMP_OUT=output.txt
PASS_COUNT=0
FAIL_COUNT=0

function run_test() {
    local input="$1"
    local expected="$2"
    echo -e "$input" | $SHELL_EXEC > $TMP_OUT 2>&1

    if grep -Fq "$expected" $TMP_OUT; then
        echo "✅ PASS: '$input' → '$expected'"
        ((PASS_COUNT++))
    else
        echo "❌ FAIL: '$input'"
        echo "   ↳ Expected: '$expected'"
        echo "   ↳ Got:"
        cat $TMP_OUT
        ((FAIL_COUNT++))
    fi
}

# PREPARACIÓN DEL ENTORNO
mkdir -p test_dir
cd test_dir
touch archivo1.zip imagen.png documento.pdf texto.txt

# === TESTS ===
run_test "ls | grep .zip\nq" "archivo1.zip"
run_test "ls | grep .png\nq" "imagen.png"
run_test "echo hola mundo | tr a-z A-Z\nq" "HOLA MUNDO"
run_test "ls | grep \".png .zip\"\nq" ""  # Debe fallar porque no hay esa secuencia exacta
run_test "ls | grep \".pdf\"\nq" "documento.pdf"
run_test "cat texto.txt | wc -l\nq" "0"  # archivo vacío
run_test "echo 'hola mundo' | tr 'a-z' 'A-Z'\nq" "HOLA MUNDO"
run_test "invalidcmd\nq" "execvp"

# === RESULTADO ===
cd ..
rm -rf test_dir
rm -f $TMP_OUT

echo "-------------------------------------"
echo "Tests completados: $((PASS_COUNT + FAIL_COUNT))"
echo "✅ Pasaron: $PASS_COUNT"
echo "❌ Fallaron: $FAIL_COUNT"
