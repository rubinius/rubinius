require File.dirname(__FILE__) + '/../spec_helper'

module ModuleSpecs
  class Parent
    def public_parent() end
    protected
      def protected_parent() end
    private
      def private_parent() end
  end
    
  module BasicModule
    def public_module() end
    protected
      def protected_module() end
    private
      def private_module() end
  end

  module SuperModule
    # include BasicModule
    
    def public_super_module() end
    protected
      def protected_super_module() end
    private
      def private_super_module() end
  end
    
  class Child < Parent
    # include SuperModule
    
    def public_child() end
    protected
      def protected_child() end
    private
      def private_child() end
  end
end


describe "Module.constants" do
  it "returns an array of the names of all constants defined" do
    a = Module.constants.size
    ABC = ""
    b = Module.constants.size.should == a + 1
  end
end

describe "Module.nesting" do
  it "returns the list of Modules nested at the point of call" do
    module ModuleSpecs
      Module.nesting.should == [ModuleSpecs]

      module BasicModule
        Module.nesting.should == [ModuleSpecs::BasicModule, ModuleSpecs]
      end

      class Parent
        Module.nesting.should == [ModuleSpecs::Parent, ModuleSpecs]
      end
    end
  end
end

describe "Module.new(&block)" do
  it "creates a new anonymous Module" do
    Module.new.is_a?(Module).should == true
  end
  
  failure :rbx do
    it "evaluates a passed block in the context of the module" do
      fred = Module.new do
        def hello() "hello" end
        def bye()   "bye"   end
      end
      
      (o = Object.new).extend(fred)
      o.hello.should == "hello"
      o.bye.should   == "bye"
    end
  end
end

describe "Module#<(Module)" do
  it "returns true if self is a subclass of or includes the given module" do
    (ModuleSpecs::Child       < ModuleSpecs::Parent).should == true
    (ModuleSpecs::Child       < ModuleSpecs::BasicModule).should == true
    (ModuleSpecs::Child       < ModuleSpecs::SuperModule).should == true
    (ModuleSpecs::SuperModule < ModuleSpecs::BasicModule).should == true
  end
  
  it "returns false if self is the same as the given module" do
    (ModuleSpecs::Child       < ModuleSpecs::Child).should == false
    (ModuleSpecs::Parent      < ModuleSpecs::Parent).should == false
    (ModuleSpecs::BasicModule < ModuleSpecs::BasicModule).should == false
    (ModuleSpecs::SuperModule < ModuleSpecs::SuperModule).should == false
  end
  
  it "returns nil if self is not related to the given module" do
    (ModuleSpecs::Parent      < ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      < ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule < ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule < ModuleSpecs::Parent).should == nil
  end
  
  it "raises a TypeError the argument is no class/module" do
    should_raise(TypeError, "compared with non class/module") do
      ModuleSpecs::Parent     < Object.new
    end
  end
end

describe "Module#<=(Module)" do
  it "returns true if self is a subclass of, the same as or includes the given module" do
    (ModuleSpecs::Child       <= ModuleSpecs::Parent).should == true
    (ModuleSpecs::Child       <= ModuleSpecs::BasicModule).should == true
    (ModuleSpecs::Child       <= ModuleSpecs::SuperModule).should == true
    (ModuleSpecs::SuperModule <= ModuleSpecs::BasicModule).should == true
    (ModuleSpecs::Child       <= ModuleSpecs::Child).should == true
    (ModuleSpecs::Parent      <= ModuleSpecs::Parent).should == true
    (ModuleSpecs::BasicModule <= ModuleSpecs::BasicModule).should == true
    (ModuleSpecs::SuperModule <= ModuleSpecs::SuperModule).should == true
  end
  
  it "returns nil if self is not related to the given module" do
    (ModuleSpecs::Parent      <= ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      <= ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule <= ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule <= ModuleSpecs::Parent).should == nil
  end
  
  it "returns false if self is a superclass of or is included by the given module" do
    (ModuleSpecs::Parent      <= ModuleSpecs::Child).should == false
    (ModuleSpecs::BasicModule <= ModuleSpecs::Child).should == false
    (ModuleSpecs::SuperModule <= ModuleSpecs::Child).should == false
    (ModuleSpecs::BasicModule <= ModuleSpecs::SuperModule).should == false
  end

  it "raises a TypeError if the argument is no class/module" do
    should_raise(TypeError, "compared with non class/module") do
      ModuleSpecs::Parent     <= Object.new
    end
  end
