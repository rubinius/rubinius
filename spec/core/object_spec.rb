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

module ObjectSpecs
  class A 
    def public_method; :public_method; end
    protected
    def protected_method; :protected_method; end
    private
    def private_method; :private_method; end
  end
end

context "Object class method" do
  specify "new should create a new Object" do
    Object.new.class.should == Object
  end
end

context "Object instance method" do
  specify "send should invoke the named method" do
    class ObjectSpecs::Foo
      def bar
        'done'
      end
    end
    ObjectSpecs::Foo.new.send(:bar).should == 'done'
  end

  specify "send should invoke a class method if called on a class" do
    class ObjectSpecs::Foo
      def self.bar
        'done'
      end
    end
    ObjectSpecs::Foo.send(:bar).should == 'done'
  end

  specify "send should raise NoMethodError if the corresponding method can't be found" do
    class ObjectSpecs::Foo
      def bar
        'done'
      end
    end
    should_raise(NoMethodError) { ObjectSpecs::Foo.new.send(:baz) }
  end

  specify "send should raise NoMethodError if the corresponding singleton method can't be found" do
    class ObjectSpecs::Foo
      def self.bar
        'done'
      end
    end
    should_raise(NoMethodError) { ObjectSpecs::Foo.send(:baz) }
  end
  
  specify "freeze should prevent self from being further modified" do
    module Mod; end
    o = Object.new
    o.freeze
    should_raise(TypeError) { o.extend Mod }
  end
  
  specify "freeze should have no effect on immediate values" do
    a = nil
    b = true
    c = false
    d = 1
    a.freeze
    b.freeze
    c.freeze
    d.freeze
    a.frozen?.should == false
    b.frozen?.should == false
    c.frozen?.should == false
    d.frozen?.should == false
  end
  
  specify "frozen? should return true if self is frozen" do
    o = Object.new
    p = Object.new
    p.freeze
    o.frozen?.should == false
    p.frozen?.should == true
  end
  
  specify "taint should set self to be tainted" do
    Object.new.taint.tainted?.should == true
  end
  
  specify "taint should have no effect on immediate values" do
    a = nil
    b = true
    c = false
    d = 1
    a.tainted?.should == false
    b.tainted?.should == false
    c.tainted?.should == false
    d.tainted?.should == false
  end
  
  specify "tainted? should return true if Object is tainted" do
    o = Object.new
    p = Object.new
    p.taint
    o.tainted?.should == false
    p.tainted?.should == true
  end
  
  specify "instance_eval with no arguments should expect a block" do
    should_raise(ArgumentError) { "hola".instance_eval }
  end
  
  specify "instance_eval with a block should take no arguments" do
    should_raise(ArgumentError) { "hola".instance_eval(4, 5) { |a,b| a + b } }
  end
  
  specify "instance_eval with a block should pass the object to the block" do
    "hola".instance_eval { |o| o.size }.should == 4
  end
  
  specify "instance_eval with a block should bind self to the receiver" do
    s = "hola"
    (s == s.instance_eval { self }).should == true
  end
  
  specify "instance_eval with a block should execute in the context of the receiver" do
    "Ruby-fu".instance_eval { size }.should == 7
  end

  specify "instance_eval with a block should have access to receiver's instance variables" do
    class Klass
      def initialize
        @secret = 99
      end
    end
    Klass.new.instance_eval { @secret }.should == 99
  end
  
  specify "instance_eval with string argument should execute on the receiver context" do
    "hola".instance_eval("size").should == 4
  end

  specify "instance_eval with string argument should bind self to the receiver" do
    o = Object.new
    (o == o.instance_eval("self")).should == true
  end

  specify "instance_eval with string argument should have access to receiver's instance variables" do
    class Klass
      def initialize
        @secret = 99
      end
    end
    Klass.new.instance_eval("@secret").should == 99
  end
  
  specify "metaclass should return the object's metaclass" do
    foo = "foo"
    foo.instance_eval "class << self; def meta_test_method; 5; end; end"
    foo.respond_to?(:meta_test_method).should == true
    should_raise(NameError) { "hello".metaclass.method(:meta_test_method) }
  end

  specify "method should return a method object for a valid method" do
    class ObjectSpecs::Foo; def bar; 'done'; end; end
    ObjectSpecs::Foo.new.method(:bar).class.should == Method
  end

  specify "method should return a method object for a valid singleton method" do
    class ObjectSpecs::Foo; def self.bar; 'done'; end; end
    ObjectSpecs::Foo.method(:bar).class.should == Method
  end
 
  specify "method should raise a NameError for an invalid method name" do
    class ObjectSpecs::Foo; def bar; 'done'; end; end
    should_raise(NameError) { ObjectSpecs::Foo.new.method(:baz) }
  end

  specify "method should raise a NameError for an invalid singleton method name" do
    class ObjectSpecs::Foo; def self.bar; 'done'; end; end
    should_raise(NameError) { ObjectSpecs::Foo.method(:baz) }
  end

  specify "respond_to? should indicate if an object responds to a particular message" do
    class ObjectSpecs::Foo; def bar; 'done'; end; end
    ObjectSpecs::Foo.new.respond_to?(:bar).should == true
    ObjectSpecs::Foo.new.respond_to?(:baz).should == false
  end

  specify "respond_to? should indicate if a singleton object responds to a particular message" do
    class ObjectSpecs::Foo; def self.bar; 'done'; end; end
    ObjectSpecs::Foo.respond_to?(:bar).should == true
    ObjectSpecs::Foo.respond_to?(:baz).should == false
  end
