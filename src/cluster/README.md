
# SEA:ME Team 6 Cluster

This README serves the purpose of documentating the development of the Cluster that will be used as the main GUI of the Jetracer AI Kit

(Expandir)


## Installation

To be able to run the program it will be needed to have a full installation of the QT6 and QML Packages ([QT](https://doc.qt.io/)) (dar mais informação das packages, quais são, etc)


After the whole QT setup, clone the repository to a place of your choice using the following command:

```bash
  git clone git@github.com:SEAME-pt/SEA-ME_Team6_2025-26.git
```
After clonning the repository you will need to enter insise the `src` folder.

In there you will find a Makefile with a defined set of rules
#### Makefile Rules Reference

| Parameter |  Description                |
| :-------- |  :------------------------- |
| `make` | Runs the `run.sh` file, located in the scripts folder. That scripts is responsible for the build of the project |
| `make clean` | Responsible to delete the `build` folder, mostly used in cases of project forced rebuilding |
| `make exec` | Runs the executable created by the build of the project |
| `make re` | Runs the `make clean && make` in order to be able to restart the project |


