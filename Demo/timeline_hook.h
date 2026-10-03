#ifndef INC_TIMELINE_HOOK_H
#define INC_TIMELINE_HOOK_H
#endif

#if (configUSE_RT_SCHEDULER == 1)

#if (configUSE_RT_HOOK == 1)

#define RT_ERROR_ALREADY_INITIALIZED                        0
#define RT_ERROR_NULL_CONFIG                                1
#define RT_ERROR_INVALID_MAJOR_FRAME                        2
#define RT_ERROR_INVALID_SUBFRAME                           3
#define RT_ERROR_INVALID_TASKCOUNT                          4
#define RT_ERROR_NULL_TASKLIST                              5
#define RT_ERROR_FRAME_ALIGNMENT                            6
#define RT_ERROR_NULL_TASK_NAME                             7
#define RT_ERROR_NULL_TASK_FUNCTION                         8
#define RT_ERROR_INVALID_TASK_TYPE                          9
#define RT_ERROR_INVALID_HRT_SUBFRAMEID                     10
#define RT_ERROR_INVALID_HRT_INTERVAL                       11
#define RT_ERROR_INVALID_HRT_EXCEEDS_MAJORFRAME_BOUND       12
#define RT_ERROR_INVALID_HRT_EXCEEDS_SUBFRAME_BOUND         13
#define RT_ERROR_INVALID_HRT_OVERLAP                        14
#define RT_ERROR_TASK_CREATION_FAILED                       15
#define RT_ERROR_WRONG_CONFIGURATION                        16
#define RT_WARNING_SUBFRAME_COUNT_MISMATCH                  17
#define RT_WARNING_TASK_COUNT_MISMATCH                      18
#define RT_ERROR_INVALID_TASK_STACK_SIZE                    19

void vApplicationWrongConfigurationHook( UBaseType_t errCode );

#endif /* configUSE_RT_HOOK */

#if (configUSE_MALLOC_FAILED_HOOK == 1)
void vApplicationMallocFailedHook( void );
#endif /* configUSE_MALLOC_FAILED_HOOK */

#endif /* #if (configUSE_RT_SCHEDULER == 1) */

