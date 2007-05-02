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
    Class.new.class.should == Class
  end   

  specify 'May be given a Class argument to be used as superclass (Object by default)' do
    Class.new.superclass.should == Object
    Class.new(Array).superclass.should == Array
    Class.new(Enumerable) rescue :failure
  end

  specify 'If a block is provided, it is evaluated in the context of the Class object' do
    c = Class.new {NO = :inside; const_set :YES, :inside
                   @civ = :civ; @@cv = :cv; 
                   def self.foo; :class_foo; end 
                   define_method('foo') {:instance_foo}}

    c.constants.should == ['YES']
    c.instance_variables.should == ['@civ']
    c.class_variables.should == ['@@cv']
    c.foo.should == :class_foo
    c.new.foo.should == :instance_foo
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
    cs = ClassSpec.allocate
    cs.instance_variables.should == []
    cs.foobar.should == :foobar
  end

  specify '#new allocates space for the object and runs the instance method #initialize' do
    cs = ClassSpec.new
    cs.instance_variables.should == ['@a']
    cs.foobar.should == :foobar
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
    class CS_SuperA; end
    class CS_SuperB < Array; end
    CS_SuperC = Class.new
    CS_SuperD = Class.new Array
    CS_SuperE = Class.new CS_SuperC

    CS_SuperA.superclass.should == Object 
    CS_SuperB.superclass.should == Array 
    CS_SuperC.superclass.should == Object 
    CS_SuperD.superclass.should == Array
    CS_SuperE.superclass.superclass.should == Object
  end
end
