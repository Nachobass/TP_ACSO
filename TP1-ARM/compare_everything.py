import subprocess

# Lista de nombres de archivo
file_names = ["add_mul.x", "add.x", "addis.x", "adds-subs.x", "adds.x", "ands.x", "b.x", "beq.x", "bge.x",
              "bgt.x", "ble.x", "blt.x", "bne.x", "br.x", "cbnz.x", "cbz_cbnz.x", "cbz.x", "cmp_ex.x", 
              "cmp.x", "eor.x", "lsl.x", "lsr_lsl.x", "lsr.x", "movz.x", "mul.x", "orr.x", 
              "sturb.x", "subis.x"]

# Mensaje esperado
expected_message_1 = "✅ No differences found in dumpsim files."
expected_message_2 = "✅ All simulations returned the same memory dump for the given cycles."

# Variable para verificar si todos cumplen
all_match = True

for file_name in file_names:
    command = f"./compare_multiple_cycles.sh ./inputs/bytecodes/{file_name}"
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    output = result.stdout.strip()
    
    print(f"Output para {file_name}: {output}")
    
    if expected_message_1 not in output or expected_message_2 not in output:
        all_match = False

if all_match:
    print("Todos los archivos devolvieron el mensaje esperado.")
else:
    print("Al menos un archivo no devolvió el mensaje esperado.")