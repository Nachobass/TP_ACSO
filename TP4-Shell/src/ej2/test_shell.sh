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

# === PREPARAR ENTORNO DE PRUEBA ===
mkdir -p test_dir
cd test_dir

# Crear archivos necesarios
touch archivo1.zip
touch imagen.png
touch documento.pdf
touch texto.txt
echo "contenido de prueba" > archivo1.txt
echo "uno.zip dos.png" > mixto.txt

# === TESTS ===
run_test "ls | grep .zip\nq" "archivo1.zip"
run_test "ls | grep .png\nq" "imagen.png"
run_test "ls | grep .pdf\nq" "documento.pdf"
run_test "echo hola mundo | tr a-z A-Z\nq" "HOLA MUNDO"
run_test "cat texto.txt | wc -l\nq" "0"
run_test "cat archivo1.txt | grep prueba\nq" "contenido de prueba"
run_test "echo hola | tr a-z A-Z | tr O A\nq" "HALA"
run_test "ls | grep \".png .zip\"\nq" ""  # No existe esa secuencia exacta
run_test "invalidcmd\nq" "execvp"

# === RESULTADO FINAL ===
cd ..
rm -rf test_dir
rm -f $TMP_OUT

echo "-------------------------------------"
echo "Tests completados: $((PASS_COUNT + FAIL_COUNT))"
echo "✅ Pasaron: $PASS_COUNT"
echo "❌ Fallaron: $FAIL_COUNT"
