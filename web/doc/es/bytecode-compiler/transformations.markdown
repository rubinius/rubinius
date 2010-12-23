---
layout: doc_es
title: Transformaciones
previous: Compilador
previous_url: bytecode-compiler/compiler
next: Generador
next_url: bytecode-compiler/generator
---

El compilador a bytecode tiene un mecanismo simple de transformación
de árboles AST que reconoce ciertas formas AST y las remplaza. Las
formas remplazadas emiten un bytecode diferente del que sería emitido
por las formas originales. El código fuente para los transformadores
se encuentra en lib/compiler/ast/transforms.rb.


TODO: Documentar la arquitectura plugin del compilador.


### Transformación de matemáticas seguras.

Debido a que las bibliotecas del núcleo estan construidas con los
mísmos elementos que cualquier otro código Ruby y debido a que Ruby es
un lenguaje dinámico con clases abiertas y late-binding, es posible
cambiar clases fundamentales como Fixnum en formas que podrían violar
el comportamiento del que dependen algunas otras clases. Por ejemplo,
imagine que hicieramos lo siguiente:

    class Fixnum
      def +(other)
        (self + other) % 5
      end
    end

Si bien es cierto que es posible redefinir el operador aritmético de
suma para que realice una operación módulo 5, hacerlo, sin duda
causaría que algunas clases como Array no pudiesen calcular siquiera la
longitud correcta cuando lo necesite. La naturaleza dinámica de Ruby
es uno de sus rasgos más apreciados, pero también es una verdadera
arma de doble filo en algunos aspectos.

En la biblioteca estándar, 'mathn' redefine Fixnum#/ de una manera
peligrosa e insegura. La biblioteca crea un alias de Fixnum#/ a
Fixnum#quo, que regresa un Float de forma predeterminada.

Debido a esto existe un plugin especial para el compilador que emite
métodos con nombre diferente cuando se encuentra con el método #/. El
compilador emite #divide en vez de #/. Las clases numéricas Fixnum,
Bignum, Float, y Numeric, todas definen este método.

Las transformaciones para matemáticas seguras estan habilitadas
durante la compilación de las bibliotecas del núcleo. Durante la
compilación de 'código de usuario' regular, el plugin es
deshabilitado. Esto nos permite soportar mathn sin alterar las
bibliotecas del núcleo o forzar a prácticas inconvenientes.
