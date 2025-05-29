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

    if [ ! -x "$SHELL_EXEC" ]; then
        echo "❌ Ejecutable $SHELL_EXEC no encontrado o no es ejecutable"
        exit 1
    fi

    timeout 5 valgrind --leak-check=full --error-exitcode=123 \
        --log-file="$VALG_LOG" "$SHELL_EXEC" > "$TMP_OUT" 2>/dev/null <<< "$input"
    local exit_code=$?

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

    # if grep -Fq "$expected" "$TMP_OUT"; then
    if grep -F "$expected" "$TMP_OUT" | grep -vq "^Shell>$"; then
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

touch archivo1.zip imagen.png documento.pdf texto.txt
echo "contenido de prueba" > archivo1.txt
echo "uno.zip dos.png" > mixto.txt
echo -e "linea1\nlinea2\nlinea3" > lines.txt

# === TESTS FUNCIONALES ===
run_test "ls | grep .zip" "archivo1.zip" "Filtra .zip"
run_test "ls | grep .png" "imagen.png" "Filtra .png"
run_test "ls | grep .pdf" "documento.pdf" "Filtra .pdf"
# run_test "ls | grep -E \"\\.png\$|\\.zip\$\"" "archivo1.zip" "Regex OR con grep -E"
run_test 'ls | grep -E "\.png$|\.zip$"' "archivo1.zip" "Regex OR con grep -E"
run_test "echo hola mundo | tr a-z A-Z" "HOLA MUNDO" "Mayúsculas"
run_test "cat texto.txt | wc -l" "0" "Archivo vacío"
run_test "cat archivo1.txt | grep prueba" "contenido de prueba" "Buscar palabra"
run_test "echo hola | tr a-z A-Z | tr O A" "HALA" "Pipes encadenados"
run_test "ls | grep \".png .zip\"" "" "Coincidencia inválida"
run_test "invalidcmd" "execvp: No such file or directory" "Comando inválido"
run_test "cat lines.txt | wc -l" "3" "Conteo de líneas"
run_test "ls | sort | head -n 1" "archivo1.txt" "Sort + head"
run_test "echo hola" "hola" "echo simple"
run_test "ls | wc -l" "" "Conteo de archivos"
run_test "echo \"hola mundo\"" "hola mundo" "Comillas dobles"
run_test "echo hola    mundo | wc -w" "2" "Espacios múltiples"
run_test "whoami | grep $(whoami)" "$(whoami)" "Usuario actual"
run_test "seq 10 | grep 5" "5" "Grep número"
run_test "seq 5 | tail -n 1" "5" "Última línea"
run_test "echo 'uno' 'dos' | wc -w" "2" "Palabras entre comillas simples"
run_test "echo \"uno  dos\" | wc -m" "9" "Espacios dobles en string"
run_test "echo hola | grep hola | wc -l" "1" "Triple pipe válido"

# === CASOS BORDE ===
run_test "   echo    prueba   " "prueba" "Espacios iniciales y finales"
run_test "| echo hola" "" "Pipe al inicio (mal formado)"
run_test "echo hola |" "" "Pipe al final (mal formado)"
run_test "echo hola || wc" "" "Doble pipe (no implementado)"
run_test "inexistentecomando" "execvp: No such file or directory" "Comando inexistente"
run_test "" "" "Línea vacía"
run_test "     " "" "Línea con solo espacios"
run_test "exit\n" "" "Comando de salida"

# === RESULTADO FINAL ===
cd ..
rm -rf test_dir "$TMP_OUT" "$VALG_LOG"

echo "-------------------------------------"
echo "Tests completados: $((PASS_COUNT + FAIL_COUNT))"
echo "✅ Pasaron: $PASS_COUNT"
echo "❌ Fallaron: $FAIL_COUNT"