end

describe "Object#instance_variable_get" do
  it "should raise ArgumentError if the instance variable name is a Fixnum" do
    should_raise(ArgumentError) { "".instance_variable_get(1) }
  end
  
  it "should raise TypeError if the instance variable name is an object that does not respond to to_str" do
    class ObjectSpecs::A; end
    should_raise(TypeError) { "".instance_variable_get(ObjectSpecs::A.new) }
  end
  
  it "should raise NameError if the passed object, when coerced with to_str, does not start with @" do
    class ObjectSpecs::B
      def to_str
        ":c"
      end
    end
    should_raise(NameError) { "".instance_variable_get(ObjectSpecs::B.new) }
  end
  
  it "should raise NameError if pass an object that cannot be a symbol" do
    should_raise(NameError) { "".instance_variable_get(:c) }
  end
  
  it "should accept as instance variable name any instance of a class that responds to to_str" do
    class ObjectSpecs::C
      def initialize
        @a = 1
      end
      def to_str
        "@a"
      end
    end
    ObjectSpecs::C.new.instance_variable_get(ObjectSpecs::C.new).should == 1
  end
end

describe "Object#instance_variable_set" do
  it "should raise ArgumentError if the instance variable name is a Fixnum" do
    should_raise(ArgumentError) { "".instance_variable_set(1, 2) }
  end
  
  it "should raise TypeError if the instance variable name is an object that does not respond to to_str" do
    class ObjectSpecs::A; end
    should_raise(TypeError) { "".instance_variable_set(ObjectSpecs::A.new, 3) }
  end
  
  it "should raise NameError if the passed object, when coerced with to_str, does not start with @" do
    class ObjectSpecs::B
      def to_str
        ":c"
      end
    end
    should_raise(NameError) { "".instance_variable_set(ObjectSpecs::B.new, 4) }
  end
  
  it "should raise NameError if pass an object that cannot be a symbol" do
    should_raise(NameError) { "".instance_variable_set(:c, 1) }
  end
  
  it "should accept as instance variable name any instance of a class that responds to to_str" do
    class ObjectSpecs::C
      def initialize
        @a = 1
      end
      def to_str
        "@a"
      end
    end
    ObjectSpecs::C.new.instance_variable_set(ObjectSpecs::C.new, 2).should == 2
  end
end