end

describe "Module#<=>(Module)" do
  it "returns -1 if self is a subclass of or includes the given module" do
    (ModuleSpecs::Child       <=> ModuleSpecs::Parent).should == -1
    (ModuleSpecs::Child       <=> ModuleSpecs::BasicModule).should == -1
    (ModuleSpecs::Child       <=> ModuleSpecs::SuperModule).should == -1
    (ModuleSpecs::SuperModule <=> ModuleSpecs::BasicModule).should == -1
  end
  
  it "returns 0 if self is the same as the given module" do
    (ModuleSpecs::Child       <=> ModuleSpecs::Child).should == 0
    (ModuleSpecs::Parent      <=> ModuleSpecs::Parent).should == 0
    (ModuleSpecs::BasicModule <=> ModuleSpecs::BasicModule).should == 0
    (ModuleSpecs::SuperModule <=> ModuleSpecs::SuperModule).should == 0
  end
  
  it "returns +1 if self is a superclas of or included by the given module" do
    (ModuleSpecs::Parent      <=> ModuleSpecs::Child).should == +1
    (ModuleSpecs::BasicModule <=> ModuleSpecs::Child).should == +1
    (ModuleSpecs::SuperModule <=> ModuleSpecs::Child).should == +1
    (ModuleSpecs::BasicModule <=> ModuleSpecs::SuperModule).should == +1
  end
  
  it "returns nil if self and the given module are not related" do
    (ModuleSpecs::Parent      <=> ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      <=> ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule <=> ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule <=> ModuleSpecs::Parent).should == nil
  end
  
  it "returns nil if the argument is no class/module" do
    (ModuleSpecs::Parent      <=> Object.new).should == nil
  end
end


module_equal = shared "Module#==(other) | Module#eql?(other) | Module#equal?(other)" do |cmd|
  describe "Module##{cmd}(other)" do
    it "returns true if self and the given module are the same" do
      ModuleSpecs.send(cmd, ModuleSpecs).should == true
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Child).should == true
      ModuleSpecs::Parent.send(cmd, ModuleSpecs::Parent).should == true
      ModuleSpecs::BasicModule.send(cmd, ModuleSpecs::BasicModule).should == true
      ModuleSpecs::SuperModule.send(cmd, ModuleSpecs::SuperModule).should == true
      
      ModuleSpecs::Child.send(cmd, ModuleSpecs).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::Parent).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::BasicModule).should == false
      ModuleSpecs::Child.send(cmd, ModuleSpecs::SuperModule).should == false
    end
  end
end

describe "Module#==(Module)" do
  it_behaves_like(module_equal, :==)
end

describe "Module#eql?(Module)" do
  it_behaves_like(module_equal, :eql?)
end

describe "Module#equal?(Module)" do
  it_behaves_like(module_equal, :equal?)
end

describe "Module#===(Object)" do
  it "returns true when the given Object is an instance of self or of self's descendants" do
    (ModuleSpecs::Child       === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::Parent      === ModuleSpecs::Parent.new).should == true
    
    (ModuleSpecs::Parent      === ModuleSpecs::Child.new).should == true
    (Object                   === ModuleSpecs::Child.new).should == true

    (ModuleSpecs::Child       === String.new).should == false
    (ModuleSpecs::Child       === Object.new).should == false
  end
  
  it "returns true when the given Object's class includes self or when the given Object is extended by self" do
    (ModuleSpecs::BasicModule === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::SuperModule === ModuleSpecs::Child.new).should == true
    (ModuleSpecs::BasicModule === Object.new.extend(ModuleSpecs::SuperModule)).should == true
    (ModuleSpecs::SuperModule === Object.new.extend(ModuleSpecs::SuperModule)).should == true

    (ModuleSpecs::BasicModule === ModuleSpecs::Parent.new).should == false
    (ModuleSpecs::SuperModule === ModuleSpecs::Parent.new).should == false
    (ModuleSpecs::BasicModule === Object.new).should == false
    (ModuleSpecs::SuperModule === Object.new).should == false
  end
