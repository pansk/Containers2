# Containers2

Containers2 is a proof-of-concept implementation of non-stl-like containers.
The main notable differences with stl containers are:

- Decoupled constness (a container of const elements of type T is different from a const container of elements of type T)
- Views and memory management embedded in the design (views must also be trivially copyable)
- No deep copies (copies must always be explicit)