describe "Object#method_missing" do
  class ObjectSpecs::A
    def method_missing (*args)
      "a_new_method_missing"
    end
  end 
   
  it  "return the correct value form method_missing after call a not defined instance method" do
    ObjectSpecs::A.new.foo.should == "a_new_method_missing"
  end
  
  it "raise a NoMethodError Exceptiong after call a not defined Class method even if te the method_missing its defined" do  
    should_raise(NoMethodError){ObjectSpecs::A.foo.should == "a_new_method_missing"} 
  end    
   
  class ObjectSpecs::B;end   
  
  it "raise a NoMethodError Exceptiong after call a not defined instance method" do
    should_raise(NoMethodError){ObjectSpecs::B.new.foo}
  end

  
  it  "raise a NoMethodError exception after call a non defined method " do
    should_raise(NoMethodError){some_method "a", 1} 
  end    
  
  class ObjectSpecs::C
    def self.method_missing (*args)
      "a_new_method_missing"
    end
  end
    
  it  "return the correct value form method_missing after call a not defined class method" do
    ObjectSpecs::C.foo.should == "a_new_method_missing"
  end
  
  it  "raise a NoMethodError Exceptiong after call a not defined instance method" do 
    should_raise(NoMethodError){ObjectSpecs::C.new.foo.should == "a_new_method_missing"} 
  end
   

  class ObjectSpecs::AParent
    def method_missing (*args)
      "a_new_method_missing"
    end
  end
    
  class ObjectSpecs::AChild < ObjectSpecs::AParent
    def foo
      super
    end
  end    
    
  it  "return the correct value form method_missing after call a not defined instance method" do
    ObjectSpecs::AChild.new.foo.should == "a_new_method_missing"
  end
   
  class ObjectSpecs::ProtectedAndPrivate
    private
    def a_private_method;  end 
    protected
    def a_protected_method; end
  end
    
  class ObjectSpecs::ProtectedAndMissing 
    def method_missing(*args)
      :a_new_method_missing
    end
    protected 
    def self.a_protected_class_method
      :a_protected_class_method
    end
  end
    
  class ObjectSpecs::PrivateAndMissing
    def method_missing(*args)
      :a_new_method_missing
    end    
    private 
    def self.a_private_class_method
      :a_private_class_method
    end 
  end 
   
  it  "raise a NoMethodError exception after call a non defined method in a class with proctected and private methods" do
    should_raise(NoMethodError){ObjectSpecs::ProtectedAndPrivate.new.a_missing_method}
  end  
  
  it  "raise a not a NoMethodError and message should be a exception after call a private methor defined in a class " do
    should_raise(NoMethodError){ObjectSpecs::ProtectedAndPrivate.new.a_private_method} 
  end
  
  it  "raise a not a NoMethodError and message should be a exception after call a protected methor defined in a class " do
    should_raise(NoMethodError){ObjectSpecs::ProtectedAndPrivate.new.a_protected_method}
  end
  
  it  "return the correct value from the method_missing method after call a private instance method" do
    ObjectSpecs::PrivateAndMissing.new.a_private_method.should == :a_new_method_missing
  end
  
  it  "return the correct value from the method_missing method after call a protected instance method" do
    ObjectSpecs::ProtectedAndMissing.new.a_protected_method.should == :a_new_method_missing
  end
  
  it  "return the correct value from the method_missing method after call a private class method" do
   ObjectSpecs::PrivateAndMissing.a_private_class_method.should == :a_private_class_method
  end
    
  it  "return the correct value from the method_missing method after call a protected class method" do
    ObjectSpecs::ProtectedAndMissing.a_protected_class_method.should == :a_protected_class_method
  end 
end

describe "Object#==" do 
  it "returns true only if obj and other are the same object" do
    o1 = Object.new
    o2 = Object.new
    (o1 == o1).should == true
    (o2 == o2).should == true
    (o1 == o2).should== false
    (nil == nil).should == true
    (o1 == nil).should== false
    (nil == o2).should== false
  end
   
end