end

describe "Module#>(Module)" do
  it "returns true if self is a superclass of or included by the given module" do
    (ModuleSpecs::Parent      > ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule > ModuleSpecs::Child).should == true
    (ModuleSpecs::SuperModule > ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule > ModuleSpecs::SuperModule).should == true
  end
  
  it "returns false if self is the same as the given module" do
    (ModuleSpecs::Child       > ModuleSpecs::Child).should == false
    (ModuleSpecs::Parent      > ModuleSpecs::Parent).should == false
    (ModuleSpecs::BasicModule > ModuleSpecs::BasicModule).should == false
    (ModuleSpecs::SuperModule > ModuleSpecs::SuperModule).should == false
  end
  
  it "returns nil if self is not related to the given module" do
    (ModuleSpecs::Parent      > ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      > ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule > ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule > ModuleSpecs::Parent).should == nil
  end
  
  it "raises a TypeError the argument is no class/module" do
    should_raise(TypeError, "compared with non class/module") do
      ModuleSpecs::Parent     > Object.new
    end
  end
end

describe "Module#>=(Module)" do
  it "returns true if self is a superclass of, the same as or included by given module" do
    (ModuleSpecs::Parent      >= ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule >= ModuleSpecs::Child).should == true
    (ModuleSpecs::SuperModule >= ModuleSpecs::Child).should == true
    (ModuleSpecs::BasicModule >= ModuleSpecs::SuperModule).should == true
    (ModuleSpecs::Child       >= ModuleSpecs::Child).should == true
    (ModuleSpecs::Parent      >= ModuleSpecs::Parent).should == true
    (ModuleSpecs::BasicModule >= ModuleSpecs::BasicModule).should == true
    (ModuleSpecs::SuperModule >= ModuleSpecs::SuperModule).should == true
  end
  
  it "returns nil if self is not related to the given module" do
    (ModuleSpecs::Parent      >= ModuleSpecs::BasicModule).should == nil
    (ModuleSpecs::Parent      >= ModuleSpecs::SuperModule).should == nil
    (ModuleSpecs::BasicModule >= ModuleSpecs::Parent).should == nil
    (ModuleSpecs::SuperModule >= ModuleSpecs::Parent).should == nil
  end
  
  it "returns false if self is a subclass of or includes the given module" do
    (ModuleSpecs::Child       >= ModuleSpecs::Parent).should == false
    (ModuleSpecs::Child       >= ModuleSpecs::BasicModule).should == false
    (ModuleSpecs::Child       >= ModuleSpecs::SuperModule).should == false
    (ModuleSpecs::SuperModule >= ModuleSpecs::BasicModule).should == false
  end

  it "raises a TypeError if the argument is no class/module" do
    should_raise(TypeError, "compared with non class/module") do
      ModuleSpecs::Parent     >= Object.new
    end
  end
end

describe "Module#ancestors" do
  it "returns a list of modules included in self (including self)" do
    ModuleSpecs.ancestors.should == [ModuleSpecs]
    ModuleSpecs::BasicModule.ancestors.should == [ModuleSpecs::BasicModule]
    ModuleSpecs::SuperModule.ancestors.should == [ModuleSpecs::SuperModule, ModuleSpecs::BasicModule]
    ModuleSpecs::Parent.ancestors.should == [ModuleSpecs::Parent, Object, Kernel]
    ModuleSpecs::Child.ancestors.should == [ModuleSpecs::Child, ModuleSpecs::SuperModule, ModuleSpecs::BasicModule, ModuleSpecs::Parent, Object, Kernel]
  end
end

describe "Module#append_features(mod)" do
  it "gets called when self is included in another module/class" do
    begin
      m = Module.new do
        def self.append_features(mod)
          $appended_to = mod
        end
      end
      
      c = Class.new do
        include m
      end
      
      $appended_to.should == c
    ensure
      $appended_to = nil
    end
  end
end

