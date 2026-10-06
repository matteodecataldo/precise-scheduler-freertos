
# Precise Timeline Scheduler for FreeRTOS
> **🎓 University Project - Politecnico di Torino**

The project was developed as part of the **Computer Architectures and Operating Systems** course at **Politecnico di Torino** by a team of four students. 

> **Team:** [Matteo De Cataldo](https://github.com/matteodecataldo) • [Taha Zied Elghazel](https://github.com/taha-zied-elghazel) • [Federico Capecelatro](https://github.com/FedericoCapecelatro) • [Sera Albayrak](https://github.com/SeraaAlbayrakk)

## About the project
A modified [FreeRTOS](https://www.freertos.org/) kernel that implements a deterministic, time-triggered scheduler (major/sub-frames) for hard and soft real-time tasks.
- Designed a new configuration API, O(1) data structures and scheduling logic
- Implemented a low-overhead tick-level tracing system and extensive validation and error hooks
- Built an automated QEMU/GDB test suite in Bash covering scheduling behavior, configuration errors, stress scenarios and scheduler's overhead
- Tech stack: C, Python, Bash | FreeRTOS, QEMU, GDB, Make, Git

# Documentation
> **Note:** This project is a work in progress. The scheduler is fully functional, but some stock FreeRTOS features may not work with it. Anything not described in this documentation has not yet been implemented or integrated.

## Table of Contents

* [Repository structure](#repository-structure)
* [Demo](#demo)
* [1. Project Overview](#1-project-overview)
* [2. Task Control Block](#2-task-control-block)
* [3. Global Scheduler Structures and State](#3-global-scheduler-structures-and-state)
* [4. `vConfigureScheduler()` — The New System Call](#4-vconfigurescheduler--the-new-system-call)
* [5. Static and Dynamic Allocation](#5-static-and-dynamic-allocation)
* [6. Task Creation](#6-task-creation)
* [7. Runtime Scheduling Core](#7-runtime-scheduling-core)
* [8. Hooks and Trace/Monitoring System](#8-hooks-and-the-tracemonitoring-system)
* [9. Automated Test Suite](#9-automated-test-suite)


## Repository structure
 
```
<repo root>/
├── Demo/                     # a small, working example built on the modified kernel
├── FreeRTOS/                 # the modified FreeRTOS kernel sources, including the tasks.c 
├── Tests/                    # the automated test suite and its runner
├── .gitignore
└── Readme.md
```

## Demo
 
This project modified the scheduler's behaviour, therefore it builds and runs the same way any FreeRTOS/QEMU demo does.
 
 
From inside `./Demo/`:
 
```bash
make clean             # remove previous build output
make all               # build the demo application (produces ./Output/demo.elf)
make qemu_start        # run the demo in QEMU
make qemu_debug        # OPTIONAL: run the demo in QEMU, halted, waiting for a debugger (GDB stub on :1234)
make gdb_start         # OPTIONAL: attach GDB to a running qemu_debug session: "target remote :1234"
```

A log (detailed here: [8. Hooks and Trace/Monitoring System](#8-hooks-and-the-tracemonitoring-system)
) of an example execution of the demo project can be found inside `./Demo/demo_example.log` file.

> The scheduler's optional instrumentation — the configuration-error hook system (`timeline_hook.c/.h`) and the trace/logging system (`logger.c/.h`, `logger.gdb`) — is **not required** for the RT scheduler to work. `configUSE_RT_HOOK` can be left at `0` and the scheduler behaves exactly the same. Both are included in `Demo/` only as a worked example of how to wire them up.

## 1. Project Overview
This project introduces a precise, timeline-based scheduler designed to replace the standard priority-based FreeRTOS scheduling model. By removing dynamic priority changes and adopting a strict time-triggered architecture, this modified kernel ensures predictable, deterministic, and repeatable real-time task execution. 

### Implementation Strategy: 
> Use of macro to switch from  stock FreeRTOS to custom RT scheduler

Because modifying a FreeRTOS kernel involves complex file dependencies, our solution preserves the integrity of the original FreeRTOS codebase. Rather than deleting standard FreeRTOS logic, all timeline-specific modifications are encapsulated using a custom configuration macro: `configUSE_RT_SCHEDULER`.
- When it is set to `0`, FreeRTOS behaves exactly like stock FreeRTOS (priority-based, preemptive scheduler). 
- When it is `1`, the kernel switches to the custom deterministic, timeline-driven scheduler.

This means the two schedulers are mutually exclusive at compile time, not at runtime — there is no code path that mixes priority-based scheduling and timeline scheduling in the same build.

**Scope of Kernel Modifications:**
Every change related to the timeline scheduler within the kernel is strictly isolated within conditional compilation blocks for example:

```c
#if ( configUSE_RT_SCHEDULER == 1 )
    /* Timeline-driven scheduler logic (Major/Sub-frames, HRT/SRT tasks) */
#else
    /* Standard FreeRTOS priority-based preemptive scheduler */
#endif
```

### Configuration macros
 
These are the new `FreeRTOSConfig.h` symbols this implementation introduces and depends on:
 
| Symbol | Purpose |
|---|---|
| `configUSE_RT_SCHEDULER` | Master switch. Selects the timeline scheduler implementation for every function below instead of the stock one. |
| `configUSE_RT_HOOK` | Enables `timeline_hook.h` inclusion and the error/trace hook calls (`vApplicationWrongConfigurationHook`, the `trace*` macros used by the timeline scheduler). |
| `configUSE_PENDING_TICKS` | Controls what happens if a tick interrupt fires while the scheduler is suspended during a timeline-critical instant (deadline, sub-frame boundary, major-frame boundary, HRT release). |
| `configRT_TASK_COUNT` | Required when `configSUPPORT_STATIC_ALLOCATION == 1`. Fixes the size of the internal task-pointer table at compile time (no heap use). |
| `configRT_SUBFRAME_COUNT` | Required when `configSUPPORT_STATIC_ALLOCATION == 1`. Fixes the size of the sub-frame table at compile time. |
| `configTRACE_INSTRUCTION_COUNT` (1 or 2) | Enables cycle-accurate instrumentation of the scheduling decision using the Cortex‑M SysTick/ICSR registers. |
 
---


## 2. Task Control Block
 
The layout, alignment-critical fields (`pxTopOfStack`, MPU settings, etc.) are untouched — only new fields are appended at the end so stack initialization code (which depends on the top-of-struct layout) keeps working unmodified.
 
**Added fields:**
 
```c
TaskType_t      uxTaskType;      // HRT, SRT or IDLE
TickType_t      xStartTime;      // release time inside the major frame (HRT only)
TickType_t      xEndTime;        // absolute deadline inside the major frame (HRT only)
UBaseType_t     uxSubframeId;    // which sub-frame this HRT task belongs to
BaseType_t      uxTaskState;     // task's state
TaskFunction_t  pxTaskCode;      // kept so the task can be re-entered from scratch at reset time
void *          pvParameters;    // kept for the same reason
```
Notes: 
- For SRT tasks, `xStartTime`, `xEndTime` and `uxSubframeId` are meaningless (SRT tasks have no fixed slot), therefore sanitization is applied and these fields are explicitly set to zeroes:
- In this implementation, SRT tasks are assumed
to have no fixed subframe; they run continuously
whenever no HRT task is active. If SRT tasks
need to be bound to specific subframes, further
modifications to the kernel are needed.

 
## 3. Global Scheduler Structures and State
 
The scheduler discards the traditional FreeRTOS `lists.c` implementation. Instead, task execution is governed by static arrays and structs, so that task selection at runtime is an **O(1) array lookup**, never a list search — this is what keeps the tick handler's overhead low and bounded, in line with the "≤10% CPU overhead" non-functional requirement.
 
There are five logical groups of state, described one at a time below: the task pool, SRT tracking, HRT/sub-frame tracking, timeline bookkeeping, and a small "current position" cache.
 
### 3.1 Task pool — `pxTasks[]`
 
```c
PRIVILEGED_DATA static TCB_t **pxTasks;
PRIVILEGED_DATA static UBaseType_t uxTaskCount;
```
 
| Symbol | Purpose |
|---|---|
| `pxTasks` | Single contiguous table holding **every** task's TCB pointer. `vConfigureScheduler()` fills it in schedule order: all HRT tasks first (grouped by sub-frame, ordered by start time), followed by all SRT tasks. |
| `uxTaskCount` | Total number of tasks registered through `vConfigureScheduler()` (HRT + SRT). |

 
> **Static vs. dynamic build:** with `configSUPPORT_STATIC_ALLOCATION == 1`, `pxTasks` is a fixed-size array `TCB_t *pxTasks[configRT_TASK_COUNT]`; with dynamic allocation it is a `TCB_t **` pointer, `pvPortMalloc()`'d by `vConfigureScheduler()` once the real task count is known.
 
### 3.2 SRT task tracking
 
```c
PRIVILEGED_DATA static TCB_t **pxSrtTasks;
PRIVILEGED_DATA static UBaseType_t uxSrtTaskCount;
PRIVILEGED_DATA static volatile UBaseType_t uxNextSrtIndex;
```
 
| Symbol | Purpose |
|---|---|
| `pxSrtTasks` | Pointer to the first SRT task inside `pxTasks[]` — the contiguous tail slice reserved for soft real-time tasks. |
| `uxSrtTaskCount` | How many SRT tasks exist in total. |
| `uxNextSrtIndex` | Index (into `pxSrtTasks`) of the next SRT task to dispatch when the CPU has idle time available. |
 
### 3.3 HRT task tracking — `TimelineSubframe_t`
 
```c
typedef struct
{
    TCB_t **pxHrtTasks;
    UBaseType_t uxHrtTaskCount;
    TickType_t xSubframeEndTick;
} TimelineSubframe_t;
```
 
| Field | Purpose |
|---|---|
| `pxHrtTasks` | Pointer to the first HRT task of *this* sub-frame inside `pxTasks[]` — a contiguous slice, not a separate array. |
| `uxHrtTaskCount` | How many HRT tasks are assigned to this sub-frame. |
| `xSubframeEndTick` | Absolute tick (within the major frame) at which this sub-frame ends; pre-computed once by `vConfigureScheduler()`. |
 
```c
PRIVILEGED_DATA static TimelineSubframe_t pxSubframes[configRT_SUBFRAME_COUNT]; // or malloc'd
PRIVILEGED_DATA static UBaseType_t uxSubframeCount;
PRIVILEGED_DATA static volatile UBaseType_t uxNextHrtIndex;
```
 
| Symbol | Purpose |
|---|---|
| `pxSubframes` | Array of `TimelineSubframe_t`, one entry per sub-frame, indexed `0 … uxSubframeCount-1`. Static or `pvPortMalloc()`'d, same rule as `pxTasks`. |
| `uxSubframeCount` | Number of sub-frames in the major frame. |
| `uxNextHrtIndex` | *Within the current sub-frame*: index of the next HRT task in `pxCurrentSubframe->pxHrtTasks` expected to be released. |
 
### 3.4 Timeline bookkeeping
 
```c
PRIVILEGED_DATA static volatile TickType_t  xMajorFrameEndTick;
PRIVILEGED_DATA static volatile UBaseType_t uxHrtState;
PRIVILEGED_DATA static volatile UBaseType_t uxSchedulerState;
PRIVILEGED_DATA static volatile TickType_t  xTimelineTickCount;
```
 
| Symbol | Purpose |
|---|---|
| `xMajorFrameEndTick` | Length of the major frame, in ticks (e.g. `100` for a 100 ms frame at 1 ms tick). |
| `uxHrtState` | `tsNO_HRT_READY` / `tsHRT_READY` — set by `xTaskIncrementTick()` the instant an HRT task's `xStartTime` is reached, consumed by `vTaskSwitchContext()` on the very next dispatch. |
| `uxSchedulerState` | The scheduler's own lifecycle state machine — see the diagram below. |
| `xTimelineTickCount` | Ticks elapsed **since the start of the current major frame** (`0 … xMajorFrameEndTick-1`), independent from the free-running `xTickCount`. Exposed to the application via the new `xTaskGetTimelineTickCount()` accessor. |
 
### 3.5 Current-position cache
 
```c
PRIVILEGED_DATA static volatile TimelineSubframe_t *pxCurrentSubframe;
PRIVILEGED_DATA static volatile UBaseType_t uxCurrentSubframeIndex;
PRIVILEGED_DATA static volatile TickType_t  uxCurrentSubframeEndTick;
```
 
| Symbol | Purpose |
|---|---|
| `pxCurrentSubframe` | Pointer to the `TimelineSubframe_t` the timeline is currently inside — cached instead of recomputed, so the tick ISR never has to search `pxSubframes[]`. |
| `uxCurrentSubframeIndex` | Index of `pxCurrentSubframe` inside `pxSubframes[]`; advanced by one at every sub-frame boundary, rewound to `0` at every major-frame boundary. |
| `uxCurrentSubframeEndTick` | Copy of `pxCurrentSubframe->xSubframeEndTick`, kept alongside the pointer purely to avoid an extra dereference in the tick ISR's hot path. |
 
**Scheduler state machine (`uxSchedulerState`)**
 
```
tsSTATE_UNINITIALIZED  --vConfigureScheduler()-->   tsSTATE_READY
tsSTATE_READY          --vTaskStartScheduler()-->   tsSTATE_RUNNING
tsSTATE_RUNNING        --major frame boundary-->    tsSTATE_RESETTING
tsSTATE_RESETTING      --vMajorFrameReset() done-->  tsSTATE_RUNNING
```
 
`vConfigureScheduler()` can only be called once (`tsSTATE_UNINITIALIZED`), and `vTaskStartScheduler()` refuses to run if the configuration step wasn't done — this is checked with `configASSERT()` **and** reported through `vApplicationWrongConfigurationHook(RT_ERROR_WRONG_CONFIGURATION)`.
 
### 3.6 Why `pxHrtTasks` and `pxSrtTasks` are just pointers into `pxTasks[]`
 
`vConfigureScheduler()` first **sorts** the incoming task array so that:
1. all SRT tasks end up at the tail,
2. all HRT tasks are grouped by `uxSubframeId` and ordered by `xStartTime` within each sub-frame.
Because of this sort, every sub-frame's HRT tasks and the whole SRT set become **contiguous runs** inside `pxTasks[]`. `pxSubframes[id].pxHrtTasks` and `pxSrtTasks` are therefore set to point at the first element of their respective run — no extra array or extra allocation is needed to index into them at runtime.
 

## 4. `vConfigureScheduler()` — The New System Call
`BaseType_t vConfigureScheduler(TimelineConfig_t *cfg);`

This new system call creates the required FreeRTOS data structures and initializes the timeline environment. It requires a `TimelineConfig_t` struct as a configuration parameter, which contains the following fields:

```c
typedef struct {
    TickType_t xMajorFrameTicks;
    UBaseType_t uxSubframeCount;
    UBaseType_t uxTaskCount;
    TimelineTaskConfig_t *tasks;
} TimelineConfig_t;
```
Each task is described by a `TimelineTaskConfig_t` struct:
```c
typedef struct {
        const char* task_name;
        TaskFunction_t function;
        configSTACK_DEPTH_TYPE uxStackDepth;
        TaskType_t uxTaskType; 
        TickType_t xStartTime;
        TickType_t xEndTime;
        UBaseType_t uxSubframeId;
        void * pvParameters;
} TimelineTaskConfig_t;
```
 
The function runs in five stages:

### 4.1 Validation
Every field and possible misconfiguration is checked before any memory is allocated or created. Each check has a dedicated trace and hook with error code that can be activated and disabled with `configUSE_RT_HOOK`.

Note: If `configSUPPORT_STATIC_ALLOCATION == 1`, vConfigureScheduler uses the values inside `configRT_TASK_COUNT` and `configRT_SUBFRAME_COUNT` to initialize the scheduler, ignoring the values passed in the `cfg` struct. If the values from the macro and the cfg don't match it is considered a configuration error, in order to help the user configure the system correctly.

### 4.2 Ordering + overlap detection
To ensure `O(1)` access to the TCBs during scheduling, a temporary array is used to sort the tasks. HRTs are ordered by `uxSubframeId` and `xStartTime`, while SRTs are placed at the end, maintaining the same relative order as they had inside `cfg`.

 **Immediate overlap check**  During sorting, the system compares each new HRT task to the previous one to ensure no overlap. If a new HRT’s `xStartTime` is earlier than the previous HRT’s `xEndTime`, it is flagged as a configuration error. This prevents potential task termination, even if the first HRT finishes early in practice.
 
### 4.3 Initialization and Allocation
Depending on whether static or dynamic allocation is selected, structures are initialized using either `cfg->uxTaskCount`/`cfg->uxSubframeCount` or `configRT_TASK_COUNT` / `configRT_SUBFRAME_COUNT`.
The allocated structures are:
- `TCB_t **pxTasks`: A memory pool for all TCBs, ordered as described in section 4.2.
- `TimelineSubframe_t *pxSubframes`: A structure that maps subframe positions inside `pxTasks` to their corresponding HRT pointers.
- `TCB_t **pxSrtTasks`: A pointer to the region inside `pxTasks` dedicated to SRTs.
### 4.4 Task creation
For every task, in its sorted order, either `xTaskCreateStatic()` or `xTaskCreate()` is called, and the resulting handle is stored in `pxTasks[i]`. Additionally, 
- HRT tasks increment `pxSubframes[subframeId].uxHrtTaskCount` and record the start pointer of their slice inside `pxSubframes[subframeId].pxHrtTasks`. 
- SRT tasks increment `uxSrtTaskCount` and record the `pxSrtTasks` slice start.

Note: If static allocation is selected, `vApplicationGetTaskMemory` is called to retrieve the task stack memory. Just like in standard FreeRTOS, if `configKERNEL_PROVIDED_STATIC_MEMORY` is set to 1, a kernel-implemented version of `vApplicationGetTaskMemory` is used; otherwise, the user must provide one.
 
### 4.5 Finalization
If all steps succeed, the kernel transitions to the next state: `uxSchedulerState = tsSTATE_READY`;. The system is now ready for `vTaskStartScheduler()`.

Note: If any step involving initialization or allocation fails (such as a malloc failure or task creation failure), `prvCleanupSchedulerOnError()` is called. This function loops through `pxTasks[]`, deletes all already-created TCBs, frees any dynamic buffers, and resets all global variables and pointers to their initial values. This ensures that a failed `vConfigureScheduler()` call never leaves the kernel in a partially initialized state.


## 5. Static and Dynamic Allocation 
Both static and dynamic allocation are supported, if both of them are selected, the static allocation will be used

Both allocation modes are supported. 

- **Static** (`configSUPPORT_STATIC_ALLOCATION == 1`): task/stack memory comes from `vApplicationGetTaskMemory()` (a kernel-provided implementation is included, gated by `configKERNEL_PROVIDED_STATIC_MEMORY`); `vConfigureScheduler()` requires the caller's `TimelineConfig_t` counts to match the compile-time constants exactly, otherwise a compile error will be displayed.
- **Dynamic** (`configSUPPORT_DYNAMIC_ALLOCATION == 1`).

Both can be enabled together (but only the static allocation will be used) and  at least one **must** be enabled (compile-time `#error` otherwise).

---

## 6. Task Creation

`xTaskCreate()` / `xTaskCreateStatic()` keep their original names and behaviours but their signature is changed: the `UBaseType_t uxPriority` parameter is replaced by the four timeline parameters. The same applies for their private helpers `prvCreateTask`, `prvCreateStaticTask` and `prvInitialiseNewTask`.  For example `xTaskCreate()`:

```c
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                         const char * const pcName,
                         const configSTACK_DEPTH_TYPE uxStackDepth,
                         void * const pvParameters,
             // removed: UBaseType_t uxPriority
                         TaskType_t uxTaskType,     // new
                         TickType_t xStartTime,     // new
                         TickType_t xEndTime,       // new
                         UBaseType_t uxSubframeId,  // new
                         TaskHandle_t * const pxCreatedTask );
```

**`prvRegisterTaskInSystem()` replaces `prvAddNewTaskToReadyList()`.** A newly-created task is only counted, tagged for tracing, and handed to `portSETUP_TCB()`:

```c
static void prvRegisterTaskInSystem( TCB_t * pxNewTCB )
{
    taskENTER_CRITICAL();
    {
        uxCurrentNumberOfTasks++;
        uxTaskNumber++;
        #if ( configUSE_TRACE_FACILITY == 1 )
            pxNewTCB->uxTCBNumber = uxTaskNumber;
        #endif
        traceTASK_CREATE( pxNewTCB );
        portSETUP_TCB( pxNewTCB );
    }
    taskEXIT_CRITICAL();
}
```

## 7. Runtime Scheduling Core

### 7.1 `vTaskStartScheduler()`

The behavior is identical to stock FreeRTOS, with the following additions:
- **Initialization Check**: A check has been added to ensure the system was properly initialized using `vConfigureScheduler()`.
- **First Task Selection**: The scheduler selects the very first task to run by directly inspecting the timeline arrays.

### 7.2 `xTaskIncrementTick()` & `configUSE_PENDING_TICKS`

#### 7.2.1 `configUSE_PENDING_TICKS`

This compile-time macro determines how the system handles a tick interrupt if it occurs while the scheduler is suspended (which typically happens when using `malloc` operations). 

**Best Practice:** It is highly recommended to avoid using `malloc` inside task code so the scheduler never needs to suspend. However, if it is unavoidable, this macro gives you two ways to handle the behavior.

Whenever `xTaskIncrementTick()` needs to update a timeline-critical state (such as HRT deadlines, major/sub-frame boundaries, or HRT releases), it checks if the scheduler is suspended and reacts based on one of two mutually exclusive modes:

*   **`configUSE_PENDING_TICKS == 0` (Strict Mode):** 
    Pending ticks are disabled. If a tick occurs while the scheduler is suspended (e.g., inside a `vTaskSuspendAll()` / `xTaskResumeAll()` block), the system intentionally halts (`configAssert()` triggers `portDISABLE_INTERRUPTS(); for(;;);`). 
    *   *Result:* Hard determinism. Instead of letting the timeline silently drift, any suspension violation becomes a fatal, instantly visible error. 

*   **`configUSE_PENDING_TICKS == 1` (Tolerant Mode):** 
    This uses standard FreeRTOS behavior. If a tick occurs while suspended, it does not halt the system. Instead, the tick is queued (`xPendedTicks += 1`) and replayed in order as soon as the scheduler resumes. 
    *   *Result:* Robustness. The timeline stays consistent and no ticks are lost, but task activations will be delayed by the length of the suspension, introducing a small, bounded amount of jitter.

**In short:** Mode 0 buys hard determinism by treating any scheduler-suspension violation as a fatal error. Mode 1 buys robustness at the cost of a small, bounded amount of jitter. Which one you use is a project configuration choice.

#### 7.2.2 `xTaskIncrementTick()`
This is the function called from the SysTick ISR on every tick; it decides whether a context switch (`vTaskSwitchContext()`) is needed. Instead of the stock logic, the RT version runs four independent, ordered checks against the *next* tick value (`xNextTimelineTickCount = xTimelineTickCount + 1`):

1. **HRT deadline check** — if the currently running task is HRT and the next tick reaches its `xEndTime`, it is forcefully terminated: `uxTaskState = tsTASK_STATE_TERMINATED`, and a switch is requested.
2. **Major-frame boundary check** — if the next tick reaches `xMajorFrameEndTick`, the whole timeline wraps: tick counter resets to 0, scheduler state is staged to `tsSTATE_RESETTING`, all HRT/SRT indices and the sub-frame cursor are rewound to the start, and a switch is requested. The actual task reset work is deferred to `vTaskSwitchContext()`.
3. **Sub-frame boundary check** (only evaluated if the major frame didn't just end) — advances `uxCurrentSubframeIndex`/`pxCurrentSubframe`/`uxCurrentSubframeEndTick`, and resets the per-sub-frame HRT cursor.
4. **HRT release check** — if the next HRT task in the (now current) sub-frame has `xStartTime` equal to the next tick, it is *released*: `uxHrtState = tsHRT_READY` and a switch is requested. If the task currently on the CPU is an SRT task, it is marked `uxTaskState = tsTASK_STATE_PREEMPTED` so it can resume later without losing its progress.


`vApplicationTickHook()` is still called unconditionally at the end, exactly like stock FreeRTOS.

### 7.3 `vTaskSwitchContext()`

Called whenever `xTaskIncrementTick()` (or a voluntary yield) requests a switch. After the standard bookkeeping (stack-overflow check, runtime-stats accounting, errno save/restore — all unchanged from stock FreeRTOS), the RT branch:

1. If the scheduler state is `tsSTATE_RESETTING` (major frame just wrapped), calls `vMajorFrameReset()` and moves the state to `tsSTATE_RUNNING` .
2. If `uxHrtState == tsHRT_READY` (an HRT task was released this tick): dispatches it directly — `pxCurrentSubframe->pxHrtTasks[uxNextHrtIndex]` — marks it `uxTaskState = tsTASK_STATE_DISPATCHED`, advances `uxNextHrtIndex`, and clears `uxHrtState`.
3. Otherwise, picks the next SRT task in round-robin order (`pxSrtTasks[uxNextSrtIndex]`), or the idle task if there are no SRT tasks at all.

#### 7.3.1 `uxTaskState` field
The `uxTaskState` field is the small state machine that lets an SRT task be interrupted and resumed:

| Value | Meaning |
|---|---|
| `tsTASK_STATE_RESET` | Task has not run yet in this major frame, or was just reset (via `vTaskReset()` / `vMajorFrameReset()`) — ready to be entered from the top. |
| `tsTASK_STATE_TERMINATED` | An HRT task was forcibly killed because it reached its deadline mid-frame, before the next reset sweep. Only ever set on HRT tasks. |
| `tsTASK_STATE_DISPATCHED` | Task is currently dispatched, or was dispatched and ran to completion without being interrupted by an HRT release. |
| `tsTASK_STATE_PREEMPTED` | Task was preempted mid-execution by an HRT release; its (real, saved) CPU context is still valid and must be resumed, **not** reset. |

An SRT task's `uxTaskState` only ever takes `tsTASK_STATE_RESET`, `tsTASK_STATE_DISPATCHED`, or `tsTASK_STATE_PREEMPTED` — `tsTASK_STATE_TERMINATED` is exclusive to HRT tasks and never appears on an SRT TCB.

When an SRT task is selected again:

```c
if (pxSrtTasks[uxNextSrtIndex]->uxTaskState == tsTASK_STATE_DISPATCHED)
{
    vTaskReset(pxSrtTasks[uxNextSrtIndex]);   // it ran to completion last time -> restart the function from scratch
}
pxCurrentTCB = pxSrtTasks[uxNextSrtIndex];
pxCurrentTCB->uxTaskState = tsTASK_STATE_DISPATCHED;
```

If the task was preempted (`uxTaskState == tsTASK_STATE_PREEMPTED`), the reset is skipped and its saved stack context (set up by the port's context-switch mechanism) is resumed exactly where it left off. If it had returned/completed normally (`uxTaskState == tsTASK_STATE_DISPATCHED`, unchanged since it was last dispatched, because nothing preempted it), it is reset before being dispatched again — required because SRT tasks are single-shot functions per the task model ("run from start to end and then terminate"), so re-running one means re-entering its function from the top.

### 7.4 Periodic repetition: `vMajorFrameReset()` 

`vMajorFrameReset()` loops through `pxTasks` and calls `vTaskReset(pxTasks[i])` for each task.

This runs for **every** task (HRT and SRT alike) exactly once per major frame, guaranteeing that every frame starts from the exact same, repeatable state.

### 7.5 `vTaskReset()`

`vTaskReset()` is a new function that **reset**, not destroy/recreate, to avoid heap churn and keep timing deterministic:

1. recomputes the top-of-stack address from the original stack buffer (same alignment logic as task creation),
2. calls `pxPortInitialiseStack()` again with the **cached** `pxTaskCode` / `pvParameters` from the TCB — this is precisely why those two fields were added to the TCB,
3. sets `uxTaskState = tsTASK_STATE_RESET`,
4. clears task-notification state, if enabled.

### 7.6 `vTimelineTaskComplete()` — voluntary early yield

```c
void vTimelineTaskComplete( void )
{
    portYIELD_WITHIN_API();
}
```

A thin wrapper task code can call to voluntarily give up the CPU immediately, letting the scheduler move on to the next task in the timeline as soon as useful work is done.It should be called at the end of every task
function body instead of letting the function
return normally. 
Therefore an example of task code is:

```c
void vTaskFunction(void *pvParameters){
    UART_printf("Hello by Task\n");
    vTimelineTaskComplete();
}
```

### 7.7 `xTaskResumeAll()` — simplified suspend/resume

Because there are no priority ready-lists or a pending-ready list to reconcile in this model, the RT version of `xTaskResumeAll()` is reduced to: decrement `uxSchedulerSuspended`, and if it reaches zero, replay any ticks that were pended while suspended (`xTaskIncrementTick()` in a loop, guarded by `configUSE_PENDING_TICKS`), then yield if a switch was requested during that replay. `vTaskSuspendAll()` itself is **not** modified — suspending the scheduler works exactly as in stock FreeRTOS; only what happens on resume differs.


## 8. Hooks and the Trace/Monitoring System

### 8.1 Error/configuration hook

A dedicated hook, gated by `configUSE_RT_HOOK` (which also pulls in `timeline_hook.h`), reports every configuration-time error from `vConfigureScheduler()` and `vTaskStartScheduler()`.


This follows the FreeRTOS hook convention. Every failure path in `vConfigureScheduler()` calls both a `trace*` macro (for the log/trace system) **and** `vApplicationWrongConfigurationHook()` (for the application to react, e.g. blink an LED / assert / log to console) with one of these codes:

`RT_ERROR_WRONG_CONFIGURATION`, `RT_ERROR_ALREADY_INITIALIZED`, `RT_ERROR_NULL_CONFIG`, `RT_ERROR_INVALID_MAJOR_FRAME`, `RT_ERROR_INVALID_SUBFRAME`, `RT_ERROR_INVALID_TASKCOUNT`, `RT_ERROR_NULL_TASKLIST`, `RT_ERROR_FRAME_ALIGNMENT`, `RT_ERROR_NULL_TASK_NAME`, `RT_ERROR_NULL_TASK_FUNCTION`, `RT_ERROR_INVALID_TASK_STACK_SIZE`, `RT_ERROR_INVALID_TASK_TYPE`, `RT_ERROR_INVALID_HRT_SUBFRAMEID`, `RT_ERROR_INVALID_HRT_INTERVAL`, `RT_ERROR_INVALID_HRT_EXCEEDS_MAJORFRAME_BOUND`, `RT_ERROR_INVALID_HRT_EXCEEDS_SUBFRAME_BOUND`, `RT_ERROR_INVALID_HRT_OVERLAP`, `RT_ERROR_TASK_CREATION_FAILED`, plus two non-fatal warnings `RT_WARNING_SUBFRAME_COUNT_MISMATCH` / `RT_WARNING_TASK_COUNT_MISMATCH` (static-allocation size mismatch between `FreeRTOSConfig.h` and the runtime `TimelineConfig_t`).

> The concrete numeric values / enum and the body of `vApplicationWrongConfigurationHook()` live in `timeline_hook.h`, which is not part of `tasks.c` — implement it in the application to decide what happens on each error (log, assert, safe-state, etc.).
> 
### 8.2 Trace macros (tick-level monitoring)

 
| Macro | Called when |
|---|---|
| `traceHRT_START(tick, tcb)` | An HRT task is dispatched (first task at boot, or via HRT release). |
| `traceHRT_TERMINATED(tick, tcb)` | An HRT task is forcefully killed because it reached its deadline. |
| `traceSRT_START_RESUME(tick, tcb, oldCompletedState)` | An SRT task is dispatched; the third argument tells whether it's a fresh start/reset (`0`) or a resume after preemption (`2`). |
| `traceIDLE_START(tick, tcb)` | The idle task is dispatched (no HRT/SRT work pending). |
| `traceMAJOR_FRAME_START(tick)` / `traceMAJOR_FRAME_END(tick)` | Beginning/end of a major-frame cycle. |
| `traceSUBFRAME_END(tick)` | A sub-frame boundary was crossed. |
| `traceTASK_SWITCH_OUT(tick, tcb, completedState, taskType)` | Right before a new task is selected — logs *why* the outgoing task left the CPU (`uxTaskState == tsTASK_STATE_TERMINATED` → forcibly terminated / frame reset, `== tsTASK_STATE_DISPATCHED`/`tsTASK_STATE_PREEMPTED` → normal or preempted handover). |
 

 
**How the log is captured.** None of this is printed live. `logger.c` holds an array of log-event records in target memory; each `trace*` macro is just a call into `logger.c` that writes one record — tick, event type, and the relevant task info — into the next free slot of that array. This write is deliberately kept cheap and O(1): no string formatting, no UART output, no blocking call. Since these macros fire from inside the tick ISR and the context-switch path — exactly the code that has to stay deterministic — anything slower would eat into the scheduler's own timing budget and add jitter to the very thing being measured. Formatting and printing are pushed entirely off the target and off the critical path.
 
**How to view it.** The array only becomes readable text later, offline, through `logger.gdb`. Attach GDB to a running or debug-halted target, interrupt the scheduler, and call the logging command defined in `logger.gdb` (`show_logger`); it walks the array in target memory and prints it in the readable, tick-by-tick form. 

### 8.3 Cycle-accurate instruction/overhead counting (`configTRACE_INSTRUCTION_COUNT`)

When set to `1` or `2`, a small block of Cortex‑M debug registers is used to timestamp scheduling decisions at cycle resolution (independent of, and finer-grained than, the tick period):

```c
#define dbg_SYSTICK_LOAD_REG    (*((volatile uint32_t *)0xE000E014))
#define dbg_SYSTICK_VAL_REG     (*((volatile uint32_t *)0xE000E018))
#define dbg_SCB_ICSR_REG        (*((volatile uint32_t *)0xE000ED04))
#define dbg_SCB_ICSR_PENDSTSET  (1 << 26)
uint32_t prvGetInstructionTimestamp( void );
```

`prvGetInstructionTimestamp()` reads the SysTick `VAL`/`LOAD` registers, corrects for a pending-but-not-yet-serviced SysTick exception (checked via `ICSR.PENDSTSET`), and returns an absolute cycle count. `xTaskIncrementTick()` uses this to timestamp the moment the scheduling policy started running (mode `1`/`2`) and, in mode `2` only, defers writing the "switch decided" timestamp until it's known a switch actually happens — avoiding polluting the log with a timestamp pair for ticks that didn't need one. This measures the scheduler's own overhead, directly supporting the "Overhead ≤ 10% CPU" non-functional requirement and the automated regression suite's timing-consistency checks.


## 9. Automated Test Suite
 
### 9.1 Project layout
 
The whole testing setup lives in one folder `Tests/`, shown below. 

This folder is a copy of the demo folder with some small modification and the addition of the test runner `run_tests.sh` and the two GDB scripts it needs to work: `logger.gdb` and `validator.gdb`. 

The folder looks like this:
 
```
Tests/
├── logs/                  # created/cleared by run_tests.sh: global, validation and overhead trace logs
├── Output/                # build output — run_tests.sh loads ./Output/demo.elf into gdb-multiarch
├── tests/                 # one main_test_N.c per test case (optional related ./config/FreeRTOSConfig_main_test_N.h)
├
├── FreeRTOSConfig.h       # default kernel config
├── logger.c / logger.h    # C-side implementation of the tick-level trace/monitoring buffer
├── uart.c / uart.h        # UART driver for console output on the target
├
├── run_tests.sh           # the test-runner script documented below
├── logger.gdb             # GDB script sourced by run_tests.sh; defines show_logger (dumps the trace log)
├── validator.gdb          # GDB script sourced by run_tests.sh; defines each validate_main_test_N check
├── overhead.py            # GDB Python script; defines show_instruction_overhead / show_tick_overhead
├
├── Makefile               # build rules; exposes the `all`, `clean`, `qemu_debug` targets used by the runner
├── mps2_m3.ld             # linker script for the QEMU MPS2 / Cortex-M3 target
└── startup.c              # Cortex-M3 startup code / vector table / reset handler for the emulated board

```
 
### 9.2 What the script does
 
`run_tests.sh` automates the "Production and Regression Testing" pipeline.
 
Each test is one `testname.c` file inside `./tests/`. Each file defines its own task set and scheduler configuration.
 
By default, every test is built with the default `./FreeRTOSConfig.h`. If a test needs a different configuration, you can add a file named `FreeRTOSConfig_testname.h` inside `./tests/config/`, and the build will use that one instead.
 
For each test file, the script:
1. Runs `make clean && make all MAIN_FILE=<testname>.c` to build it.
2. Starts `make qemu_debug` in the background.
3. Builds a temporary `.temp_script.gdb` script. This script reads the test's `ulTestTicksToRun` variable inside `testname.c` and stops the scheduler at that exact tick.
4. Sources `logger.gdb` and calls `show_logger` to save the tick-level trace (task starts/ends, deadline misses, idle time) into `logs/global_tests_trace.log`.
5. Sources `validator.gdb` to run the test's `validate_testname` check and save a PASS/FAIL summary into `logs/validation_tests_trace.log`.
6. Optionally can source `overhead.py` to log cycle-level scheduling overhead into `logs/overhead_trace.log`, covering the "≤10% CPU overhead" requirement. 
A `trap`-based cleanup makes sure QEMU and GDB are always closed, and the temporary GDB script always deleted — even if the run is interrupted (Ctrl-C) or a test fails to compile. This way, no emulator process is ever left running.
 
**Provided test coverage:**
- `main_test_01.c` – `main_test_10.c`: scheduler tests, such as task termination and preemption.
- `main_test_11.c` – `main_test_30.c`: configuration tests, covering everything that happens before `vTaskStartScheduler()` runs.
- `main_test_31.c`: a stress test.
- `main_test_32.c`: the overhead test.

### 9.3 Usage
 
```bash
./run_tests.sh                                # run every test found in ./tests/
./run_tests.sh main_test_3.c                  # run only one test
./run_tests.sh main_test_3.c main_test_7.c    # run only the named tests
```
 
Results land in `./logs/`: 
- `global_tests_trace.log` holds the full scheduling trace for every test run, 
- `validation_tests_trace.log` holds the per-test PASS/FAIL summaries,
- `overhead_trace.log` holds the timing/overhead measurement produced by `main_test_32.c`.

An example of a run of `./run_tests.sh` can be found in these logs files.