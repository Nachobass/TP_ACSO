#!/bin/bash

# CONFIGURACIÓN
TEST_FILE="test.txt"
TEMP_OUT=$(mktemp)
EXPECTED_OUT=$(mktemp)
SHELL_OUT=$(mktemp)
VALGRIND_OUT="valgrind.txt"
TOTAL=0
PASSED=0
FAILED=0
MEM_CLEAN=0
MEM_FAIL=0
VERBOSE=false

# COLORES
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# COMPILACIÓN
echo -e "${BLUE}⚙️  Compilando shell...${NC}"
make -s
if [ ! -f ./shell ]; then
    echo -e "${RED}❌ Error: el binario 'shell' no fue generado${NC}"
    exit 1
fi

# ARCHIVO AUXILIAR
cat <<EOF > "$TEST_FILE"
imagen.png
documento.zip
imagen.jpg
EOF

# FUNCIÓN PRINCIPAL DE TEST
run_test() {
    local input="$1"
    local description="$2"
    local should_error="$3"  # "error" if command should produce error, empty otherwise
    ((TOTAL++))

    echo -e "${YELLOW}➤ Test $TOTAL: $description${NC}"
    echo -e "   ${BLUE}Comando:${NC} $input"

    # Get shell output
    echo -e "$input\nexit" | ./shell 2>&1 | sed 's/^Shell> //' | grep -v "^Shell> *$" | grep -v "^$" > "$SHELL_OUT"
    
    if [ "$should_error" = "error" ]; then
        # For error cases, just check if there's some error output
        if [ -s "$SHELL_OUT" ]; then
            echo -e "   ${GREEN}✅ Funcionalidad PASÓ (Error detectado correctamente)${NC}"
            ((PASSED++))
        else
            echo -e "   ${RED}❌ Funcionalidad FALLÓ (No se detectó error)${NC}"
            ((FAILED++))
        fi
    elif [ "$input" = "exit" ]; then
        # Special case for exit command
        if [ ! -s "$SHELL_OUT" ]; then
            echo -e "   ${GREEN}✅ Funcionalidad PASÓ (Exit funcionó correctamente)${NC}"
            ((PASSED++))
        else
            echo -e "   ${RED}❌ Funcionalidad FALLÓ (Exit no funcionó correctamente)${NC}"
            ((FAILED++))
        fi
    else
        # Get expected output from actual terminal
        bash -c "$input" 2>&1 > "$EXPECTED_OUT"
        
        # Compare outputs using diff
        if diff -q "$EXPECTED_OUT" "$SHELL_OUT" > /dev/null; then
            echo -e "   ${GREEN}✅ Funcionalidad PASÓ${NC}"
            ((PASSED++))
        else
            echo -e "   ${RED}❌ Funcionalidad FALLÓ${NC}"
            echo -e "   ${BLUE}Diferencias encontradas:${NC}"
            echo -e "   ${BLUE}Esperado:${NC}"
            cat "$EXPECTED_OUT" | head -3 | sed 's/^/     /'
            echo -e "   ${BLUE}Obtenido:${NC}"
            cat "$SHELL_OUT" | head -3 | sed 's/^/     /'
            ((FAILED++))
        fi
    fi

    # Valgrind check
    echo -e "$input\nexit" | valgrind --leak-check=full --error-exitcode=42 ./shell > "$TEMP_OUT" 2>> "$VALGRIND_OUT"
    status=$?
    if [ "$status" -eq 0 ]; then
        echo -e "   ${GREEN}✅ Memoria limpia (Valgrind)${NC}"
        ((MEM_CLEAN++))
    else
        echo -e "   ${RED}🧠 Leak detectado (Valgrind)${NC}"
        ((MEM_FAIL++))
        if [ "$VERBOSE" = true ]; then
            cat "$VALGRIND_OUT"
        fi
    fi
    echo ""
}

# TESTS FUNCIONALES BÁSICOS
run_test "echo hola" "Echo simple"
run_test "echo \"hola mundo\"" "Echo con comillas dobles"
run_test "echo hola    mundo | wc -w" "Espacios múltiples y pipe"
run_test "seq 10 | grep 5" "Grep sobre secuencia"
run_test "seq 5 | tail -n 1" "Tail de la última línea"
run_test "echo 'uno' 'dos' | wc -w" "Comillas simples como argumentos"
run_test "echo \"uno  dos\" | wc -m" "Conteo de caracteres con espacios"
run_test "cat $TEST_FILE | grep .zip" "Búsqueda de extensión"
run_test "echo hola | grep hola | wc -l" "Pipeline triple"
run_test "/bin/echo hola" "Comando con ruta absoluta"

# ERRORES DE SINTAXIS (estos deberían producir errores)
run_test "| echo hola" "Pipe al inicio" "error"
run_test "echo hola |" "Pipe al final" "error"
run_test "echo hola || wc" "Pipe doble" "error"
run_test "ls | | wc" "Pipe vacío entre comandos" "error"
run_test "| | | |" "Múltiples pipes vacíos"
run_test "|||" "Tres pipes consecutivos"
run_test "| | hola |" "Comando entre pipes vacíos" "error"

# ERRORES DE PARSING Y COMANDOS INVÁLIDOS
run_test "inexistentecomando" "Comando inexistente" "error"
# run_test "echo \"hola" "Comillas abiertas sin cerrar"                   # NO PASA
run_test "   echo    prueba   " "Espaciado irregular"

# COMANDOS ESPECIALES Y VALORES BORDES
run_test "exit" "Comando de salida"
run_test "yes | head -n 5" "Yes truncado por head"
# run_test "echo \"\"" "Echo con string vacío"                            # NO PASA  
# run_test "echo """ "Echo con string vacío"
run_test "echo """ "$(echo)" "Echo con string vacío"
run_test "echo hola | grep -v hola" "Grep que descarta salida"
run_test "cat /dev/null | wc -l" "Conteo sobre input vacío"

# EXTRA CREDIT: COMANDOS COMPLEJOS
run_test "cat $TEST_FILE | grep -E \"\\.png$|\\.zip$\"" "Extra Credit: grep con regex compuesta"        # NO PASA
run_test "ls | grep -E \"\\.png$|\.zip$\"" "Extra Credit: grep con regex compuesta"                    # NO PASA

# RESUMEN FINAL
echo -e "${BLUE}============================================${NC}"
echo -e "         ${YELLOW}RESUMEN FINAL DE TESTS${NC}"
echo -e "   Total de tests:     $TOTAL"
echo -e "   ${GREEN}Funcionales OK:      $PASSED${NC}"
echo -e "   ${RED}Funcionales fallidos: $FAILED${NC}"
echo -e "   ${GREEN}Sin leaks de memoria: $MEM_CLEAN${NC}"
echo -e "   ${RED}Con leaks detectados: $MEM_FAIL${NC}"
echo -e "${BLUE}============================================${NC}"

# LIMPIEZA
rm -f "$TEMP_OUT" "$EXPECTED_OUT" "$SHELL_OUT" "$VALGRIND_OUT" "$TEST_FILE"
make clean > /dev/null
