# TO RUN:
# chmod +x compare_everything.py
# python3 compare_everything.py


import subprocess

file_names = ["add_mul.x", "add.x", "addis.x", "adds_add_subs.x", "adds-subs.x", "adds.x", "ands.x", "b.x", "beq.x", "bge.x",
              "bgt.x", "ble.x", "blt.x", "bne.x", "br.x", "branch_if.x", "cbnz.x", "cbz_cbnz.x", "cbz.x", "cmp_ex.x", 
              "cmp.x", "conditional_branches.x", "eor.x", "ldurh.x", "lsl.x", "lsr_lsl.x", "lsr.x", "memory_operations_and_shifts.x", 
              "movz.x", "mul.x", "orr.x", "sturb.x", "sturh.x", "subis.x"]

expected_message_1 = "✅ No differences found in dumpsim files."
expected_message_2 = "✅ All simulations returned the same memory dump for the given cycles."

all_match = True
failing_files = []  # Lista para almacenar los archivos que fallan

for file_name in file_names:
    command = f"./compare_multiple_cycles.sh ./inputs/bytecodes/{file_name}"
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    output = result.stdout.strip()
    
    print(f"Output para {file_name}: {output}")
    
    if expected_message_1 not in output or expected_message_2 not in output:
        all_match = False
        failing_files.append(file_name)  # Agregar el archivo que falla a la lista

if all_match:
    print("****************************************")
    print("****************************************")
    print("All files returned the expected message.")
    print("****************************************")
    print("****************************************")
else:
    print("******************************************************")
    print("******************************************************")
    print("At least one file did not return the expected message.")
    print("******************************************************")
    print("******************************************************")
    print("Archivos que fallaron:")
    for file_name in failing_files:
        print(f"- {file_name}")