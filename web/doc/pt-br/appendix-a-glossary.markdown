---
layout: doc_pt_br
title: Apêndice A - Glossário
previous: Como - Traduzir Documentação
previous_url: how-to/translate-documentation
next: Apêndice B - Lendo a Lista
next_url: appendix-b-reading-list
review: false
---

Definições dos termos e frases usadas na linguagem de programação Ruby e nessa
implementação. Veja também "The Ruby Programming Language" by Flanagan and
Matsumoto [O'Reilly 2008] and "Programming Ruby: The Pragmatic Programmer's
Guide" 2nd or 3rd Edition by Thomas et al [The Pragmatic Programmers
2005-2008]


* método de pesquisa ou resolução de método

  A regra é simples: Pegue o objeto localizado na classe de onde o objeto veio
  (que nem sempre retorna um valor de Objetct#class; se o objeto tiver um. Ele
  será uma classe singleton) e começa a procurar.

  A busca sobe até a cadeia de superclasses enquanto a superclasse for nil.

  Nesse caso, procura pelo method missing. Se ele falhar na busca
  o method missing falha tragicamente

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

  1. Resolvendo método 'wanker' -- procura método nas tabelas:

      1. SingletonClass(F)
      1. SingletonClass(Object)
      1. Class

  Encontrado


* _method_table_

  A estrutura de dados em cada classe (e módulo) que contem os métodos definidos
  para cada classe.

  No Rubinius um método de classe é uma instância de LookupTable.


* _MatzRuby_

  Veja MRI


* _MRI_

  Matz's Ruby Interpreter ou implementação do interpretador ruby. O nome da
  implementação oficial do Ruby. Veja <http://ruby-lang.org>.


* _private send_

  Uma chamada de método que não tem um receptor léxico explícito. O receptor
  de uma chamada é o +self+. Por exemplo:

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

  A chamada para +you_are_mine+ no método +sunshine+ é uma chamada privada.
  A chamada para +today.you_are_mine+ não terá sucesso porque métodos privados
  não tem receptor explicito. Nesse caso, o objeto +today+ é um receptor 
  explicito

* _singleton class_

  Todo objeto no Ruby pode ter uma, apesar de elas só serem criadas quando
  necessário. A classe singleton detém o método e a tabela constanteis que
  pertence somente a uma instancia de objeto particular. Por exemplo, o
  método +hello+ definido abaixo existe somente na classe singleton de +obj+.

      obj = Object.new
      def obj.hello
        puts 'hi'
      end

  Todas as classes em Ruby são também objetos, elas podem ter também sua 
  classe singleton. Os métodos chamados 'métodos de classe' são somente
  métodos na tabela de métodos da classe singleton. O método +honk+ existe
  na classe singleton para a classe +Car+.

      class Car
        def self.honk
        end
      end

  No Rubiniys, classes singleton são instancias da classe SingletonClass.
  A classe singleton para um objeto pode ser obtida pela chamada para
  método +singleton_class+. O arranjo global dos conceitos envolvidos aqui
  é as vezes referenciados como o 'Protocolo Meta-Objeto' ou +MOP+.

* _superclass_

  É uma classe que  uma determinada classe herda imediatament. A classe Objetct
  é a superclasse de todas as classes que não herda explicitamente de outra classe.

      class A
      end

      class B < A
      end

  A classe A herda de Object. Em outras palavras, A.superclass == Objetc. A classe B
  herda explicitamente da classe A. Então, B.superclass == A.