describe "Module#attr(name, writable)" do
  it "creates a getter for the given attribute name" do
    c = Class.new do
      attr :attr
      attr :attr2.to_i
      attr "attr3"
      
      def initialize
        @attr, @attr2, @attr3 = "test", "test2", "test3"
      end
    end
    
    o = c.new
    
    %w{attr attr2 attr3}.each do |a|
      o.respond_to?(a).should == true
      o.respond_to?("#{a}=").should == false
    end

    o.attr.should  == "test"
    o.attr2.should == "test2"
    o.attr3.should == "test3"
  end
  
  it "creates a setter for the given attribute name if writable is true" do
    c = Class.new do
      attr :attr, true
      attr :attr2.to_i, true
      attr "attr3", true
      
      def initialize
        @attr, @attr2, @attr3 = "test", "test2", "test3"
      end
    end
    
    o = c.new
    
    %w{attr attr2 attr3}.each do |a|
      o.respond_to?(a).should == true
      o.respond_to?("#{a}=").should == true
    end
    
    o.attr.should == "test"
    o.attr = "test updated"
    o.attr.should == "test updated"

    o.attr2.should == "test2"
    o.attr2 = "test2 updated"
    o.attr2.should == "test2 updated"

    o.attr3.should == "test3"
    o.attr3 = "test3 updated"
    o.attr3.should == "test3 updated"
  end
  
  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "test")
    Class.new { attr o }.new.respond_to?("test").should == true
  end
  
  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
      Class.new { attr o }
    end
    
    (o = Object.new).should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      Class.new { attr o }
    end
  end
end

describe "Module#attr_accessor(name, ...)" do
  it "creates a getter and setter for each given attribute name" do
    c = Class.new do
      attr_accessor :a, :b.to_i, "c"
    end
    
    o = c.new
    
    ('a'..'c').each do |x|
      o.respond_to?(x).should == true
      o.respond_to?("#{x}=").should == true
    end
    
    o.a = "a"
    o.a.should == "a"
    
    o.b = "b"
    o.b.should == "b"
    
    o.c = "c"
    o.c.should == "c"
  end
  
  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "test")
    c = Class.new do
      attr_accessor o
    end
    
    c.new.respond_to?("test").should == true
    c.new.respond_to?("test=").should == true
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
      Class.new { attr_accessor o }
    end
    
    (o = Object.new).should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      Class.new { attr_accessor o }
    end
  end
end

describe "Module#attr_reader(name, ...)" do
  it "creates a getter for each given attribute name" do
    c = Class.new do
      attr_reader :a, :b.to_i, "c"
      
      def initialize
        @a = "test"
        @b = "test2"
        @c = "test3"
      end
    end
    
    o = c.new
    ('a'..'c').each do |x|
      o.respond_to?(x).should == true
      o.respond_to?("#{x}=").should == false
    end
    
    o.a.should == "test"
    o.b.should == "test2"
    o.c.should == "test3"
  end

  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "test")
    c = Class.new do
      attr_reader o
    end
    
    c.new.respond_to?("test").should == true
    c.new.respond_to?("test=").should == false
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
      Class.new { attr_reader o }
    end
    
    (o = Object.new).should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      Class.new { attr_reader o }
    end
  end
end

describe "Module#attr_writer(name, ...)" do
  it "creates a setter for each given attribute name" do
    c = Class.new do
      attr_writer :a, :b.to_i, "c"
    end
    
    o = c.new
    ('a'..'c').each do |x|
      o.respond_to?(x).should == false
      o.respond_to?("#{x}=").should == true
    end
    
    o.a = "test"
    o.instance_variable_get(:@a).should == "test"
    o.b = "test2"
    o.instance_variable_get(:@b).should == "test2"
    o.c = "test3"
    o.instance_variable_get(:@c).should == "test3"
  end

  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "test")
    c = Class.new do
      attr_writer o
    end
    
    c.new.respond_to?("test").should == false
    c.new.respond_to?("test=").should == true
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
      Class.new { attr_writer o }
    end
    
    (o = Object.new).should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      Class.new { attr_writer o }
    end
  end
end

