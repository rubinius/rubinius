---
layout: doc_es
title: Garbage Collector
previous: JIT Compiler
previous_url: jit
next: Nursery
next_url: garbage-collector/nursery
review: true
---

## Introducción

Rubinius implementa la [Generational Garbage Collector (GC)](http://en.wikipedia.org/wiki/Garbage_collection_&#40;computer_science&#41;#Generational_GC_.28ephemeral_GC.29).
Rubinus gestiona el GC la asignación dinámica y liberación de memoria de 
espacio dinámico de un pronceso ejecutandose en un proceso de 
funcionamiento de Rubinius.

## Definición

1. Nosotros decimos que un objecto *vive* si hay al menos una estructura
(variable) en el intérprete que hace referencia al objecto.
1. Cuando el Colector de basura (GC) se ejecuta, este es llamado como una *colección*.
1. El número de veces que el GC se ejecuta llama al número de *ciclos de GC*
   o a la *colección de GC*.

## Ciclo de vida del Objecto

1. [Guardería](/doc/es/garbage-collector/nursery/)
1. [Generación Joven](/doc/es/garbage-collector/young-generation/)
1. [Generación Madura](/doc/es/garbage-collector/mature-generation/)
1. [Objetos grandes](/doc/es/garbage-collector/large-objects/)
