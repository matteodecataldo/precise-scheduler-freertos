set auto-load safe-path /

define show_logger
    set $i = 0

    # configTICK_RATE_HZ
    set $tick_hz = 1000

    printf "--- TIMELINE SCHEDULER TRACE START ---\n"
    printf "   Tick    |    Time (ms)   |   Event Type   |   Task Name   \n"      
    printf "-------------------------------------------------------------\n"

    while $i < ulLogIndex
        set $entry = xTraceLog[$i]

        set $timestamp = $entry.ulTimestamp
        set $event_id = $entry.ucEventID
        set $xtask = $entry.xTask

        set $ms = ($timestamp * 1000) / $tick_hz

        # Nome default
        set $name = "SYSTEM"

        if $xtask != 0 
            set $tcb = (TCB_t *)$xtask
            if $tcb->pcTaskName != 0
                set $name = (char *)$tcb->pcTaskName
            else
                set $name = "UNKNOWN"
            end
        end

        if $event_id == 1
            printf "[ %5d ]  |  [ %5d ms ]  |   -------MAJOR FRAME END-------\n\n", $timestamp, $ms
        end

        if $event_id == 29
            printf "[ %5d ]  |  [ %5d ms ]  |   MAJOR FRAME START \n", $timestamp, $ms
        end

        if $event_id == 2
            printf "[ %5d ]  |  [ %5d ms ]  |   SUBFRAME END\n\n", $timestamp, $ms
        end

        if $event_id == 3
            printf "[ %5d ]  |  [ %5d ms ]  |   HRT START:    %s\n", $timestamp, $ms, $name
        end

        if $event_id == 4
            printf "[ %5d ]  |  [ %5d ms ]  |   SRT START:    %s\n", $timestamp, $ms, $name
        end

        if $event_id == 5
            printf "[ %5d ]  |  [ %5d ms ]  |   SRT RESUME:   %s\n", $timestamp, $ms, $name
        end

        if $event_id == 6
            printf "[ %5d ]  |  [ %5d ms ]  |   NO TASK RUNNING ! IDLE TIME\n", $timestamp, $ms
        end

        if $event_id == 7
            printf "[ %5d ]  |  [ %5d ms ]  |   %s TERMINATED (Deadline Missed @ tick %d / %d ms)\n", $timestamp, $ms, $name, $timestamp, $ms
        end

        if $event_id == 8
            printf "[ %5d ]  |  [ %5d ms ]  |   %s PREEMPTED\n", $timestamp, $ms, $name
        end

        if $event_id == 9
            printf "[ %5d ]  |  [ %5d ms ]  |   TASK END:     %s\n", $timestamp, $ms, $name
        end
        
        # ---------------------------------------------------------
        # ERROR AND WARNING EVENTS (10 - 27)
        # ---------------------------------------------------------

        if $event_id == 10
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_ALREADY_INITIALIZED\n", $timestamp, $ms
        end

        if $event_id == 11
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_NULL_CONFIG\n", $timestamp, $ms
        end

        if $event_id == 12
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_MAJOR_FRAME\n", $timestamp, $ms
        end

        if $event_id == 13
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_WARNING_SUBFRAME_COUNT_MISMATCH\n", $timestamp, $ms
        end

        if $event_id == 14
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_SUBFRAME\n", $timestamp, $ms
        end

        if $event_id == 15
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_WARNING_TASK_COUNT_MISMATCH\n", $timestamp, $ms
        end

        if $event_id == 16
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_TASKCOUNT\n", $timestamp, $ms
        end

        if $event_id == 17
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_NULL_TASKLIST\n", $timestamp, $ms
        end

        if $event_id == 18
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_FRAME_ALIGNMENT\n", $timestamp, $ms
        end

        if $event_id == 19
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_NULL_TASK_NAME\n", $timestamp, $ms
        end

        if $event_id == 20
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_NULL_TASK_FUNCTION\n", $timestamp, $ms
        end

        if $event_id == 21
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_TASK_STACK_SIZE\n", $timestamp, $ms
        end

        if $event_id == 22
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_TASK_TYPE\n", $timestamp, $ms
        end

        if $event_id == 23
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_HRT_SUBFRAMEID\n", $timestamp, $ms
        end

        if $event_id == 24
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_HRT_INTERVAL\n", $timestamp, $ms
        end

        if $event_id == 25
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_HRT_EXCEEDS_MAJORFRAME_BOUND\n", $timestamp, $ms
        end

        if $event_id == 26
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_HRT_EXCEEDS_SUBFRAME_BOUND\n", $timestamp, $ms
        end

        if $event_id == 27
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_INVALID_HRT_OVERLAP\n", $timestamp, $ms
        end

        if $event_id == 28
            printf "[ %5d ]  |  [ %5d ms ]  | EVENT_ERROR_NOT_INITIALIZED\n", $timestamp, $ms
        end
        
        set $i = $i + 1
    end

    printf "--- TIMELINE SCHEDULER TRACE END ---\n"
end