describe "Module#autoload(name, filename)" do
  it "registers the given filename to be loaded the first time that the Module with the given name is accessed" do
    begin
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/module_spec.autoload.rb") }
      
      # $m::A is set in module_spec.autoload.rb
      $m.const_get(:A).should == "test"

      # I know this is bad...
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../core/module_spec.autoload.rb") }
      $m.const_get(:A).should == "test"

      # And this is no better...
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../core/../core/module_spec.autoload.rb") }
      $m.const_get(:A).should == "test"
    ensure
      $m = nil
    end
  end
  
  it "does not autoload when the specified constant was already set" do
    begin
      $m = Module.new { autoload(:A, File.dirname(__FILE__) + "/../core/../core/../core/module_spec.autoload.rb") }
      $m.const_set(:A, "testing!")
      $m.const_get(:A).should == "testing!"
    ensure
      $m = nil
    end
  end

  it "raises a NameError when an invalid constant name is given" do
    should_raise(NameError, "autoload must be constant name") do
      Module.new { autoload("no_constant", File.dirname(__FILE__) + "/module_spec.autoload.rb") }
    end

    should_raise(NameError, "autoload must be constant name") do
      Module.new { autoload("123invalid", File.dirname(__FILE__) + "/module_spec.autoload.rb") }
    end

    should_raise(NameError, "autoload must be constant name") do
      Module.new { autoload("This One is invalid, too!", File.dirname(__FILE__) + "/module_spec.autoload.rb") }
    end
  end
  
  it "raises an ArgumentError when an empty filename is given" do
    should_raise(ArgumentError, "empty file name") do
      Module.new { autoload("A", "") }
    end
  end
end

module_class_eval = shared "Module#class_eval | Module#module_eval" do |cmd|
  describe "Module##{cmd}(string, filename, lineno, &block)" do
    it "evaluates a given string in the context of self" do
      ModuleSpecs.send(cmd, "self").should == ModuleSpecs
      ModuleSpecs.send(cmd, "1 + 1").should == 2
    end
    
    it "evaluates a given block in the context of self" do
      ModuleSpecs.send(cmd) { self }.should == ModuleSpecs
      ModuleSpecs.send(cmd) { 1 + 1 }.should == 2
    end
    
    it "uses the optional filename and lineno parameters for error messages" do
      begin
        ModuleSpecs.send(cmd, "raise", "test", 102)
      rescue Exception => e
        e.backtrace.first.should == "test:102:in `send'"
      end
    end
  end

  it "converts non string eval-string to string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "1 + 1")
    ModuleSpecs.send(cmd, o).should == 2
  end

  it "raises a TypeError when the given eval-string can't be converted to string using to_str" do
    o = Object.new
    should_raise(TypeError, "can't convert Object into String") do
      ModuleSpecs.send(cmd, o)
    end
    
    (o = Object.new).should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      ModuleSpecs.send(cmd, o)
    end
  end
  
  it "raises an ArgumentError when no arguments and no block are given" do
    should_raise(ArgumentError, "block not supplied") do
      ModuleSpecs.send(cmd)
    end
  end
  
  it "raises an ArgumentError when more than 3 arguments are given" do
    should_raise(ArgumentError, "wrong number of arguments: #{cmd}(src) or #{cmd}{..}") do
      ModuleSpecs.send(cmd, "1 + 1", "some file", 0, "bogus")
    end
  end
  
  it "raises an ArgumentError when a block and normal arguments are given" do
    should_raise(ArgumentError, "wrong number of arguments (1 for 0)") do
      ModuleSpecs.send(cmd, "1 + 1") { 1 + 1 }
    end
  end
end

describe "Module#class_eval" do
  it_behaves_like module_class_eval, :class_eval
end

describe "Module#module_eval" do
  it_behaves_like module_class_eval, :module_eval
end

describe "Module#class_variable_defined?(name)" do
  it "returns true if a class variable with the given name is defined in self" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    c.class_variable_defined?(:@@class_var).should == true
    c.class_variable_defined?(:@@class_var.to_i).should == true
    c.class_variable_defined?("@@class_var").should == true
    c.class_variable_defined?(:@@no_class_var).should == false
    c.class_variable_defined?(:@@no_class_var.to_i).should == false
    c.class_variable_defined?("@@no_class_var").should == false
  end
  
  it "raises a NameError when the given name is not allowed" do
    c = Class.new
    
    should_raise(NameError, "`invalid_name' is not allowed as a class variable name") do
      c.class_variable_defined?(:invalid_name)
    end
    
    should_raise(NameError, "`@invalid_name' is not allowed as a class variable name") do
      c.class_variable_defined?("@invalid_name")
    end
  end

  it "converts a non string/symbol/fixnum name to string using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    (o = Object.new).should_receive(:to_str, :returning => "@@class_var")
    c.class_variable_defined?(o).should == true
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
       c.class_variable_defined?(o)
    end
    
    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      c.class_variable_defined?(o)
    end
  end
