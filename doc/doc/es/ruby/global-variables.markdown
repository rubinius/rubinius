---
layout: doc_es
title: Global Variables
previous: Class Variables
previous_url: ruby/class-variables
next: Specs
next_url: specs
review: true
translated: true
---

Syntatically, una variable global se supone que es una variable cuyo nombre
comienza con `$`. Las variables globales se supone que deben estar disponibles
en cualquier contexto en un Ruby programa. Sin embargo, en realidad hay tres
tipos de variables globales: true globales, globales locales-hilos, y pseudo
globales.

Globales verdadero asociar un valor con un nombre mundial, tales como
`$LOAD_PATH`.

Tema globales-locales tienen la sintaxis de una variable global, pero hay una
versión diferente de lo global para cada subproceso en el proceso en ejecución
Ruby.  Ejemplos de variables globales locales hilos son `$SAFE` y `$!`.
Para ver que estos valores dependen de la rosca, considere el siguiente
código:

    puts $SAFE

    Thread.new do
      $SAFE = 2
      puts $SAFE
    end

    puts $SAFE

Pseudo globales son un subconjunto estricto de los nombres que se refieren no
a los valores globales pero los valores en el ámbito actual, al igual que las
variables locales. Estos siguen siendo a que se refiere como variables
globales porque comienzan con un signo de dólar, pero esto es la fuente de
confusión para los usuarios.

Todas las variables globales pseudo se organizan alrededor de una pseudo
primaria global: `$~`. Ellos son piezas simples de acceder a `$` y por lo
tanto, cuando `$` cambios, todos ellos cambio.

El resto de la pseudo globales son: `$&`, <code>$`</code> (backtick), `$'` (single
quote), `$+`, y `$1`, `$2`, `$3`, etc.

Un aspecto delicado de estos valores es que son estrictamente ligado a la
actual, el alcance, pero Ruby les permite tener un alias, como se hace en
English.rb.

Estos nuevos alias son, básicamente, la adición de nuevas variables locales de
todos los ámbitos, incluso que ya están en marcha. Thusly, Rubinius no mi
pleno apoyo. En su lugar, simplemente proporcionar el alias presente en
English.rb de forma predeterminada. Por ejemplo, `$MATCH` puede utilizarse
en lugar `$~`, English.rb si se incluye o no.

