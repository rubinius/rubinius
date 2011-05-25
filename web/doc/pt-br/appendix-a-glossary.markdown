---
layout: doc_pt_br
title: Appendix A - Glossary
previous: How-To - Translate Documentation
previous_url: how-to/translate-documentation
next: Appendix B - Reading List
next_url: appendix-b-reading-list
review: true
---

Definitions of terms and phrases used in the Ruby programming language and in
this implementation. See also "The Ruby Programming Language" by Flanagan and
Matsumoto [O'Reilly 2008] and "Programming Ruby: The Pragmatic Programmer's
Guide" 2nd or 3rd Edition by Thomas et al [The Pragmatic Programmers
2005-2008]


* _method lookup or method resolution_

  The rule is simple: Take the object located in the class slot of the object
  (which is not always the return value of Object#class; if the object has one,
  it'll be the singleton class) and begin searching.

  Searching goes up the superclass chain until the superclass is nil.

  In which case, redo lookup for method_missing. If we fail to find
  method_missing, fail tragically.

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

  1. Resolve method 'wanker' -- search method_tables in:

      1. SingletonClass(F)
      1. SingletonClass(Object)
      1. Class

  Found


* _method_table_

  A data structure in every class (and module) that contains the methods defined
  for that class.

  In Rubinius, a class's method_table is an instance of LookupTable.


* _MatzRuby_

  See MRI


* _MRI_

  Matz's Ruby Interpreter or Matz's Ruby Implementation. A short name to refer
  to the official implementation of Ruby. See <http://ruby-lang.org>.


* _private send_

  A method call that has no explicit lexical receiver. The receiver of the
  call is +self+. For example:

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

  The call to +you_are_mine+ in the method +sunshine+ is a private send. The
  call to +today.you_are_mine+ will not succeed because private methods cannot
  have an explicit receiver. In this case, the object +today+ is the explicit
  receiver.


* _singleton class_

  Every object in Ruby can have one, although they are only created as
  necessary. The singleton class holds the method and constant tables that
  belong only to a particular object instance.  For example, the method
  +hello+ defined below exists only in the singleton class for +obj+.

      obj = Object.new
      def obj.hello
        puts 'hi'
      end

  Since all classes in Ruby are also objects, they can have singleton classes.
  The methods called "class methods" are just methods in the method_table of
  the class's singleton class. The method +honk+ exists in the singleton class
  for the class +Car+.

      class Car
        def self.honk
        end
      end

  In Rubinius, singleton classes are all instances of the class
  SingletonClass. The singleton class for an object can be obtained by calling
  the +singleton_class+ method.  The overall arrangement of concepts involved
  here is sometimes referred to as the 'Meta-Object Protocol' or +MOP+.


* _superclass_

  The class that a particular class immediately inherits from. The class Object
  is the superclass of all classes that do not inherit explicitly from a class.

      class A
      end

      class B < A
      end

  Class A inherits from Object. In other words, A.superclass == Object. Class B
  inherits explicitly from class A. So, B.superclass == A.
