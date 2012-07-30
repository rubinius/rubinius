---
layout: doc_es
title: Etapa de Escritura
previous: Etapa de Empaquetación
previous_url: bytecode-compiler/packager
next: Transformaciones
next_url: bytecode-compiler/transformations
---

Una vez el Packager ha creado el CompiledCode, Rubinius escribirá el método
en un fichero para consumo futuro. Por ejemplo, después de que un fichero es
requerido por primera vez, si se requiere más adelante cargará el fichero del
disco, en vez de cargar el código Ruby, analizarlo y compilarlo de nuevo.

Esta etapa es extremadamente simple. Toma el nombre del fichero original, le
añade una `c` al final, y llama a Rubinius::CompiledFile.dump con el
CompiledCode de la etapa anterior y el nuevo nombre de fichero donde
escribir.

Después de escribir el fichero en disco, devuelve la entrada (el
CompiledCode), el cual pasa a ser el valor de retorno de todo el proceso de
compilación.

## Ficheros mencionados

* *lib/compiler/compiled_file.rb*: la implementación de CompiledFile.
  `CompiledFile.dump` es el método llamado para realizar toda la escritura.

## Personalización

Esta etapa es opcional, y sólo se usa cuando compilamos un fichero. En el caso
de compilar un String, como con eval, nos saltamos esta etapa. En ese caso, el
compilador se detiene en la etapa de Empaquetación, y devuelve el
CompiledCode de esa etapa como valor de retorno del compilador.

Dada la arquitectura del compilador de Rubinius, es muy sencillo añadir etapas
adicionales al final del proceso. Siempre y cuando cada una de dichas etapas
devuelva el mismo CompiledCode (o uno diferente), todo irá como se espera.

Para más información, léase [Personalizando las
etapas](/doc/en/bytecode-compiler/customization/).
