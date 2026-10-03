import gdb

class ShowInstructionOverhead(gdb.Command):

    def __init__(self):
        super(ShowInstructionOverhead, self).__init__("show_instruction_overhead", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        try:
            ulInstIndex = int(gdb.parse_and_eval("ulInstIndex"))
            xTraceInst = gdb.parse_and_eval("xTraceInst")
        except gdb.error as e:
            print(f"[-] Error: Could not read trace variables ({e}). Are symbols loaded?")
            return

        if ulInstIndex == 0:
            print("[-] Error: Trace array is empty (ulInstIndex is 0).")
            return

        val_0 = None
        val_1 = None
        
        # Policy Tracking (Flags 4 and 5)
        sum_policy = 0
        count_policy = 0
        in_policy = False
        policy_start_val = 0
        
        # Mechanism Tracking (Flags 2 and 3)
        sum_mech = 0
        count_mech = 0
        in_mech = False
        mech_start_val = 0

        # --- QEMU CALIBRATION ---
        # 1 SysTick unit = 20 CPU instructions
        QEMU_INSTRUCTION_RATIO = 20.0  

        for i in range(ulInstIndex):
            entry = xTraceInst[i]
            flag = int(entry['flag'])
            count = int(entry['count'])
            
            if flag == 0 and val_0 is None:
                val_0 = count
            elif flag == 1 and val_1 is None:
                val_1 = count
                
            # --- POLICY EVALUATION (4->5) ---
            elif flag == 4:
                if not in_policy:
                    policy_start_val = count
                    in_policy = True
            elif flag == 5:
                if in_policy:
                    sum_policy += (count - policy_start_val)
                    count_policy += 1
                    in_policy = False
                    
            # --- MECHANISM EVALUATION (2->3) ---
            elif flag == 2:
                if not in_mech:
                    mech_start_val = count
                    in_mech = True
            elif flag == 3:
                if in_mech:
                    sum_mech += (count - mech_start_val)
                    count_mech += 1
                    in_mech = False

        if val_0 is None or val_1 is None:
            print("[-] Error: Could not find both flag 0 and flag 1 in the trace.")
            return

        # Calculate Raw Frame Time
        raw_frame_time = val_1 - val_0
        if raw_frame_time <= 0:
            print("[-] Error: Total frame time is zero or negative.")
            return

        # Scale Everything to Realistic Instructions via QEMU Ratio
        total_frame_time = raw_frame_time * QEMU_INSTRUCTION_RATIO
        sum_policy = sum_policy * QEMU_INSTRUCTION_RATIO
        sum_mech = sum_mech * QEMU_INSTRUCTION_RATIO

        # Final Aggregation
        total_overhead = sum_policy + sum_mech
        
        ratio_policy = sum_policy / total_frame_time
        ratio_mech = sum_mech / total_frame_time
        ratio_total = total_overhead / total_frame_time

        avg_policy = (sum_policy / count_policy) if count_policy > 0 else 0
        avg_mech = (sum_mech / count_mech) if count_mech > 0 else 0

        fmt = lambda r: f"{r:.8f}".replace('.', ',')

        print("\n================ OVERHEAD REPORT ================")
        print(f"Data Points Parsed      : {ulInstIndex} traces")
        print(f"QEMU Scaling Ratio      : 1 SysTick = {QEMU_INSTRUCTION_RATIO} instructions")
        print(f"Total Major Frame Time  : {total_frame_time:.0f} instructions")
        print("-------------------------------------------------")
        print(f"Policy Evaluations      : {count_policy} (Avg: {avg_policy:.1f} instrs/eval)")
        print(f"Context Switches (Mech) : {count_mech} (Avg: {avg_mech:.1f} instrs/switch)")
        print("-------------------------------------------------")
        print(f"Policy Overhead         : {sum_policy:<8.0f} instrs  ({fmt(ratio_policy)})")
        print(f"Mechanism Overhead      : {sum_mech:<8.0f} instrs  ({fmt(ratio_mech)})")
        print("-------------------------------------------------")
        print(f"TOTAL SYSTEM OVERHEAD   : {total_overhead:<8.0f} instrs  ({fmt(ratio_total)})")
        print("=================================================\n")



class ShowIncrementTickOverhead(gdb.Command):

    def __init__(self):
        super(ShowIncrementTickOverhead, self).__init__("show_tick_overhead", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        try:
            ulInstIndex = int(gdb.parse_and_eval("ulInstIndex"))
            xTraceInst = gdb.parse_and_eval("xTraceInst")
        except gdb.error as e:
            print(f"[-] Error: Could not read trace variables ({e}). Are symbols loaded?")
            return

        if ulInstIndex == 0:
            print("[-] Error: Trace array is empty (ulInstIndex is 0).")
            return

        sum_empty_tick = 0
        count_empty_tick = 0
        sum_switch_tick = 0
        count_switch_tick = 0
        
        in_tick = False
        tick_start_val = 0
        tick_caused_switch = False

        # --- QEMU CALIBRATION ---
        # The ratio of simulated CPU Frequency to the 1GHz icount base.
        # e.g., 50MHz board = 20 instructions per SysTick unit.
        # e.g., 16MHz board = 62.5 instructions per SysTick unit.
        QEMU_INSTRUCTION_RATIO = 20.0  

        for i in range(ulInstIndex):
            entry = xTraceInst[i]
            flag = int(entry['flag'])
            count = int(entry['count'])
                
            if flag == 6:
                if not in_tick:
                    tick_start_val = count
                    in_tick = True
                    tick_caused_switch = False
            elif flag == 4:
                if in_tick:
                    tick_caused_switch = True
            elif flag == 7:
                if in_tick:
                    true_diff = (count - tick_start_val)
                    if tick_caused_switch:
                        sum_switch_tick += true_diff
                        count_switch_tick += 1
                    else:
                        sum_empty_tick += true_diff
                        count_empty_tick += 1
                    in_tick = False

        # Apply the QEMU Multiplier
        sum_empty_tick = sum_empty_tick * QEMU_INSTRUCTION_RATIO
        sum_switch_tick = sum_switch_tick * QEMU_INSTRUCTION_RATIO

        avg_empty = (sum_empty_tick / count_empty_tick) if count_empty_tick > 0 else 0
        avg_switch = (sum_switch_tick / count_switch_tick) if count_switch_tick > 0 else 0

        print("\n============= TICK OVERHEAD ISOLATION =============")
        print(f"Data Points Parsed      : {ulInstIndex} traces")
        print(f"QEMU Scaling Ratio      : 1 SysTick = {QEMU_INSTRUCTION_RATIO} instructions")
        print("-------------------------------------------------")
        print(f"Empty RTOS Ticks        : {count_empty_tick}")
        print(f"  -> Total Instructions : {sum_empty_tick:.0f}")
        print(f"  -> Average per Tick   : {avg_empty:.1f} instrs")
        print("-------------------------------------------------")
        print(f"Switching RTOS Ticks    : {count_switch_tick}")
        print(f"  -> Total Instructions : {sum_switch_tick:.0f}")
        print(f"  -> Average per Tick   : {avg_switch:.1f} instrs")
        print("=================================================\n")





# Instantiate the command so it loads into GDB automatically
ShowIncrementTickOverhead()
ShowInstructionOverhead()


