---
layout: doc_es
title: Etapa de Empaquetación
previous: Etapa de Codificación
previous_url: bytecode-compiler/encoder
next: Etapa de Escritura
next_url: bytecode-compiler/writer
---

Una vez que el generador ha sido correctamente codificado en la etapa de
Codificación, Rubinius empaqueta el bytecode creando un nuevo método compilado
(un objeto CompiledMethod) con ciertos atributos.

Estos atributos están expuestos en cualquier CompiledMethod. Es posible
extraer un CompiledMethod a partir de un método de Ruby llamando `executable`
sobre el método.

* *iseq*: un objeto Tuple con la secuencia de instrucciones
* *literals*: un objeto Tuple con los literales utilizados en el método. Los
  literales se utilizan internamente por Rubinius para valores como Strings,
  y las instrucciones que los manipulan son `push_literal` y `set_literal`.
* *lines*: un Array que contiene el primer puntero de instrucción para cada
  línea representada en el bytecode
* *required_args*: el número de argumentos requerido por el método
* *total_args*: el número total de argumentos, incluyendo argumentos
  opcionales exceptuando `*args`
* *splat*: la posición del argumento splat, si existe
* *local_count*: el número de variables locales, incluyendo parámetros
* *local_names*: un objeto Tuple con una lista de nombres de todas las
  variables locales. Los primeros nombres serán los argumentos requeridos,
  opcionales, splat y de bloque, en ese orden exactamente.
* *file* el nombre del fichero que se utilizará en los stack traces y otra
  información de debugging.
* *name*: el nombre del método
* *primitive*: el nombre de la primitiva asociada a este método, si es que hay
* metadatos: es posible almacenar metadatos adicionales en un método
  compilado. Éste tiene un metadato llamado `for_block` con el valor `true` si
  el generador original fue creado para un bloque.

La etapa de Empaquetación se encarga también de que cualquier generador hijo
(como por ejemplo los generadores para bloques o métodos) también sean
convertidos en métodos compilados. Estos métodos compilados hijos se incluyen
en la tupla de literales del método compilado padre.

Después de que el Generator ha terminado de empaquetarse a sí mismo como un
CompiledMethod, invoca la etapa de Escritura, con el CompiledMethod como
entrada.

## Ficheros mencionados

* *kernel/bootstrap/compiled_method.rb*: la implementación básica de
  CompiledMethod, básicamente compuesta de diversas primitivas
* *kernel/common/compiled_method.rb*: una implementación más robusta de
  CompiledMethod, una combinación de métodos primitivos y métodos escritos en
  Ruby
* *vm/builtin/compiledmethod.cpp*: la implementación en C++ de las primitivas
  de un CompiledMethod
* *lib/compiler/generator.rb*: La implementación del método `package`, el cual
  popula el CompiledMethod con información sobre el objeto Generator.

## Personalización

En general, el método `package` está diseñado para popular el CompiledMethod
con un grupo de variables. Sin embargo, también se podría utilizar el
empaquetador para popular otro objeto con la misma interfaz. A pesar de todo,
esto podría no ser útil en sí mismo sin otras personalizaciones más adelante.
