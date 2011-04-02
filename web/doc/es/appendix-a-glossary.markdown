---
layout: doc_es
title: Apéndice A - Glosario
previous: Cómos - Traducir Documentación
previous_url: how-to/translate-documentation
next: Apéndice B - Lista de Lecturas
next_url: appendix-b-reading-list
translated: false
---
Definición de términos y frases usadas en el lenguaje de programación Ruby y en
esta implementación. Vea también "The Ruby Programming Language" por Flanagan y
Matsumoto [O'Reilly 2008] y "Programming Ruby: The Pragmatic Programmer's Guide"
segunda o tercera edición por Thomas et al [The Pragmatic Programmers 2005-2008].


* _method lookup or method resolution (búsqueda de métodos o resolución
  de métodos)_

  La regla para la resolución de métodos es sencilla: Se toma el objeto
  que se encuentra en la posición *class* del objeto (que no siempre es
  igual a Object#class) y se comienza la búsqueda del método.

  La búsqueda se hace subiendo de niveles en la cadena de superclases
  hasta que el valor de superclass sea nil.

  Si superclass es nil se vuelve a comenzar la búsqueda tratando de hallar
  method_missing. Si no se encuentra method_missing se genera un error.

                                            +----------------+
                                            |      nil       |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            |     Object     |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            |     Module     |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            |     Class      |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            | SingletonClass |
                                            |    (Object)    |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
       +-------------+                      +----------------+
       |      F      |  ----------------->  | SingletonClass |
       +-------------+   singleton class    |      (F)       |
                                            +----------------+


      class Class
        def wanker
          puts 'you are'
        end
      end

      class F
        def self.bloke
          wanker
        end
      end

  1. Para la resolución del método 'wanker' se hace la búsqueda en las
  method_tables de:

      1. SingletonClass(F)
      1. SingletonClass(Object)
      1. Class

  Encontrado.


* _method_table (tabla de métodos)_

  Una estructura de datos hallada en todas las clases (y módulos) que contiene
  los métodos definidos para esa clase.

  En Rubinius la clase de method_table es una instancia de LookupTable.

* _MatzRuby_

  Ver MRI.


* _MRI_

  "Matz's Ruby Interpreter" o "Matz's Ruby Implementation". Una abreviación
  para referirse a la implementación oficial de Ruby. Ver
  <http://ruby-lang.org>.


* _private send (send privado)_

  Una llamada de método que no tiene un receptor léxico explícito. El
  receptor en este caso es +self+. Ejemplo:

      class A
      private
        def you_are_mine
        end
      end

      class B < A
        def sunshine
          you_are_mine
        end
      end

      class C
        def dear
          today = B.new
          today.you_are_mine
        end
      end

  La llamada a +you_are_mine+ en el método +sunshine+ es un send privado.
  La llamada a +today.you_are_mine+ no funcionará ya que los métodos privados
  no pueden tener un receptor explícito. En el caso anterior +today+ sería
  el receptor explícito.


* _singleton class (metaclase)_

  Cualquier objeto en Ruby puede tener una aunque solamente son creadas cuando
  es necesario. La singleton clase tiene las tablas de métodos y constantes
  que preñen a una instancia (objeto) en particular. Por ejemplo, el método
  +hello+ definido a continuación existe solamente en la singleton clase de
  +obj+.

      obj = Object.new
      def obj.hello
        puts 'hi'
      end

  Ya que las clases en Ruby también son objetos, estas pueden tener singleton
  clases.  Los métodos de clase (*class methods*) son simplemente métodos en
  la tabla de métodos de la singleton clase de la clase. El método +honk+
  existe en la singleton clase de la clase +Car+.

      class Car
        def self.honk
        end
      end

  Todas las singleton clases en Rubinius son instancias de la clase
  SingletonClass.  La singleton clase para un objeto puede ser obtenida
  llamando al método +singleton_class+.  Los conceptos mencionados aquí se
  conocen como el 'Meta-Object Protocol' o +MOP+.


* _superclass (superclase)_

  La clase de la cual una clase en particular hereda. La clase Object es la
  superclase de todas las clases que no heredan explícitamente de una clase.

      class A
      end

      class B < A
      end

  La clase A hereda de Object. En otras palabras, A.superclass == Object. La
  clase B hereda explícitamente de la clase A, así que B.superclass = A.
