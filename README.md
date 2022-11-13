# InteractiveSpatialSIR

A fun simulation of a spatially-resolved epidemic (SIR) model.

Welcome to our Oxford Hack 2022 project! :rocket:

## Inspiration

The mathematical beauty behind the SIR model of epidemics and the situational relevance.

## What it does

Hosts a server for players to connect to, coupled with a User Interface in C++ that simulates spatially resolved infection probabilities over a small area.
You can follow it live in time!

Once the main simulation hub is running, players should scan a QR code that opens a React application on their mobiles that enables them to control one person in the simulation at a time!
The goal is then to stay healthy as long as possible.

## How we built it

In C++, using Qt for the simulation interface, using a low-level code binding to facilitate the high-performance many-body simulation with interactive components.

## Challenges we ran into

Adjusting the probability distribution function used to model the infection.

## Accomplishments that we're proud of

It's scientifically not the most inaccurate simulation.
And we got the server running with a custom-made interaction protocol.

## What we learned

React, C++ and Qt.

## What's next for Stay healthy - Survive the Epidemic!

Probably we will extend the simulation to be able to compare it with existing research and models.
Mainly, it would be of high interest to compare the resulting graphs to those predicted by the SIR ordinary differential equations.
