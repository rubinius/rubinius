---
layout: doc_es
title: Etapa de Codificación
previous: Etapa de Generación
previous_url: bytecode-compiler/generator
next: Etapa de Empaquetación
next_url: bytecode-compiler/packager
---

Una vez el generador ha procesado el AST, necesita propiamente codificar el
bytecode. Esta etapa es muy simple, y es básicamente para documentación
formal.

La etapa de Codificación es responsable de dos cosas:

* convertir el flujo de instrucciones creado en la etapa anterior (cuando se
  llamaba el método `bytecode` sobre los nodos del AST) en una secuencia de
  instrucciones.
* validar que la pila no se desborde ni subdesborde, o que contenga algún otro
  tipo de semántica defectuosa.

Realiza estos pasos sobre el objeto Generator principal, así como en otros
objetos Generator hijos (generadores para bloques, métodos u otras estructuras
encontradas dentro del cuerpo principal).

Una vez esta etapa ha sido completada, se pasa el objeto Generator codificado
a la siguiente etapa, la etapa de Empaquetación.

## Ficheros mencionados

* *lib/compiler/generator.rb*: El método `encode` en este archivo realiza la
  mayor parte del trabajo en esta etapa.

## Personalización

Dado que esta etapa es muy simple, no hace falta personalizarla. Quizás se
quiera usarla con otros fines (por ejemplo, para profiling o impresión). Para
aprender más acerca de la personalización de partes del compilador en general,
lo cual puede ser usado para instrumentalizar cualquier etapa, léase
[Personalizando las etapas](/doc/en/bytecode-compiler/customization/).
