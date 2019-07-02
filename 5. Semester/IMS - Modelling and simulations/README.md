# Modelling and simulations project - Taxi service simulation in Nove Zamky

The goal of this project was to discretely model and simulate a real-world system of our choice.
Our team chose to model the dynamics of a taxi service stationed in front of a rail station in a Slovak travel hub.
The system is modelled using Petri nets and implemented in a simulation library Simlib (https://www.fit.vutbr.cz/~peringer/SIMLIB/).
Our goal was to determine whether it would be profittable to open a new taxi service in Nove Zamky under the current conditions

The model captures the following properties of the system:
 - Arrival of passengers by regular trains and by more packed intercity trains
 - Waiting queue for taxis with a timeout
 - Round-trip time based on destination
 - Competition taxis
