---
layout: doc_es
title: Variables globales
previous: Variables de clase
previous_url: ruby/class-variables
next: Especificaciones
next_url: specs
review: true
---

Sintacticamente, una variable global es una variable cuyo nombre
comienza con `$`. Las variables globales estan disponibles
en cualquier contexto dentro de un programa Ruby. Sin embargo, en realidad hay tres
tipos de variables globales: globales verdaderas, globales por-hilo, y pseudo
globales.

Las globales verdaderas asocian un valor con un nombre universal, tales como
`$LOAD_PATH`.

Las globales por-hilos tienen la sintaxis de una variable global, pero
existe una version diferente de dicha global para cada hilo de
ejecución en el proceso Ruby. Ejemplos de globales por-hilo son `$SAFE` y `$!`.
Para ver que estos valores dependen del hilo de ejecución, considere el siguiente
código:

    puts $SAFE

    Thread.new do
      $SAFE = 2
      puts $SAFE
    end

    puts $SAFE

Las variables pseudo globales son un subconjunto de nombres que hacen
referencia no a valores globales, si no a valores en el scope actual,
como si fueran variables locales. Aún asi se refiere a ellas como
variables globales porque su nombre comienza con un signo de
dolar. Este hecho ha sido fuente de confusión para algunos usuarios.

Todas las variables pseudo globales estan organizadas en torno a una
pseudo global principal: `$~`. Todas las demás son parte de `$~` asi
que cuando `$~` cambia, las demás cambian también.

El resto de las pseudo globales son: `$&`, <code>$`</code> (backtick), `$'` (single
quote), `$+`, y `$1`, `$2`, `$3`, etc.

Un aspecto interesante de estos valores es que estan estríctamente
ligados al scope actual, pero Ruby permite que tengan alias, como se
hace en English.rb.

Lo que hacen estos nuevos alias es, basicamente, agregar nuevas
variables locales a todos los scope, aun en aquellos que se encuentran
ejecutando. Por lo tanto, Rubinius no los soporta completamente. En
vez de ello, Rubinius expone los alias presentes en English.rb por
omisión. Por ejemplo, se puede utilizar `$MATCH` en lugar `$~`, aunque
English.rb este incluido o no.

