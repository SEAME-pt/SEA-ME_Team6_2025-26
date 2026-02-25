> Create a CAN emulator that can replicate STM32's frame sending
> This is done to test the UI without having to manually drive the car

Flow:
- Open `vcan0`
- Read a scenario file
- Periodically send CAN frames
- Simulate realistic timing

## How to setup

### 1️⃣ Build the emulator
`g++ -std=c++17 -O2 can_emulator.cpp -o can_emulator -lpthread`

### 2️⃣ Create and enable `vcan0`
`sudo modprobe vcan`
`sudo ip link add dev vcan0 type vcan`
`sudo ip link set up vcan0`
`ip link show vcan0`

### 3️⃣ Test that CAN works
`candump vcan0`

#### ⓸ Run the emulator
`./can_emulator vcan0 ./scenarios`

### 5️⃣ Run the can_to_kuksa_publisher with vcan0
`./home/kuksa_RPi5/bin/can_to_kuksa_publisher vcan0`

In this case we can't use the can_to_kuksa service that is running on AGL because it is hardcoded to can1 channel.
So we run the binary and specify the channel - `vcan0`

### ⓻ Check if it is working with kuksa-client
`kuksa-client --cacertificate /etc/kuksa/tls/ca.crt --token_or_tokenfile /etc/kuksa/jwt/publisher.jwt grpcs://10.21.220.191:55555`

Then:
`getValue Vehicle.Speed` 

Or:
`Run Qt and see the changes in the cluster`
