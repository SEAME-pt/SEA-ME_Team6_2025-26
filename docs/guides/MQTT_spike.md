# Is it worth it to use MQTT in our project?

So in our we have a Raspberry Pi 5 running on our car with AGL and Kuksa databroker that will produce data like speed value, battery capacity, and so on, and we want our Raspberry Pi 4 that's connected to the display to consume that data. How do we do that ?


We have 2 Options, one is to use Kuksa directly and make PI 4 a client, and the other is to add MQTT between PI5 and PI4.

# Option 1: PI4 as a kuksa client

In this situation, PI4 will act as a remote kuksa client via gRPC or Websocket, allowing PI4 to subscribe to the signals.

Below, I'm gonna list some Pros and Cons.

**PROS**

  - **No duplication of signal logic**

    - This means that we only need to define the signals once(vehicle.speed) and the display will subscribe to them.

  - **Security & access control**

    - Our display should never write vehicle state, should never see only a set of signals, and it cant accidentally influence control paths. With Kuksa, this is a default behavior, with MQTT, it is our responsibility.

**CONS**

  - Display dependency

      - Display depends directly on KUKSA being reachable.

  - Heavier client

      - this doesnt mean that it will make it heavier or slower, it means that we will have more things to configure, and it will be more complex to integrate.

# Option 2: Use MQTT between Pi 5 and Pi 4

In this case, PI5 reads data from Kuksa, publishes selected signals via MQTT, and PI4 subscribes to those MQTT topics.

Below, I'm gonna list some Pros and Cons.

**PROS**

  - Very lightweight

    - very lightweight because the display only subscribes to simple topics and reacts to messages, with minimal code, minimal dependencies, and minimal runtime.

  - Display dependency

    - Display doesnt care where the data comes from.
   
  - Better for UI-driven systems

    - MQTT is ideal for UI-driven systems because it pushes updates as events, which maps directly to how displays render data in real time(with kuksa, there are a few more steps).

                ┌──────────────┐
                │   Pi 5       │
                │  Vehicle     │
                │  Sensors     │
                └─────┬────────┘
                      │
               ┌──────┴───────┐
               │              │
           ┌───▼───┐      ┌───▼───┐
           │KUKSA  │      │ MQTT   │
           │Data   │      │ Broker │
           │Broker │      └───┬───┘
           └───┬───┘          │
               │              │
               │              │
               │              │
               ▼              ▼
            ┌───────┐     ┌────────┐
            │ Pi 4  │     │ Pi 4   │
            │Dashboard│   │Dashboard│
            │Client │     │Client  │
            └───┬───┘     └───┬────┘
                │             │
           Decodes VSS       Receives
           structured data   simple push
                │            messages
                │             │
                ▼             ▼
            ┌───────────┐  ┌───────────┐
            │ Updates   │  │ Updates   │
            │ UI        │  │ UI        │
            └───────────┘  └───────────┘

**CONS**

  - Signal duplication

    - We have to define the signals on kuksa and define them again in MQTT topics.

  - Another broker

    - This means we that we gonna add one more broker that could lead us to more problems and more responsibility.
   


# Important notes

- There is no meaningful performance advantage either way.

  -  KUKSA gRPC/WebSocket → milliseconds
  -  MQTT → milliseconds


Ideal solution:

  Kuksa -> MQTT bridge -> PI4(display)
