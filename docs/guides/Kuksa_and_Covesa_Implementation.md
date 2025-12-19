# Kuksa and Covesa Implementation

In this guide, I will show you how to implement COVESA(VSS) with KUKSA(Databroker) on an AGL build.

Before we start, imagine this:
VSS is the contract, and CAN, Databroker, VISS, and UI all serve the VSS tree.

# 1 Step - Defining VSS tree

For this step, we will use the Covessa VSS tree already made(standard tree).

After that, we need to make sure every signal we need is on the standard tree otherwise, we need to write it on the tree.

What we must define as our vss signals:

| Property    | Example                               |
| ----------- | ------------------------------------- |
| Path        | `Vehicle.Speed`                       |
| Type        | `float`, `int`, `boolean`, `string` |
| Unit        | `km/h`, `percent`, `celsius`          |
| Description | Human readable                        |
| Access      | read / write / subscribe              |

Ok, now let's take a deep dive into how we're gonna start.

We need to pick a VSS version and go with it all the way.


# Step 2 - Include Kuksa Databroker on AGL

**documentação de como foi feito aqui**


Now that we have the Databroker on our AGL image, we need to install our VSS file.

We need to install our VSS file to:

    /etc/kuksa/vss.json

Next, let's boot AGL and verify:

    systemctl status kuksa-databroker

Use the KUKSA CLI:

    kuksa-client get Vehicle.Speed

Kuksa cli is a command-line client for talking with the KUKSA databroker.

# Step 3 - Adding kuksa viss server

This step will make us sure that our VSS tree is working as expected.