describe "Object#eql?" do 
  it "returns true if obj and anObject are the same object." do 
    o1 = Object.new
    o2 = Object.new
    (o1.eql?(o1)).should == true
    (o2.eql?(o2)).should == true
    (o1.eql?(o2)).should == false
  end
  
  it "returns true if obj and anObject have the same value." do 
    o1 = 1
    o2 = :hola
    (:hola.eql? o1).should == false
    (1.eql? o1).should == true
    (:hola.eql? o2).should == true
  end
end

describe "Object#===" do 
  
end

describe "Object#equal?" do   
  it "returns true only if obj and other are the same object" do
    o1 = Object.new
    o2 = Object.new
    (o1.equal? o1).should == true
    (o2.equal? o2).should == true
    (o1.equal? o2).should== false
    (nil.equal? nil).should == true
    (o1.equal?  nil).should== false
    (nil.equal?  o2).should== false
  end
  
  
  it "returns true if obj and anObject have the same value." do 
    o1 = 1
    o2 = :hola
    (:hola.equal? o1).should == false
    (1.equal? o1).should == true
    (:hola.equal? o2).should == true
  end
end

generic_entries_object_id = shared "Object#__id__object_id" do |cmd|
  describe "Enumerable##{cmd}" do     
    # #object_id and #__id__ are aliases, so we only need one function
    # that tests both methods
    it "return an Integer" do
      Object.new.send(cmd).class.should == Fixnum      
      nil.send(cmd).class.should == Fixnum      
    end
  
    it "return same number will be returned on all calls to id for a given object" do
      o1 = Object.new
      o2 = Object.new
      o1.send(cmd).should == o1.send(cmd)
    end
  
    it "return same number will be returned on all calls to id for a given object" do
      o1 = Object.new
      o2 = Object.new
      o1.send(cmd).should_not == o2.send(cmd)
    end
    it "" do
    end
    
  end
end

describe "Obejct.__id__" do
  it_behaves_like(generic_entries_object_id, :__id__) 
end

describe "Obejct.object_id" do
  it_behaves_like(generic_entries_object_id, :object_id) 
end

describe "Object.=~" do
  it "Overridden by descendents" do
    o1 = Object.new
    o2 = Object.new
    (o1 =~ o1).should == false
    (o2 =~ o2).should == false
    (o1 !~ o2).should == true
    (o1 =~ o2).should == false
  end
end

generic_entries_kind_of = shared "Object#is_a_kind_of" do |cmd|
  describe "Enumerable##{cmd}" do     
    # #kind_of? and #is_a are aliases, so we only need one function
    # that tests both methods  
    
    it "returns true if class is the class of obj, or if class is one of the superclasses of obj or modules included in obj (String example)" do    
      s = "hello"
      s.send(cmd, String).should == true
      s.send(cmd, Object).should == true
      s.send(cmd, Class).should == false 
    end

    it "returns true if class is the class of obj, or if class is one of the superclasses of obj or modules included in obj Array example" do
      a = []
      a.send(cmd, Array).should == true
      a.send(cmd, Enumerable).should == true
    end    
          
    module ObjectSpecs::M;    end
    class ObjectSpecs::X; include ObjectSpecs::M; end
    class ObjectSpecs::Y < ObjectSpecs::X; end
    class ObjectSpecs::Z < ObjectSpecs::Y; end 
  
    it "returns true if class is the class of obj, or if class is one of the superclasses of obj or modules included in obj Custom class example" do     
      y = ObjectSpecs::Y.new
      y.send(cmd, ObjectSpecs::X).should == true       
      y.send(cmd, ObjectSpecs::Y).should == true       
      y.send(cmd, ObjectSpecs::Z).should == false       
      y.send(cmd, ObjectSpecs::M).should == true
    end
    
    it "nil.#{cmd} cases specs"  do 
      (nil.send(cmd,  NilClass)).should == true
      (nil.send(cmd, Object)).should == true
      (nil.send(cmd, Module)).should == false
      (nil.send(cmd, Kernel)).should == true
      
    end
  end 
