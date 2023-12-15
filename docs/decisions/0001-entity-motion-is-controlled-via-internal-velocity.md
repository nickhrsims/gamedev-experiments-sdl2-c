---
# These are optional elements. Feel free to remove any of them.
status: "proposed"
date: 2023-12-15
deciders: Nick
consulted: None
informed: None
---

# Entity motion is controlled via internal velocity

## Context and Problem Statement

Due to use of `C`, structuring data and operations on that data are less
flexible, requiring discipline. Entities of different types are defined
by their operations, rather than data unique to them. A such, data that drives
similar but different operations must be flexible enough to be useful to all
purposes entities serve in the game. This record deals with decisions regarding
data related to entity motion.

<!-- This is an optional element. Feel free to remove. -->

## Decision Drivers

- No language-support for OOP (data structure / type polymorphism).

- Ball and Paddle motion are driven by different factors, but to aid code  
  surface reduction should have common API.

- One architectural objective of the project is for entities to data-driven,  
  allowing for diverse in entity capability without necessarily being encumbered  
  by more entity-specific code.

## Considered Options

- Entity motion is driven directly through different sets of operations based  
  on context.

- Entity motion is driven via a common operation on each entity's velocity  
  properties.

## Decision Outcome

Chosen option:
"Entity motion is driven via a common operation on each entity's velocity  
properties"

### Consequences

- Good, because all motion operations can be grouped and generalized around  
  velocity as a system of motion.

- Good, because all motion is driven by simple data modifications, abstracting
  away use of delta on a per-operation basis.

- Good, because all non-motion systems with motion-based relationships (such as
  collision) have a simple way to convey appropriate changes through
  velocity/trajectory.

- Bad, because some degree of flexibility is lost for simplicity's sake when
  unable to directly modify position.

- Bad, because different complex forms of motion based on positional
  interpolation cannot be achieved without violating use of velocity (ignoring
  velocity properties, which would lead to operational inconsistency and increase
  complexity).

### Confirmation

Current implementation already reflects this.

Future prospects may include data-hiding of entities to prevent direction
positional manipulation (thus enforcing motion via velocity).
