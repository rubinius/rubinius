# This file specs the methods available to Class. Syntax 
# related to Class (such as the class and def keywords) 
# is in spec/language/class_syntax_spec.rb
#
require File.dirname(__FILE__) + '/../spec_helper'

# Class methods:
#   .new              OK
#
# Instance methods:
#   #allocate         OK
#   #inherited        OK
#   #new              OK
#   #superclass       OK
#

context 'Using Class.new to create a new class' do
  specify 'Returns a new anonymous Class instance' do
    example do 
      Class.new.class
    end.should == Class
  end   

  specify 'May be given a Class argument to be used as superclass (Object by default)' do
    example do 
      a = Class.new
      b = Class.new(Array)
      c = Class.new(Enumerable) rescue :failure

      [a.superclass, b.superclass, c]
    end.should == [Object, Array, :failure]
  end

  specify 'If a block is provided, it is evaluated in the context of the Class object' do
    example do
      c = Class.new {NO = :inside; const_set :YES, :inside
                     @civ = :civ; @@cv = :cv; 
                     def self.foo; :class_foo; end 
                     define_method('foo') {:instance_foo}}

      [c.constants, c.instance_variables, c.class_variables, c.foo, c.new.foo]
    end.should == [['YES'], ['@civ'], ['@@cv'], :class_foo, :instance_foo]
  end
end


context 'Class object instantiation' do
  setup do
    class ClassSpec
      def initialize(); @a = true; end
      def foobar(); :foobar; end
    end
  end

  specify '#allocate allocates space for the object but does not run the instance method #initialize' do
    example do
      cs = ClassSpec.allocate
      [cs.instance_variables, cs.foobar]
    end.should == [[], :foobar]
  end

  specify '#new allocates space for the object and runs the instance method #initialize' do
    example do
      cs = ClassSpec.new
      [cs.instance_variables, cs.foobar]
    end.should == [['@a'], :foobar]
  end
end


context 'Class event hook methods' do
  specify '#inherited, if implemented, is called when a Class object is inherited. Subclass object given as parameter.' do
    example do
      class ClassSpecNotInherited
        @@i = nil
      end
      
      class ClassSpecInherited
        @@i = nil
        def self.inherited(by); @@i = by; end
      end
       
      Class.new ClassSpecNotInherited
      i = Class.new ClassSpecInherited

      [ClassSpecNotInherited.send('class_variable_get', '@@i') == nil, 
       ClassSpecInherited.send('class_variable_get', '@@i') == i]
    end.should == [true, true]
  end
end

context 'Instantiated Class object' do
  specify 'Makes its superclass object available through #superclass' do
    example do
      class CS_SuperA; end
      class CS_SuperB < Array; end
      CS_SuperC = Class.new
      CS_SuperD = Class.new Array
      CS_SuperE = Class.new CS_SuperC

      [CS_SuperA.superclass, CS_SuperB.superclass, 
       CS_SuperC.superclass, CS_SuperD.superclass,
       CS_SuperE.superclass.superclass]
    end.should == [Object, Array, Object, Array, Object]
  end
end