end

describe "Obejct.is_a?" do
  it_behaves_like(generic_entries_kind_of , :is_a?) 
end

describe "Obejct.kind_of?" do
  it_behaves_like(generic_entries_kind_of , :kind_of?) 
end

describe "Object#to_s" do
  it "match the  /^#<Object:0x[0-9a-f]+>/ format" do
    o = Object.new
    o.to_s.match(/^#<Object:0x[0-9a-f]+>/).should_not == nil
  end
end

describe "Object#respond_to?" do
  before :each do 
    @a = ObjectSpecs::A.new  
  end
   
  it "returns true if obj responds to the given public method" do    
    @a.respond_to?("five").should == false
    @a.respond_to?(:public_method).should == true
    @a.respond_to?("public_method").should == true
  end
  
  it "returns true if obj responds to the given protected method" do
    @a.respond_to?("five", true).should == false
    @a.respond_to?(:protected_method, false).should == true
    @a.respond_to?("protected_method", false).should == true
  end
  
  it "returns true if obj responds to the given protected method, include_private = true" do 
    @a.respond_to?("seven").should == false
    @a.respond_to?(:protected_method).should == true
    @a.respond_to?("protected_method").should == true
  end
  
  it "returns true if obj responds to the given protected method" do
    @a.respond_to?("seven", true).should == false
    @a.respond_to?(:protected_method, false).should == true
    @a.respond_to?("protected_method", false).should == true
  end
  
  it "returns true if obj responds to the given private method, include_private = true" do
    @a.respond_to?("six").should == false
    @a.respond_to?(:private_method).should == false
    @a.respond_to?("private_method").should == false
  end
  
  it "returns true if obj responds to the given private method" do    
    @a.respond_to?("six", true).should == false
    @a.respond_to?(:private_method, true).should == true    
    @a.respond_to?("private_method", true).should == true
  end 
end

# describe "objects#methods" do
#   it "return all the methods of a custom class" do
#     A.new.methods.should ==   ["inspect", "send", "clone", "should_be_ancestor_of", 
#                                "should", "public_methods", "should_not_receive",
#                                "__send__", "equal?", "freeze", "should_include", 
#                                "protected_method", "methods", "instance_eval", 
#                                "display", "dup", "object_id", "instance_variables",
#                                 "extend", "instance_of?", "eql?", "hash", "id", 
#                                 "singleton_methods", "taint", "frozen?", 
#                                 "instance_variable_get", "kind_of?", "to_a", 
#                                 "type", "should_be_close", "protected_methods", 
#                                 "==", "method_missing", "===", "instance_variable_set",
#                                 "is_a?", "respond_to?", "to_s", "class", "tainted?", "=~",
#                                 "private_methods", "__id__", "nil?", "should_not", "untaint",
#                                 "should_receive", "method", "public_method"] 
#     A.new.methods.size.should == 50
#   end
#   
#   it "return all the methods of Object.new" do          
#     Object.new.methods.size.should == 47
#   end  
#   
#   it "return all the methods of Object" do    
#     Object.methods.size.should == 80 
#   end
# end
# 
# describe "objects#private_methods" do
#   it "return all the methods of a custom class" do
#     A.new.private_methods.should == ["Integer", "initialize", "p", "singleton_method_removed", 
#                                      "chomp", "fail", "exec", "syscall", "callcc",
#                                       "sub!", "load", "proc", "it_behaves_like",
#                                       "iterator?", "catch", "puts", "it","Float", 
#                                       "singleton_method_undefined", "split", "caller",
#                                       "system", "require", "open", "gsub!", "lambda", 
#                                       "try", "block_given?", "throw", "gets", "private_method",
#                                       "sub", "loop", "trap", "String", "fork",
#                                       "initialize_copy", "sprintf", "shared", "setup", 
#                                       "exit", "sleep", "printf", "chop!", "autoload", 
#                                       "scan", "failure", "trace_var", "select", 
#                                       "global_variables", "should_output", "readline", 
#                                       "warn", "`", "gsub", "context", "exit!", "before", 
#                                       "Array", "format", "teardown", "abort", "chomp!", 
#                                       "print", "eval", "srand", "engine?", "noncompliant",
#                                       "untrace_var", "local_variables", "readlines",
#                                       "singleton_method_added", "specify", "chop", 
#                                       "raise", "getc", "autoload?", "binding", "compliant", 
#                                       "should_raise", "at_exit", "describe", "putc", 
#                                       "remove_instance_variable", "set_trace_func", 
#                                       "after", "extension", "rand", "test"] 
#     A.new.private_methods.size.should == 89 
#   end
#   
#   it "return all the methods of Object.new" do          
#     Object.new.private_methods.size.should == 89
#   end  
#   
#   it "return all the methods of Object" do    
#     Object.private_methods.size.should == 109
#   end
#   
#   it "return all the methods of a custom class, all = false" do
#     A.new.private_methods(false).should ==  ["private_method"]   
#     A.new.private_methods(false).size.should == 1
#   end
#   
#   it "return all the methods of Object.new, all = false" do          
#     Object.new.private_methods(false).size.should == 19
#   end  
#   
#   it "return all the methods of Object, all = false" do    
#     Object.private_methods(false).size.should == 3 
#   end
# end
# 
# 
# describe "objects#protected_methods" do
#   it "return all the methods of a custom class" do
#     A.new.protected_methods.should == ["protected_method"] 
#     A.new.protected_methods.size.should == 1
#   end
#   
#   it "return all the methods of Object.new" do          
#     Object.new.protected_methods.size.should == 0
#   end  
#   
#   it "return all the methods of Object" do    
#     Object.protected_methods.size.should == 0
#   end
#   
#   it "return all the methods of a custom class, all = false" do
#     A.new.protected_methods(false).should ==  ["protected_method"]   
#     A.new.protected_methods(false).size.should == 1
#   end
#   
#   it "return all the methods of Object.new, all = false" do          
#     Object.new.protected_methods(false).size.should == 0
#   end  
#   
#   it "return all the methods of Object, all = false" do    
#     Object.protected_methods(false).size.should == 0 
#   end
# end
# 
# 
# describe "objects#public_methods" do
#   it "return all the methods of a custom class" do
#     A.new.public_methods.should ==  ["inspect", "send", "clone", 
#                                      "should_be_ancestor_of", "should", "public_methods", 
#                                      "should_not_receive", "__send__", "equal?", 
#                                      "freeze", "should_include", "methods", 
#                                      "instance_eval", "display", "dup", "object_id", 
#                                      "instance_variables", "extend", "instance_of?", "eql?",
#                                       "hash", "id", "singleton_methods", "taint", "frozen?", 
#                                       "instance_variable_get", "kind_of?", "to_a", "type",
#                                       "should_be_close", "protected_methods", "==", "method_missing", 
#                                       "===", "instance_variable_set", "is_a?", 
#                                       "respond_to?", "to_s", "class", "tainted?", 
#                                       "=~", "private_methods", "__id__", "nil?", "should_not",
#                                       "untaint", "should_receive", "method", "public_method"]
#     A.new.public_methods.size.should == 49
#   end
#   
#   it "return all the methods of Object.new" do          
#     Object.new.public_methods.size.should == 47
#   end  
#   
#   it "return all the methods of Object" do    
#     Object.public_methods.size.should == 80
#   end
#   
#   it "return all the methods of a custom class, all = false" do
#     A.new.public_methods(false).should ==  ["method_missing", "public_method"]   
#     A.new.public_methods(false).size.should == 2
#   end
#   
#   it "return all the methods of Object.new, all = false" do          
#     Object.new.public_methods(false).size.should == 7
#   end  
#   
#   it "return all the methods of Object, all = false" do    
#     Object.public_methods(false).size.should == 3
#   end
# end
