# mangoponent
## What is it?

mangoponent is an experimental WIP implementation of an Entity Component system, currently targeting C.

Its main aim is to provide a common ground for implementing "component" modules which can interact with each other via a shared API.

Components implement "fields", "interfaces", and "callbacks" in order to interact with other elements of the system. In the case where modules might want more direct private interactions, these abstractions can be skipped.

"fields" are simple properties that can be modified from any part of the system.

"interfaces" are groups of functions exposed by components that implement the "API" of an object.

"callbacks" are analogous to "systems" in ECS but actual registration of the objects with each callback is deferred to the components.

Currently the following is planned to be implemented:

- An implementation of an object system to own references to components, interfaces, and callbacks
- Shared library loading & unloading
- Reference scripting backends
- A set of example components for testing


## Licensing

For licensing details, refer to the LICENSE file in the root directory.