end

describe "Module#class_variable_get(name)" do
  it "returns the value of the class variable with the given name" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    c.send(:class_variable_get, :@@class_var).should == "test"
    c.send(:class_variable_get, :@@class_var.to_i).should == "test"
    c.send(:class_variable_get, "@@class_var").should == "test"
  end
  
  it "raises a NameError when an uninitialized class variable is accessed" do
    c = Class.new
    [:@@no_class_var, :@@no_class_var.to_i, "@@no_class_var"].each do |cvar|
      should_raise(NameError, "uninitialized class variable @@no_class_var in #{c}") do
        c.send(:class_variable_get, cvar)
      end
    end
  end
  
  it "raises a NameError when the given name is not allowed" do
    c = Class.new
    
    should_raise(NameError, "`invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_get, :invalid_name)
    end
    
    should_raise(NameError, "`@invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_get, "@invalid_name")
    end
  end

  it "converts a non string/symbol/fixnum name to string using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    (o = Object.new).should_receive(:to_str, :returning => "@@class_var")
    c.send(:class_variable_get, o).should == "test"
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
       c.send(:class_variable_get, o)
    end
    
    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      c.send(:class_variable_get, o)
    end
  end
end

describe "Module#class_variable_set(name, value)" do
  it "sets the class variable with the given name to the given value" do
    c = Class.new
    
    c.send(:class_variable_set, :@@test, "test")
    c.send(:class_variable_set, :@@test2.to_i, "test2")
    c.send(:class_variable_set, "@@test3", "test3")
    
    c.send(:class_variable_get, :@@test).should == "test"
    c.send(:class_variable_get, :@@test2).should == "test2"
    c.send(:class_variable_get, :@@test3).should == "test3"
  end
  
  it "raises a TypeError when self is frozen" do
    should_raise(TypeError, "can't modify frozen class") do
      Class.new.freeze.send(:class_variable_set, :@@test, "test")
    end

    should_raise(TypeError, "can't modify frozen module") do
      Module.new.freeze.send(:class_variable_set, :@@test, "test")
    end
  end
  
  it "raises a NameError when the given name is not allowed" do
    c = Class.new
    
    should_raise(NameError, "`invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_set, :invalid_name, "test")
    end
    
    should_raise(NameError, "`@invalid_name' is not allowed as a class variable name") do
      c.send(:class_variable_set, "@invalid_name", "test")
    end
  end

  it "converts a non string/symbol/fixnum name to string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "@@class_var")
    c = Class.new
    c.send(:class_variable_set, o, "test")
    c.send(:class_variable_get, :@@class_var).should == "test"
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    c = Class.new { class_variable_set :@@class_var, "test" }
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
       c.send(:class_variable_set, o, "test")
    end
    
    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      c.send(:class_variable_set, o, "test")
    end
  end
end

describe "Module#class_variables" do
  it "returns an array with the names of class variables of self and self's ancestors" do
    c = Class.new { class_variable_set(:@@var1, "one") }
    m = Module.new { class_variable_set(:@@var2, "two") }
    c2 = Class.new(c) { include m; class_variable_set(:@@var3, "three") }
    
    Class.new.class_variables.should == []
    Module.new.class_variables.should == []
    c.class_variables.should == ["@@var1"]
    m.class_variables.should == ["@@var2"]
    c2.class_variables.should == ["@@var3", "@@var2", "@@var1"]
  end
end

describe "Module#const_defined?(name)" do
  it "returns true if a constant with the given name is defined in self" do
    ModuleSpecs.const_defined?(:Child).should == true
    ModuleSpecs.const_defined?(:Child.to_i).should == true
    ModuleSpecs.const_defined?("Child").should == true

    ModuleSpecs.const_defined?("SomeThing").should == false
    ModuleSpecs.const_defined?(:SomeThing.to_i).should == false
    ModuleSpecs.const_defined?(:SomeThing).should == false
  end
  
  it "tries to convert the given name to a string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "Child")
    ModuleSpecs.const_defined?(o).should == true
  end

  it "raises a NameError when the given constant name is not allowed" do
    should_raise(NameError, "wrong constant name invalid_name") do
      ModuleSpecs.const_defined?("invalid_name")
    end
    
    should_raise(NameError, "wrong constant name @invalid_name") do
      ModuleSpecs.const_defined?("@invalid_name")
    end
  end
  
  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
       ModuleSpecs.const_defined?(o)
    end
    
    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      ModuleSpecs.const_defined?(o)
    end
  end
end

describe "Module#const_get(name)" do
  it "returns the value of the constant with the given name" do
    ModuleSpecs.const_get(:BasicModule).should == ModuleSpecs::BasicModule
    ModuleSpecs.const_get(:SuperModule.to_i).should == ModuleSpecs::SuperModule
    ModuleSpecs.const_get("Child").should == ModuleSpecs::Child
  end
  
  it "raises a NameError when there is no constant with the given name" do
    should_raise(NameError, "uninitialized constant ModuleSpecs::NotExistant") do
      ModuleSpecs.const_get("NotExistant").should == nil
    end
  end

  it "tries to convert the given name to a string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "Parent")
    ModuleSpecs.const_get(o).should == ModuleSpecs::Parent
  end

  it "raises a TypeError when the given name can't be converted to string using to_str" do
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
      ModuleSpecs.const_get(o)
    end

    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      ModuleSpecs.const_get(o)
    end
  end
end

describe "Module#const_missing(name)" do
  it "is invoked when an undefined constant is referenced in the scope of self" do
    begin
      A = Module.new do
        const_set(:A, "test")
        
        def self.const_missing(mod)
          $missing_const = mod
          return 123
        end
      end

      A::NotExistant.should == 123
      $missing_const.should == :NotExistant
      
      A.const_get(:NotExistantToo).should == 123
      $missing_const.should == :NotExistantToo

      A::A.should == "test"
      A.const_get(:A).should == "test"
      
      $missing_const.should == :NotExistantToo
    ensure
      $missing_const = nil
    end
  end
end

describe "Module#const_set(name, value)" do


  it "sets the constant with the given name to the given value" do
    
  end

  
  it "raises a NameError when there is no constant with the given name" do
    should_raise(NameError, "uninitialized constant ModuleSpecs::NotExistant") do
      ModuleSpecs.const_get("NotExistant").should == nil
    end
  end

  it "tries to convert the given name to a string using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "A")
    m = Module.new { const_set o, "test" } 
    m.const_get(:A).should == "test"
  end

  it "raises a TypeError when the given name can't be converted to string using to_str" do
    o = Object.new
    m = Module.new
    
    should_raise(TypeError, "#{o} is not a symbol") do
      m.const_set(o, "test")
    end

    o.should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      m.const_set(o, "Test")
    end
  end
end

describe "Module#constants" do
  it "returns an array with the names of all constants accessible in the scope of self" do
    ModuleSpecs.constants.should == ["Child", "SuperModule", "BasicModule", "Parent"]
    
    Module.new { const_set :A, "test" }.constants.should == [ "A" ]
    Class.new  { const_set :A, "test" }.constants.should == [ "A" ]
  end
end

describe "Module#define_method(name, Method, &block)" do
  it "defines the given method as an instance method with the given name in self" do
    c = Class.new do
      def test() "test" end
      define_method(:another_test, instance_method(:test))
    end
    
    o = c.new
    o.test.should == o.another_test
  end
  
  it "defines a new method with the given name and the given block as body in self" do
    c = Class.new do
      define_method(:test) { self }
      define_method(:test2, &lambda { self })
    end
    
    o = c.new
    o.test.should == o
    o.test2.should == o
  end
  
  it "raises a TypeError when the given method is no Method/Proc" do
    should_raise(TypeError, "wrong argument type String (expected Proc/Method)") do
      Class.new { define_method(:test, "self") }
    end
    
    should_raise(TypeError, "wrong argument type Fixnum (expected Proc/Method)") do
      Class.new { define_method(:test, 1234) }
    end
  end
end

describe "Module#extend_object(object)" do
  it "extends the given object with constants and methods of self" do
    m = Module.new do
      const_set :C, "test"
      def test() "hello" end
    end
    
    o = Object.new
    m.send(:extend_object, o)
    
    o.test.should == "hello"
    (class << o; C; end).should == "test"
  end
  
  it "is called when an object gets extended with self" do
    begin
      m = Module.new do
        def self.extend_object(o)
          $extended_object = o
        end
      end
      
      (o = Object.new).extend(m)
      
      $extended_object.should == o
    ensure
      $extended_object = nil
    end
  end
end

describe "Module#extended(module)" do
  it "is called when an object gets extended with self" do
    begin
      m = Module.new do
        def self.extended(o)
          $extended_object = o
        end
      end
      
      (o = Object.new).extend(m)
      
      $extended_object.should == o
    ensure
      $extended_object = nil
    end
  end
  
  it "is called after Module#extend_object" do
    begin
      m = Module.new do
        def self.extend_object(o)
          $extended_object = nil
        end
        
        def self.extended(o)
          $extended_object = o
        end
      end
      
      (o = Object.new).extend(m)
      
      $extended_object.should == o
    ensure
      $extended_object = nil
    end
  end
end

describe "Module#freeze" do
  it "prevents further modifications to self" do
    m = Module.new.freeze
    m.frozen?.should == true
    
    # Does not raise
    class << m; end
    
    should_raise(TypeError, "can't modify frozen object") do
      class << m
        def test() "test" end
      end
    end

    should_raise(TypeError, "can't modify frozen object") do
      def m.test() "test" end
    end
  end
end

describe "Module#include(module, ...)" do
  it "calls #append_features(self) in reversed order on each module" do
    $appended_modules = []
    
    m = Module.new do
      def self.append_features(mod)
        $appended_modules << [ self, mod ]
      end
    end
    
    m2 = Module.new do
      def self.append_features(mod)
        $appended_modules << [ self, mod ]
      end
    end

    m3 = Module.new do
      def self.append_features(mod)
        $appended_modules << [ self, mod ]
      end
    end
    
    c = Class.new { include(m, m2, m3) }
    
    $appended_modules.should == [ [ m3, c], [ m2, c ], [ m, c ] ]
  end
end

describe "Module#include?(Module)" do
  it "returns true if the given module is included by self or one of it's ancestors" do
    ModuleSpecs::SuperModule.include?(ModuleSpecs::BasicModule).should == true
    ModuleSpecs::Child.include?(ModuleSpecs::BasicModule).should == true
    ModuleSpecs::Child.include?(ModuleSpecs::SuperModule).should == true
    ModuleSpecs::Child.include?(Kernel).should == true
    
    ModuleSpecs::Parent.include?(ModuleSpecs::BasicModule).should == false
    ModuleSpecs::BasicModule.include?(ModuleSpecs::SuperModule).should == false
  end
  
  it "raises a TypeError when no module was given" do
    should_raise(TypeError, "wrong argument type String (expected Module)") do
      ModuleSpecs::Child.include?("Test")
    end

    should_raise(TypeError, "wrong argument type Class (expected Module)") do
      ModuleSpecs::Child.include?(ModuleSpecs::Parent)
    end
  end
end

describe "Module#included(module)" do
  it "is invoked when self is included in another module or class" do
    begin
      m = Module.new do
        def self.included(o)
          $included_by = o
        end
      end
      
      c = Class.new { include m }
      
      $included_by.should == c
    ensure
      $included_by = nil
    end
  end
end

describe "Module#included_modules" do
  it "returns a list of modules included in self" do
    ModuleSpecs.included_modules.should              == []
    ModuleSpecs::Child.included_modules.should       == [ModuleSpecs::SuperModule, ModuleSpecs::BasicModule, Kernel]
    ModuleSpecs::Parent.included_modules.should      == [Kernel]
    ModuleSpecs::BasicModule.included_modules.should == []
    ModuleSpecs::SuperModule.included_modules.should == [ModuleSpecs::BasicModule]
  end
end
