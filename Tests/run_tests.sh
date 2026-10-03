#!/usr/bin/env bash
set -euo pipefail

# --- 1. Process Management & Cleanup ---
cleanup() {
    echo "[*] Cleanup: terminating all child processes..." >&2
    # Killa tutti i figli diretti e indiretti di questo script
    if [ -n "$MAKE_DEBUG_PID" ]; then
        kill -9 -"$MAKE_DEBUG_PID" 2>/dev/null || true
    fi
    pkill -TERM -P $$ 2>/dev/null || true
    sleep 1
    pkill -KILL -P $$ 2>/dev/null || true
    rm -f .temp_script.gdb
}
trap cleanup EXIT SIGINT SIGTERM


TEST_DIR="./tests"
LOG_DIR="./logs/"
GLOBAL_LOG="${LOG_DIR}global_tests_trace.log"
VALIDATION_LOG="${LOG_DIR}validation_tests_trace.log"
OVERHEAD_LOG="${LOG_DIR}overhead_trace.log"
TEST_FILES=()
MAKE_DEBUG_PID=""
TICKS_TO_RUN=1000

for script in "validator.gdb" "logger.gdb"; do
    if [ ! -x "$script" ]; then
        echo "[-] FATAL: $script not found or not executable. Run: chmod +x $script" >&2
        exit 1
    fi
done
 
# --- 3. Input Validation ---
if [ $# -eq 0 ]; then
    for file in "$TEST_DIR"/*.c; do
        [ -e "$file" ] || continue
        TEST_FILES+=("$(basename "$file")")
    done
else
    for file in "$@"; do
        if [ ! -f "$TEST_DIR/$file" ] && [ ! -f "$file" ]; then
            echo "[-] FATAL: Provided file '$file' does not exist." >&2
            exit 1 
        fi
        TEST_FILES+=("$(basename "$file")")
    done
fi
echo "TEST FILES: ${TEST_FILES[@]}"

mkdir -p "$LOG_DIR"
printf "" > "$GLOBAL_LOG"
printf "" > "$VALIDATION_LOG"
printf "" > "$OVERHEAD_LOG"

# --- 4. The Execution Loop ---
for main_file in "${TEST_FILES[@]}"; do
    test_name="${main_file%.*}"
    echo "============================================================="
    echo "[*] TARGET: $main_file"
    echo "============================================================="

    echo "[*] 1. Cleaning and Building Project..."
    make clean > /dev/null
    
    if ! make all MAIN_FILE="$main_file" > /dev/null; then
        echo "[-] ERROR: Compilation failed for $main_file. Skipping to next..." >&2
        continue
    fi

    echo "[*] 2. Starting QEMU..."
    make qemu_debug > /dev/null 2>&1 &
    MAKE_DEBUG_PID=$!
    sleep 1

    echo "[*] 3. Attaching GDB and Executing..."

    printf "\n=============================================================\n" >> "$GLOBAL_LOG"
    printf "  TARGET TEST: %s \n" "$main_file" >> "$GLOBAL_LOG"
    printf "=============================================================\n" >> "$GLOBAL_LOG"

    cat << EOF > .temp_script.gdb
set auto-load safe-path /
target remote :1234

set \$hit_vconfig = 0
set \$hit_start = 0
set \$config_ok = 0

break vConfigureScheduler
commands
    set \$hit_vconfig = 1
end

break vTaskStartScheduler
commands
    set \$hit_start = 1
end

continue

if \$hit_vconfig == 1
    finish
    clear vConfigureScheduler
    set \$config_ok = \$1
    
    if \$config_ok != 0
        continue
    end
end

if \$hit_start == 1
    clear vTaskStartScheduler
    
    if \$hit_vconfig == 0
        finish
    else
        set \$target_ticks = ulTestTicksToRun
        break xTaskIncrementTick if xTickCount + 1== \$target_ticks
        continue
        finish
        clear xTaskIncrementTick
    end
end

source logger.gdb
set logging file ${GLOBAL_LOG}
set logging overwrite off
set logging enabled on
show_logger
set logging enabled off

source validator.gdb
set logging file ${VALIDATION_LOG}
set logging overwrite off
set logging enabled on
validate_${test_name}
set logging enabled off

EOF
    if [ "$main_file" = "main_test_32.c" ]; then
        cat << EOF >> .temp_script.gdb
source overhead.py
set logging file ${OVERHEAD_LOG}
set logging overwrite off
set logging enabled on
show_instruction_overhead
show_tick_overhead
set logging enabled off
EOF
    fi
echo "quit" >> .temp_script.gdb

    gdb-multiarch ./Output/demo.elf --batch -x .temp_script.gdb > /dev/null 2>&1
    rm -f .temp_script.gdb

    echo "[*] 4. Killing emulator processes..."    
    if [ -n "$MAKE_DEBUG_PID" ]; then
        pkill -9 -P "$MAKE_DEBUG_PID" 2>/dev/null || true
        kill -9 "$MAKE_DEBUG_PID" 2>/dev/null || true
        wait "$MAKE_DEBUG_PID" 2>/dev/null || true
    fi

    printf "[+] DONE: %s\n" "$main_file"
done

echo "=================================================="
