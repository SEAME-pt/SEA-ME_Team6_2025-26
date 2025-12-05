# Benefits of Automotive Grade Linux

- Inherits the robustness and security of Linux  
- Backed by the Linux Foundation and major automakers  
- Includes ready-to-use frameworks for multimedia, connectivity, HMI, navigation, and vehicle bus (CAN) integration  
- Based on standard Linux, so developers can modify kernels, drivers, and user-space applications freely to fit specific needs.  

---

# Benefits of ThreadX

- Ultra-low interrupt latency and fast context switching make it ideal for time-critical automotive functions  
- Requires very little memory, making it perfect for microcontrollers  
- Simple API and rich debugging tools  
- ThreadX’s preemptive, priority-based scheduling ensures predictable task execution  

---

## Combined Benefits on a PiRacer

- **Hybrid Architecture:** AGL handles the **high-level user interface, AI inference, and connectivity**, while ThreadX manages **real-time motor and sensor control**.  
- **Optimized Performance:** Dividing tasks between AGL (Linux-based) and ThreadX (RTOS) maximizes both processing efficiency and determinism.  
- **Scalable Design:** This setup mirrors modern automotive ECUs, making the PiRacer a strong platform for **autonomous driving research and prototyping**.  
- **Educational and Practical:** Ideal for learning about real automotive-grade system design — combining open-source Linux flexibility with real-time embedded reliability. 
