define validate_main_test_01
    set $i = 0
    set $found_start      = 0
    set $found_terminated = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 3
            set $found_start = 1
        end
        if $event_id == 7
            set $found_terminated = 1
        end
        set $i = $i + 1
    end

    printf "Test 1 - HRT task complete before their deadline "
    if $found_start == 1 && $found_terminated == 0
        printf "PASSED\n"
    else
        printf "FAILED (Expected no termination, but HRT_TERMINATED was found or HRT never started)\n"
    end
end

define validate_main_test_02
    set $i = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 7
            set $found_terminated = 1
        end
        set $i = $i + 1
    end

    printf "Test 2 - HRT tasks exceed their deadlines: "
    if $found_terminated == 1
        printf "PASSED\n"
    else
        printf "FAILED (Expected HRT_TERMINATED but not found)\n"
    end
end

define validate_main_test_03
    set $i = 0
    set $hrt_starts       = 0
    set $found_terminated = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 3
            set $hrt_starts = $hrt_starts + 1
        end
        if $event_id == 7
            set $found_terminated = 1
        end
        set $i = $i + 1
    end
    
    printf "Test 3 - An HRT exceeds its deadline and another HRT starts on the same tick: "
    if $hrt_starts >= 2 && $found_terminated == 1
        printf "PASSED\n"
    else
        printf "FAILED (Expected two HRT_START events, HRT_TERMINATED)\n"
    end
end

define validate_main_test_04
    set $i = 0
    set $found_srt_start     = 0
    set $found_srt_preempted = 0
    set $found_hrt_start     = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 4
            set $found_srt_start = 1
        end
        if $event_id == 8
            set $found_srt_preempted = 1
        end
        if $event_id == 3
            set $found_hrt_start = 1
        end
        set $i = $i + 1
    end

    printf "Test 4 - SRT preempted by HRT: "
    if $found_srt_start == 1 && $found_srt_preempted == 1 && $found_hrt_start == 1
        printf "PASSED\n"
    else
        printf "FAILED (SRT preemption not detected as expected)\n"
    end
end

define validate_main_test_05
    set $i = 0
    set $found_preempted = 0
    set $found_resumed   = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 8
            set $found_preempted = 1
        end
        if $event_id == 5
            set $found_resumed = 1
        end
        set $i = $i + 1
    end

    printf "Test 5 - SRT preempted by HRT then resumes: "
    if $found_preempted == 1 && $found_resumed == 1
        printf "PASSED\n"
    else
        printf "FAILED (SRT preemption or resume not detected)\n"
    end
end

define validate_main_test_06
    set $i = 0
    set $hrt_starts = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 3
            set $hrt_starts = $hrt_starts + 1
        end
        set $i = $i + 1
    end

    printf "Test 6 - Major frame reset, deterministic replay: "
    if $hrt_starts >= 3
        printf "PASSED\n"
    else
        printf "FAILED (Expected at least 3 HRT_START events)\n"
    end
end

define validate_main_test_07
    set $i = 0
    set $srt_in_gap = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 4 || $event_id == 5
            set $ts = xTraceLog[$i].ulTimestamp
            if $ts >= 300 && $ts < 600
                set $srt_in_gap = 1
            end
        end
        set $i = $i + 1
    end

    printf "Test 7 - SRT runs in gap between two HRT tasks: "
    if $srt_in_gap == 1
        printf "PASSED\n"
    else
        printf "FAILED (SRT did not run in the expected gap)\n"
    end
end

define validate_main_test_08
    set $i = 0
    set $found_preempted = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 8
            set $found_preempted = 1
        end
        set $i = $i + 1
    end

    printf "Test 8 - Single HRT task, SRT never preempted: "
    if $found_preempted == 0
        printf "PASSED\n"
    else
        printf "FAILED (SRT was preempted unexpectedly)\n"
    end
end

define validate_main_test_09
    set $i = 0
    set $srt_count = 0
    set $pass = 1

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 4
            if $srt_count % 2 == 0
                if ((TCB_t *)xTraceLog[$i].xTask)->pcTaskName[5] != 'C'
                    set $pass = 0
                end
            else
                if ((TCB_t *)xTraceLog[$i].xTask)->pcTaskName[5] != 'D'
                    set $pass = 0
                end
            end
            set $srt_count = $srt_count + 1
        end
        set $i = $i + 1
    end

    printf "Test 9 - Two SRT tasks run in order: "
    if $pass == 1 && $srt_count >= 2
        printf "PASSED\n"
    else
        printf "FAILED (Order incorrect or insufficient SRT starts)\n"
    end
end

define validate_main_test_10
    set $i = 0
    set $hrt_in_last_sf = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 3
            set $ts = xTraceLog[$i].ulTimestamp
            if $ts % 500 == 400
                set $hrt_in_last_sf = $hrt_in_last_sf + 1
            end
        end
        set $i = $i + 1
    end

    printf "Test 10 - HRT in last subframe replays each major frame: "
    if $hrt_in_last_sf >= 3
        printf "PASSED\n"
    else
        printf "FAILED (Expected HRT in last subframe across 3 frames)\n"
    end
end

define validate_main_test_11
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 11
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 11 - Passes a NULL pointer to the scheduler configuration: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_NULL_CONFIG)\n"
    end
end

