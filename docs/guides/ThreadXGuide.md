# ThreadX Real-Time Operating System
-------------------------------------

# What is ThreadX ?

ThreadX is a **real-time operating System** used for embedded applications where deterministic behavior, low latency, and minimal memory usage are critical.

Below, I will give you some characteristics that make ThreadX the go-to.
  - **Certifiable:** Supports safety-critical standards.
  - **Tiny footprint:** Under 10 KB for full kernel.
  - **commercial-grade reliability:** Used in long-lived industrial and aerospace systems.

# Key Features
  - **Premptive, Priority-based scheduling**.

      ThreadX always runs the highest-priority ready thread. Lower-priority tasks are preempted immediately, ensuring a deterministic response to real-time events.
  
      And now you ask me, "How do I make a thread a high priority?"
    
      So, my young Padawan, to make a thread be High priority, you simply assign it a lower priority number when creating a thread(0 being the highest priority number).
    
    Below are the ThreadX Priority rules.
    
      Priority numbers range from 0 to 31.
    
      0 = Highest Priority.
    
      31 = Lowest Priority.
         
  - **Deterministic Behavior**.
    
      The kernel is designed to ensure that operations have bounded execution times. What this means is that no matter how many threads are created, the time to act does not increase, this makes the
      Time is unpredictable and not scalable with system size.
      
  - **Fast interrupt handling**.
    
      This basically means that the CPU can react quickly to external events with minimal delay.
        - This can be achieved by:
          - rarely blocking interrupts
          - Interruptions are accepted almost instantly.
          - interrupts response time stays predictable.

# Core Components
  - **Threads:** Independent executions units.
  - **Thread Scheduler:** Determines which thread runs next.
    
      The scheduler is the foundation of ThreadX. Here you will select the highest-priority runnable thread.
  - **Timers:** For periodic tasks.
    
      Here, you decide whether the application timers are one-shot (runs on time and then stops) or periodic (runs every X time). Its also the basis for delays and timeouts.
  - **Message Queues:** For safe inter-thread communication.
    
      Designed for blocking or non-blocking operations, fixed messages, and prioritized message delivery.
  - **Event Flags:** Allow multiple triggering conditions.

# Memory & Performance
  **Memory Footprint**
  A typical ThreadX kernel with essential services uses:
  
  6–9 KB RAM
  
  Memory is increased minimally based on many threads that exist.
  
# Typical Use Cases
  - Driver assistance sensors.
  - Infotainment side subsystems.
  - Battery management controllers.

# Why use Threadx?
  - **Ultra-low latency** suitable for real-time applications.
  - **Very stable and mature** (used in millions of devices).
  - Easy to learn & well-documented.
  - **Supports advanced features like:**
      - Preemption-threshold scheduling.
      - Priority inheritance.

# Example Thread Code (c)
  ```
void my_thread_entry(ULONG input)
{
    while(1)
    {
        printf("Hello from ThreadX!\n");
        tx_thread_sleep(100);
    }
}
```

# Advantages vs Other RTOS

| Feature | ThreadX | FreeRTOS | Zephyr | 
|------------|-----------|-------|------|
| Footprint | Very Small | Small | Medium |
| Performance | Excellent | Good |Good |
| Maturity | Very High | High | High |
| License  | Commercial | MIT | Apache 2 |

# Summary
  - ThreadX is a **fast**, **compact**, and **reliable** RTOS.
  - Ideal for embedded systems needing **deterministic performance**.
  - Part of the **Azure RTOS ecosystem**.
  - Used globally in millions of high-volume devices.
