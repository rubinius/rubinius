require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# method_added, new
 
# ==, ===, =~, __id__, __send__, class, class_def, clone,
# copy_instance_variables_from, dclone, deep_clone, display, dup,
# enum_for, eql?, equal?, expects, extend, freeze, frozen?, hash, id,
# inspect, inspect_for_expectation_not_met_error, instance_eval,
# instance_exec, instance_of?, instance_variable_get,
# instance_variable_set, instance_variables, is_a?, kind_of?,
# meta_def, meta_eval, metaclass, method, methods, mocha_inspect,
# nil?, object_id, parse_tree, private_methods, protected_methods,
# public_methods, remove_instance_variable, respond_to?, returning,
# send, singleton_method_added, singleton_method_removed,
# singleton_method_undefined, singleton_methods, source, stubs,
# taint, tainted?, to_a, to_enum, to_json, to_s, to_yaml,
# to_yaml_properties, to_yaml_style, type, untaint, verify

context "Object class method" do
  specify "new should create a new Object" do
    example do
      Object.new.class
    end.should == Object
  end
end

context "Object instance method" do
  specify "send should invoke the named method" do
    example do
      class Foo
        def bar
          'done'
        end
      end

      Foo.new.send(:bar)
    end.should == 'done'
  end
  
  specify "freeze should prevent self from being further modified" do
    example do
      module Mod; end
      @o = Object.new
      @o.freeze
      begin
        @o.extend(Mod)
      rescue TypeError => @e
      end
      @e.message
    end.should == "can't modify frozen object"
  end
  
  specify "freeze should have no effect on immediate values" do
    example do
      @a = nil
      @b = true
      @c = false
      @d = 1
      @a.freeze
      @b.freeze
      @c.freeze
      @d.freeze
      [@a.frozen?, @b.frozen?, @c.frozen?, @d.frozen?]
    end.should == [false, false, false, false]
  end
  
  specify "frozen? should return true if self is frozen" do
    example do
      @o = Object.new
      @p = Object.new
      @p.freeze
      [@o.frozen?, @p.frozen?]
    end.should == [false, true]
  end
  
  specify "taint should set self to be tainted" do
    example do
      Object.new.taint.tainted?
    end.should == true
  end
  
  specify "taint should have no effect on immediate values" do
    example do
      @a = nil
      @b = true
      @c = false
      @d = 1
      @a.taint
      @b.taint
      @c.taint
      @d.taint
      [@a.tainted?, @b.tainted?, @c.tainted?, @d.tainted?]
    end.should == [false, false, false, false]
  end
  
  specify "tainted? should return true if Object is tainted" do
    example do
      @o = Object.new
      @p = Object.new
      @p.taint
      [@o.tainted?, @p.tainted?]
    end.should == [false, true]
  end
  
  specify "instance_eval with no arguments should expect a block" do
    example do
      begin
        "hola".instance_eval
      rescue ArgumentError => @e
        @e.message
      end
    end.should == "block not supplied"
  end
  
  specify "instance_eval with a block should take no arguments" do
    example do
      begin
        "hola".instance_eval(4, 5) { |a,b| a + b }
      rescue ArgumentError => @e
        @e.message
      end
    end.should == "wrong number of arguments (2 for 0)"
  end
  
  specify "instance_eval with a block should pass the object to the block" do
    example do
      "hola".instance_eval { |o| o.size }
    end.should == 4
  end
  
  specify "instance_eval with a block should bind self to the receiver" do
    example do
      @s = "hola"
      @s == @s.instance_eval { self }
    end.should == true
  end
  
  specify "instance_eval with a block should execute in the context of the receiver" do
    example do
      "Ruby-fu".instance_eval { size }
    end.should == 7
  end

  specify "instance_eval with a block should have access to receiver's instance variables" do
    example do
      class Klass
        def initialize
          @secret = 99
        end
      end
      
      @k = Klass.new
      @k.instance_eval { @secret }
    end.should == 99
  end
  
  specify "instance_eval with string argument should execute on the receiver context" do
    example do
      "hola".instance_eval("size")
    end.should == 4
  end

  specify "instance_eval with string argument should bind self to the receiver" do
    example do
      @o = Object.new
      @o == @o.instance_eval("self")
    end.should == true
  end

  specify "instance_eval with string argument should have access to receiver's instance variables" do
    example do
      class Klass
        def initialize
          @secret = 99
        end
      end
      
      @k = Klass.new
      @k.instance_eval("@secret")
    end.should == 99
  end
  
  specify "instance_variable_get should return the value of the instance variable" do
    example do
      class Fred 
      def initialize(p1, p2) 
      @a, @b = p1, p2 
      end 
      end 
      @fred = Fred.new('cat', 99) 
      [ @fred.instance_variable_get(:@a),
        @fred.instance_variable_get("@b") ]
    end.should == ["cat", 99]
  end
  
  specify "instance_variable_get should raise NameError exception if the argument is not of form '@x'" do
    example do
      class NoFred
      end
      try(NameError) { NoFred.new.instance_variable_get(:c) }
    end.should == true
  end

  specify "instance_variable_set should set the value of the specified instance variable" do
    example do
      class Dog
        def initialize(p1, p2) 
          @a, @b = p1, p2 
        end 
      end 
      @dog = Dog.new('cat', 99) 
      @dog.instance_variable_set(:@a, 'dog') 
    end.should == "dog"
  end
  
  specify "instance_variable_set should set the value of the instance variable when no instance variables exist yet" do
    example do
      class NoVariables; end
      NoVariables.new.instance_variable_set(:@a, "new")
    end.should == "new"
  end
  
  specify "instance_variable_set should raise NameError exception if the argument is not of form '@x'" do
    example do
      class NoDog
      end
      try(NameError) { NoDog.new.instance_variable_set(:c, "cat") }
    end.should == true
  end
end