define validate_main_test_12
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 12
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 12 - Sets an invalid Major Frame duration (0): "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_MAJOR_FRAME)\n"
    end
end

define validate_main_test_13
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 13
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 13 - Sets a different value for configRT_SUBFRAME_COUNT and uxSubframeCount in static allocation: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_WARNING_SUBFRAME_COUNT_MISMATCH)\n"
    end
end

define validate_main_test_14
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 14
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 14 - Sets an invalid Subframe Count (0): "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_SUBFRAME)\n"
    end
end

define validate_main_test_15
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 15
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 15 - Sets a different value for configRT_TASK_COUNT and uxTaskCount in static allocation: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_WARNING_TASK_COUNT_MISMATCH)\n"
    end
end

define validate_main_test_16
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 16
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 16 - Sets an invalid Task Count (0): "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_TASKCOUNT)\n"
    end
end

define validate_main_test_17
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 17
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 17 - Passes a NULL pointer to the task list configuration: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_NULL_TASKLIST)\n"
    end
end

define validate_main_test_18
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 18
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 18 - Major frame is not perfectly divisible by the number of subframes: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_FRAME_ALIGNMENT)\n"
    end
end

define validate_main_test_19
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 19
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 19 - A task is provided without a name (NULL string): "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_NULL_TASK_NAME)\n"
    end
end

define validate_main_test_20
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 20
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 20 - A task is provided without a function (NULL pointer): "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_NULL_TASK_FUNCTION)\n"
    end
end

define validate_main_test_21
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 21
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 21 - Task has invalid stack size , e.g. set to 0 ( less than configMINIMAL_STACK_SIZE ): "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_TASK_STACK_SIZE)\n"
    end
end

define validate_main_test_22
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 22
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 22 - Task type is neither HRT nor SRT ( e.g., IDLE ): "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_TASK_TYPE)\n"
    end
end

define validate_main_test_23
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 23
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 23 - Task is assigned to a subframe ID that exceeds the configured number of subframes: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_HRT_SUBFRAMEID)\n"
    end
end

define validate_main_test_24
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 24
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 24 - Task start time is greater than or equal to its deadline: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_HRT_INTERVAL)\n"
    end
end

define validate_main_test_25
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 25
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 25 - Task end time is greater than or equal than the major frame duration: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_HRT_EXCEEDS_MAJORFRAME_BOUND)\n"
    end
end

define validate_main_test_26
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 26
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 26 - Task start and/or end time exceeds the boundary of its assigned subframe: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_HRT_EXCEEDS_SUBFRAME_BOUND)\n"
    end
end


define validate_main_test_27
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 27
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 27 - Two HRT tasks are scheduled with overlapping execution windows in the same subframe: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_INVALID_HRT_OVERLAP)\n"
    end
end

define validate_main_test_28
    set $pass = 0

    if (pxTasks[0]->pcTaskName[5] == 'A' && \
        pxTasks[1]->pcTaskName[5] == 'B' && \
        pxTasks[2]->pcTaskName[5] == 'C' && \
        pxTasks[3]->pcTaskName[5] == 'D' && \
        pxTasks[4]->pcTaskName[5] == 'E' && \
        pxTasks[5]->pcTaskName[5] == 'F' && \
        pxTasks[6]->pcTaskName[5] == 'G')
        set $pass = 1
    end

    printf "Test 28 - Passes HRTs not ordered by start time: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (HRT tasks in pxTasks should be ordered by start time)\n"
    end
end

define validate_main_test_29
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 28
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 29 - vConfigureScheduler is not called: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vTaskStartScheduler should trigger EVENT_ERROR_NOT_INITIALIZED)\n"
    end
end

define validate_main_test_30
    set $i = 0
    set $pass = 0

    while $i < ulLogIndex
        set $event_id = xTraceLog[$i].ucEventID
        if $event_id == 10
            set $pass = 1
        end
        set $i = $i + 1
    end

    printf "Test 30 - vConfigureScheduler is called twice: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (vConfigureScheduler should trigger EVENT_ERROR_ALREADY_INITIALIZED)\n"
    end
end

define validate_main_test_31
    set $pass = 0
    set $i = 0
    set $expected_step = 0
    set $failed_sequence = 0

    while $i < ulLogIndex && $expected_step < 14 && $failed_sequence == 0
        set $entry = xTraceLog[$i]
        set $event_id = $entry.ucEventID
        
        if $event_id != 0
            
            set $expected_event = 7
            if $expected_step % 2 == 0
                set $expected_event = 3
            end
            
            set $expected_char = 'A' + ($expected_step / 2)
            set $task_char = ((TCB_t *)$entry.xTask)->pcTaskName[5]
            if $event_id == $expected_event && $task_char == $expected_char
                set $expected_step = $expected_step + 1
            else
                set $failed_sequence = 1
            end
        end
        
        set $i = $i + 1
    end

    if $expected_step == 14 && $failed_sequence == 0
        set $pass = 1
    end

    printf "Test 31 - Stress Test: each tick there is an HRT start time: "
    if $pass == 1
        printf "PASSED\n"
    else
        printf "FAILED (Each tick there should be a HRT deadline and a HRT start)\n"
    end
end

define validate_main_test_32
    printf "Test 32 - Overhead: details in logs/overhead_trace.log"
end