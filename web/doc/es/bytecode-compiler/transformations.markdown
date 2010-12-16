---
layout: doc_es
title: Compiler Transforms
previous: Compiler
previous_url: bytecode-compiler/compiler
next: Generator
next_url: bytecode-compiler/generator
translated: true
---

El compilador de código de bytes tiene un simple mecanismo de AST
transformación que reconoce ciertas formas de AST y los sustituye. Las formas
sustituye emiten diferentes código de bytes de la forma original se han
emitido. La fuente de la se transforma en lib/compiler/ast/transforms.rb


TODO: document the compiler plugin architecture.


### Compilador de Matemáticas seguro de transformación

Dado que las bibliotecas del núcleo se construyen con los mismos bloques como
cualquier otro código de Ruby y desde Ruby es un lenguaje dinámico, con clases
abiertas y el enlace, es posible cambiar las clases fundamentales como Fixnum
en formas que violan los semántica que dependen de otras clases. Por ejemplo,
imagine que hicimos el siguientes:


  class Fixnum
    def +(other)
      (self + other) % 5
    end
  end

Si bien es ciertamente posible para redefinir aritmética de punto fijo más ser
módulo 5, haciendo así que sin duda hará que alguna clase de matriz como para
no poder calcular la longitud correcta cuando se necesita. La naturaleza
dinámica de Ruby es uno de sus rasgos apreciados, pero también es una
verdadera arma de doble filo en algunos aspectos.

En la colección de la biblioteca estándar de la 'mathn' redefines Fixnum#/ en
un peligroso e incompatibles manera. La colección de alias Fixnum#/ a
Fixnum#quo, que devuelve un flotador de forma predeterminada.

Debido a esto existe un compilador especial plugin que emite una diferente
nombre de método cuando se encuentra con el #/ método. El compilador emite
#divide en lugar de #/. Las clases numéricas Fixnum, un número grande,
flotador, y numérico todos los definir este método.

Las matemáticas segura transformación está habilitada durante la compilación
del Núcleo bibliotecas para que el plugin. Durante la compilación regular
"user code", el plugin no está habilitado. Esto nos permite apoyar mathn sin
romper el bibliotecas del núcleo o forzar a prácticas inconvenientes.
