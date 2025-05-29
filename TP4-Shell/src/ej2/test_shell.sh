# #!/bin/bash

# # SHELL_EXEC=./shell
# SHELL_EXEC=$(realpath ./shell)
# TMP_OUT=output.txt
# PASS_COUNT=0
# FAIL_COUNT=0

# function run_test() {
#     local input="$1"
#     local expected="$2"
#     echo -e "$input" | $SHELL_EXEC > $TMP_OUT 2>&1
#     if [ ! -x "$SHELL_EXEC" ]; then
#         echo "❌ Ejecutable $SHELL_EXEC no encontrado o no es ejecutable"
#         exit 1
#     fi


#     if grep -Fq "$expected" $TMP_OUT; then
#         echo "✅ PASS: '$input' → '$expected'"
#         ((PASS_COUNT++))
#     else
#         echo "❌ FAIL: '$input'"
#         echo "   ↳ Expected: '$expected'"
#         echo "   ↳ Got:"
#         cat $TMP_OUT
#         ((FAIL_COUNT++))
#     fi
# }

# # === PREPARAR ENTORNO DE PRUEBA ===
# mkdir -p test_dir
# cd test_dir

# # Crear archivos necesarios
# touch archivo1.zip
# touch imagen.png
# touch documento.pdf
# touch texto.txt
# echo "contenido de prueba" > archivo1.txt
# echo "uno.zip dos.png" > mixto.txt

# # === TESTS ===
# run_test "ls | grep .zip" "archivo1.zip"
# run_test "ls | grep .png" "imagen.png"
# run_test "ls | grep .pdf" "documento.pdf"
# run_test "echo hola mundo | tr a-z A-Z" "HOLA MUNDO"
# run_test "cat texto.txt | wc -l" "0"
# run_test "cat archivo1.txt | grep prueba" "contenido de prueba"
# run_test "echo hola | tr a-z A-Z | tr O A" "HALA"
# run_test "ls | grep \".png .zip\"" ""  # No existe esa secuencia exacta
# run_test "invalidcmd" "execvp: No such file or directory"

# # === RESULTADO FINAL ===
# cd ..
# rm -rf test_dir
# rm -f $TMP_OUT

# echo "-------------------------------------"
# echo "Tests completados: $((PASS_COUNT + FAIL_COUNT))"
# echo "✅ Pasaron: $PASS_COUNT"
# echo "❌ Fallaron: $FAIL_COUNT"



#!/bin/bash

SHELL_EXEC=$(realpath ./shell)
TMP_OUT=$(mktemp)
VALG_LOG=$(mktemp)
PASS_COUNT=0
FAIL_COUNT=0

function run_test() {
    local input="$1"
    local expected="$2"
    local label="$3"

    echo -e "\n🔹 Test: $label"

    # Ejecutar con Valgrind, timeout de 5 segundos por seguridad
    timeout 5 valgrind --leak-check=full --error-exitcode=123 \
        --log-file="$VALG_LOG" "$SHELL_EXEC" > "$TMP_OUT" 2>/dev/null <<< "$input"

    local exit_code=$?

    if [ ! -x "$SHELL_EXEC" ]; then
        echo "❌ Ejecutable $SHELL_EXEC no encontrado o no es ejecutable"
        exit 1
    fi

    if [ $exit_code -eq 124 ]; then
        echo "❌ TIMEOUT - El programa tardó demasiado"
        ((FAIL_COUNT++))
        return
    elif [ $exit_code -eq 123 ]; then
        echo "❌ MEMORY LEAK - Detectado por Valgrind"
        cat "$VALG_LOG"
        ((FAIL_COUNT++))
        return
    fi

    if grep -Fq "$expected" "$TMP_OUT"; then
        echo "✅ PASS: '$input' → '$expected'"
        ((PASS_COUNT++))
    else
        echo "❌ FAIL: '$input'"
        echo "   ↳ Expected: '$expected'"
        echo "   ↳ Got:"
        cat "$TMP_OUT"
        ((FAIL_COUNT++))
    fi
}

# === PREPARAR ENTORNO DE PRUEBA ===
mkdir -p test_dir
cd test_dir

touch archivo1.zip
touch imagen.png
touch documento.pdf
touch texto.txt
echo "contenido de prueba" > archivo1.txt
echo "uno.zip dos.png" > mixto.txt
echo "linea1" > lines.txt; echo "linea2" >> lines.txt

# === TESTS ===
run_test "ls | grep .zip\nq" "archivo1.zip" "Filtra .zip"
run_test "ls | grep .png\nq" "imagen.png" "Filtra .png"
run_test "ls | grep .pdf\nq" "documento.pdf" "Filtra .pdf"
run_test "ls | grep -E \".png$|.zip$\"\nq" "archivo1.zip" "Filtra .zip o .png con regex"
run_test "echo hola mundo | tr a-z A-Z\nq" "HOLA MUNDO" "Mayúsculas"
run_test "cat texto.txt | wc -l\nq" "0" "Conteo líneas vacías"
run_test "cat archivo1.txt | grep prueba\nq" "contenido de prueba" "Buscar palabra"
run_test "echo hola | tr a-z A-Z | tr O A\nq" "HALA" "Pipe múltiple"
run_test "ls | grep \".png .zip\"\nq" "" "Búsqueda inválida"
run_test "invalidcmd\nq" "execvp: No such file or directory" "Comando inexistente"
run_test "echo linea1 >> lines.txt && cat lines.txt | wc -l\nq" "3" "Archivo con múltiples líneas"
run_test "ls | sort | head -n 1\nq" "archivo1.txt" "Head sobre sort"
run_test "echo prueba cd\ncd ..\npwd\nq" "$(basename $(pwd))" "cd y pwd"

# === RESULTADO FINAL ===
cd ..
rm -rf test_dir "$TMP_OUT" "$VALG_LOG"

echo "-------------------------------------"
echo "Tests completados: $((PASS_COUNT + FAIL_COUNT))"
echo "✅ Pasaron: $PASS_COUNT"
echo "❌ Fallaron: $FAIL_COUNT"
