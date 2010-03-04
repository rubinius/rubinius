require File.expand_path('../../spec_helper', __FILE__)
require File.expand_path('../fixtures/defined', __FILE__)

describe "The defined? keyword for literals" do
  it "returns 'self' for self" do
    ret = defined?(self)
    ret.should == "self"
  end

  it "returns 'nil' for nil" do
    ret = defined?(nil)
    ret.should == "nil"
  end

  it "returns 'true' for true" do
    ret = defined?(true)
    ret.should == "true"
  end

  it "returns 'false' for false" do
    ret = defined?(false)
    ret.should == "false"
  end
end

describe "The defined? keyword when called with a method name" do
  describe "without a receiver" do
    it "returns 'method' if the method is defined" do
      defined?(puts).should == "method"
    end

    it "returns nil if the method is not defined" do
      defined?(defined_specs_undefined_method).should be_nil
    end
  end

  describe "having a module as receiver" do
    it "returns 'method' if the method is defined" do
      defined?(Kernel.puts).should == "method"
    end

    it "returns nil if the method is not defined" do
      defined?(Kernel.defined_specs_undefined_method).should be_nil
    end

    it "returns nil if the class is not defined" do
      defined?(DefinedSpecsUndefined.puts).should be_nil
    end

    it "returns nil if the subclass is not defined" do
      defined?(DefinedSpecs::Undefined.puts).should be_nil
    end
  end

  describe "having a local variable as receiver" do
    it "returns 'method' if the method is defined" do
      obj = DefinedSpecs::Basic.new
      defined?(obj.a_defined_method).should == "method"
    end

    it "returns nil if the method is not defined" do
      obj = DefinedSpecs::Basic.new
      defined?(obj.an_undefined_method).should be_nil
    end

    it "returns nil if the variable does not exist" do
      defined?(nonexistent_local_variable.some_method).should be_nil
    end
  end

  describe "having an instance variable as receiver" do
    it "returns 'method' if the method is defined" do
      @obj = DefinedSpecs::Basic.new
      defined?(@obj.a_defined_method).should == "method"
    end

    it "returns nil if the method is not defined" do
      @obj = DefinedSpecs::Basic.new
      defined?(@obj.an_undefined_method).should be_nil
    end

    it "returns nil if the variable does not exist" do
      defined?(@nonexistent_instance_variable.some_method).should be_nil
    end
  end

  describe "having a global variable as receiver" do
    it "returns 'method' if the method is defined" do
      $defined_specs_obj = DefinedSpecs::Basic.new
      defined?($defined_specs_obj.a_defined_method).should == "method"
    end

    it "returns nil if the method is not defined" do
      $defined_specs_obj = DefinedSpecs::Basic.new
      defined?($defined_specs_obj.an_undefined_method).should be_nil
    end

    it "returns nil if the variable does not exist" do
      defined?($nonexistent_global_variable.some_method).should be_nil
    end
  end

  describe "having a class variable as receiver" do
    it "returns 'method' if the method is defined" do
      @@defined_specs_obj = DefinedSpecs::Basic.new
      defined?(@@defined_specs_obj.a_defined_method).should == "method"
    end

    it "returns nil if the method is not defined" do
      @@defined_specs_obj = DefinedSpecs::Basic.new
      defined?(@@defined_specs_obj.an_undefined_method).should be_nil
    end

    it "returns nil if the variable does not exist" do
      defined?(@@nonexistent_class_variable.some_method).should be_nil
    end
  end

  describe "having a method call as a receiver" do
    it "returns nil if evaluating the receiver raises an exception" do
      defined?(DefinedSpecs.exception_method / 2).should be_nil
      ScratchPad.recorded.should == :defined_specs_exception
    end

    it "returns nil if the method is not defined on the object the receiver returns" do
      defined?(DefinedSpecs.side_effects / 2).should be_nil
      ScratchPad.recorded.should == :defined_specs_side_effects
    end

    it "returns 'method' if the method is defined on the object the receiver returns" do
      defined?(DefinedSpecs.fixnum_method / 2).should == "method"
      ScratchPad.recorded.should == :defined_specs_fixnum_method
    end
  end
end

describe "The defined? keyword for an expression" do
  before :each do
    ScratchPad.clear
  end

  it "returns 'assignment' for assigning a local variable" do
    defined?(x = 2).should == "assignment"
  end

  it "returns 'assignment' for assigning an instance variable" do
    defined?(@defined_specs_x = 2).should == "assignment"
  end

  it "returns 'assignment' for assigning a global variable" do
    defined?($defined_specs_x = 2).should == "assignment"
  end

  it "returns 'assignment' for assigning a class variable" do
    defined?(@@defined_specs_x = 2).should == "assignment"
  end

  it "returns 'assignment' for assigning multiple variables" do
    defined?((a, b = 1, 2)).should == "assignment"
  end

  it "returns 'assignment' for an expression with '%='" do
    defined?(x %= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '/='" do
    defined?(x /= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '-='" do
    defined?(x -= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '+='" do
    defined?(x += 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '*='" do
    defined?(x *= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '|='" do
    defined?(x |= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '&='" do
    defined?(x &= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '^='" do
    defined?(x ^= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '~='" do
    defined?(x = 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '<<='" do
    defined?(x <<= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '>>='" do
    defined?(x >>= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '||='" do
    defined?(x ||= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '&&='" do
    defined?(x &&= 2).should == "assignment"
  end

  it "returns 'assignment' for an expression with '**='" do
    defined?(x **= 2).should == "assignment"
  end

  it "returns nil for an expression with == and an undefined method" do
    defined?(defined_specs_undefined_method == 2).should be_nil
  end

  it "returns nil for an expression with != and an undefined method" do
    defined?(defined_specs_undefined_method != 2).should be_nil
  end

  it "returns nil for an expression with !~ and an undefined method" do
    defined?(defined_specs_undefined_method !~ 2).should be_nil
  end

  it "returns 'method' for an expression with '=='" do
    x = 42
    defined?(x == 2).should == "method"
  end

  ruby_version_is ""..."1.9" do
    it "returns 'expression' for an expression with '!='" do
      x = 42
      defined?(x != 2).should == "expression"
    end

    it "returns 'expression' for an expression with '!~'" do
      x = 42
      defined?(x !~ 2).should == "expression"
    end
  end

  ruby_version_is "1.9" do
    it "returns 'method' for an expression with '!='" do
      x = 42
      defined?(x != 2).should == "method"
    end

    it "returns 'method' for an expression with '!~'" do
      x = 42
      defined?(x !~ 2).should == "method"
    end
  end

  it "returns nil for an expression with 'not' and an undefined method" do
    defined?(not defined_specs_undefined_method).should be_nil
  end

  it "returns nil for an expression with 'not' and an unset class variable" do
    defined?(not @@defined_specs_undefined_class_variable).should be_nil
  end

  it "does not propagate an exception raised by a method in a 'not' expression" do
    defined?(not DefinedSpecs.exception_method).should be_nil
    ScratchPad.recorded.should == :defined_specs_exception
  end

  ruby_version_is ""..."1.9" do
    it "calls a method in a 'not' expression and returns 'expression'" do
      defined?(not DefinedSpecs.side_effects).should == "expression"
      ScratchPad.recorded.should == :defined_specs_side_effects
    end

    it "returns 'expression' for an expression with 'not' and an unset global variable" do
      defined?(not $defined_specs_undefined_global_variable).should == "expression"
    end

    it "returns 'expression' for an expression with 'not' and an unset instance variable" do
      defined?(not @defined_specs_undefined_global_variable).should == "expression"
    end
  end

  ruby_version_is "1.9" do
    it "returns 'method' for a 'not' expression with a method" do
      defined?(not DefinedSpecs.side_effects).should == "method"
    end

    it "calls a method in a 'not' expression and returns 'method'" do
      defined?(not DefinedSpecs.side_effects).should == "method"
      ScratchPad.recorded.should == :defined_specs_side_effects
    end

    it "returns nil for an expression with 'not' and an unset global variable" do
      defined?(not $defined_specs_undefined_global_variable).should be_nil
    end

    it "returns nil for an expression with 'not' and an unset instance variable" do
      defined?(not @defined_specs_undefined_global_variable).should be_nil
    end
  end

  it "returns nil for an expression with '!' and an undefined method" do
    defined?(!defined_specs_undefined_method).should be_nil
  end

  it "returns nil for an expression with '!' and an unset class variable" do
    defined?(!@@defined_specs_undefined_class_variable).should be_nil
  end

  ruby_version_is ""..."1.9" do
    it "returns 'expression' for an expression with '!' and an unset global variable" do
      defined?(!$defined_specs_undefined_global_variable).should == "expression"
    end

    it "returns 'expression' for an expression with '!' and an unset instance variable" do
      defined?(!@defined_specs_undefined_global_variable).should == "expression"
    end
  end

  ruby_version_is "1.9" do
    it "returns nil for an expression with '!' and an unset global variable" do
      defined?(!$defined_specs_undefined_global_variable).should be_nil
    end

    it "returns nil for an expression with '!' and an unset instance variable" do
      defined?(!@defined_specs_undefined_global_variable).should be_nil
    end
  end

  it "returns 'expression' when passed a String" do
    defined?("garble gooble gable").should == "expression"
  end

  describe "with a dynamic String" do
    it "returns 'expression' when the String contains a literal" do
      defined?("garble #{42}").should == "expression"
    end

    it "returns 'expression' when the String contains a call to a defined method" do
      defined?("garble #{DefinedSpecs.side_effects}").should == "expression"
    end

    ruby_version_is ""..."1.9" do
      it "returns nil when the String contains a call to an undefined method" do
        defined?("garble #{DefinedSpecs.undefined_method}").should be_nil
      end

      it "calls the method in the String" do
        defined?("garble #{DefinedSpecs.side_effects}")
        ScratchPad.recorded.should == :defined_specs_side_effects
      end
    end

    ruby_version_is "1.9" do
      it "returns 'expression' when the String contains a call to an undefined method" do
        defined?("garble #{DefinedSpecs.undefined_method}").should == "expression"
      end

      it "does not call the method in the String" do
        defined?("garble #{DefinedSpecs.dynamic_string}")
        ScratchPad.recorded.should be_nil
      end
    end
  end

  it "returns 'expression' when passed a Fixnum literal" do
    defined?(42).should == "expression"
  end

  it "returns 'expression' when passed a Bignum literal" do
    defined?(0xdead_beef_deed_feed).should == "expression"
  end

  it "returns 'expression' when passed a Float literal" do
    defined?(1.5).should == "expression"
  end

  it "returns 'expression' when passed a Range literal" do
    defined?(0..2).should == "expression"
  end

  it "returns 'expression' when passed a Regexp literal" do
    defined?(/undefined/).should == "expression"
  end

  it "returns 'expression' when passed an Array literal" do
    defined?([1, 2]).should == "expression"
  end

  it "returns 'expression' when passed a Hash literal" do
    defined?({:a => :b}).should == "expression"
  end

  it "returns 'expression' when passed a Symbol literal" do
    defined?(:defined_specs).should == "expression"
  end
end

describe "The defined? keyword for variables" do
  it "returns 'local-variable' when called with the name of a local variable" do
    DefinedSpecs::Basic.new.local_variable_defined.should == "local-variable"
  end

  it "returns nil for an instance variable that has not been read" do
    DefinedSpecs::Basic.new.instance_variable_undefined.should be_nil
  end

  it "returns nil for an instance variable that has been read but not assigned to" do
    DefinedSpecs::Basic.new.instance_variable_read.should be_nil
  end

  it "returns 'instance-variable' for an instance variable that has been assigned" do
    DefinedSpecs::Basic.new.instance_variable_defined.should == "instance-variable"
  end

  it "returns nil for a global variable that has not been read" do
    DefinedSpecs::Basic.new.global_variable_undefined.should be_nil
  end

  it "returns nil for a global variable that has been read but not assigned to" do
    DefinedSpecs::Basic.new.global_variable_read.should be_nil
  end

  it "returns 'global-variable' for a global variable that has been assigned" do
    DefinedSpecs::Basic.new.global_variable_defined.should == "global-variable"
  end

  it "returns nil for a class variable that has not been read" do
    DefinedSpecs::Basic.new.class_variable_undefined.should be_nil
  end

  # There is no spec for a class variable that is read before being assigned
  # to because setting up the code for this raises a NameError before you
  # get to the defined? call so it really has nothing to do with 'defined?'.

  it "returns 'class variable' when called with the name of a class variable" do
    DefinedSpecs::Basic.new.class_variable_defined.should == "class variable"
  end

  ruby_version_is ""..."1.9" do
    not_compliant_on :rubinius do
      it "returns 'local-variable(in-block)' when called with the name of a block local" do
        block = Proc.new { |xxx| defined?(xxx) }
        block.call(1).should == "local-variable(in-block)"
      end
    end

    deviates_on :rubinius do
    # Rubinius does not care about dynamic vars
    it "returns 'local-variable' when called with the name of a block local" do
      block = Proc.new { |x| defined?(x) }
      ret = block.call(1)
      ret.should == 'local-variable'
    end
  end

  end

  ruby_version_is "1.9" do
    it "returns 'local-variable' when called with the name of a block local" do
      block = Proc.new { |xxx| defined?(xxx) }
      block.call(1).should == "local-variable"
    end
  end
end

describe "The defined? keyword for a constant" do
  it "returns 'constant' when the constant is defined" do
    defined?(DefinedSpecs).should == "constant"
  end

  it "returns 'constant' when passed the name of a top-level constant" do
    defined?(::DefinedSpecs).should == "constant"
  end

  it "returns nil when the constant is not defined" do
    defined?(DefinedSpecsUndefined).should be_nil
  end

  it "returns 'constant' when the scoped constant is defined" do
    defined?(DefinedSpecs::Basic).should == "constant"
  end

  it "returns nil when the scoped constant is not defined" do
    defined?(DefinedSpecs::Undefined).should be_nil
  end

  it "returns nil when the undefined constant is scoped to an undefined constant" do
    defined?(DefinedSpecs::Undefined::Undefined).should be_nil
  end

  it "returns 'constant' for a constant explicitly scoped to self:: when set" do
    defined?(DefinedSpecs::SelfScoped).should == "constant"
  end

  it "returns 'constant' for a constant explicitly scoped to self:: in subclass's metaclass" do
    DefinedSpecs::Child.parent_constant_defined.should == "constant"
  end

  it "returns 'constant' for an included module" do
    DefinedSpecs::Child.module_defined.should == "constant"
  end

  it "returns 'constant' for a constant defined in an included module" do
    DefinedSpecs::Child.module_constant_defined.should == "constant"
  end
end

describe "The defined? keyword for yield" do
  it "returns nil if no block is passed to a method not taking a block parameter" do
    DefinedSpecs::Basic.new.no_yield_block.should be_nil
  end

  it "returns nil if no block is passed to a method taking a block parameter" do
    DefinedSpecs::Basic.new.no_yield_block_parameter.should be_nil
  end

  it "returns 'yield' if a block is passed to a method not taking a block parameter" do
    DefinedSpecs::Basic.new.yield_block.should == "yield"
  end

  it "returns 'yield' if a block is passed to a method taking a block parameter" do
    DefinedSpecs::Basic.new.yield_block_parameter.should == "yield"
  end
end

describe "The defined? keyword for super" do
  it "returns nil when a superclass undef's the method" do
    DefinedSpecs::ClassWithoutMethod.new.test.should be_nil
  end

  describe "for a method taking no arguments" do
    it "returns nil when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_method_no_args.should be_nil
    end

    it "returns nil from a block when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_method_block_no_args.should be_nil
    end

    it "returns nil from a #define_method when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_define_method_no_args.should be_nil
    end

    it "returns nil from a block in a #define_method when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_define_method_block_no_args.should be_nil
    end

    it "returns 'super' when a superclass method exists" do
      DefinedSpecs::Super.new.method_no_args.should == "super"
    end

    it "returns 'super' from a block when a superclass method exists" do
      DefinedSpecs::Super.new.method_block_no_args.should == "super"
    end

    it "returns 'super' from a #define_method when a superclass method exists" do
      DefinedSpecs::Super.new.define_method_no_args.should == "super"
    end

    it "returns 'super' from a block in a #define_method when a superclass method exists" do
      DefinedSpecs::Super.new.define_method_block_no_args.should == "super"
    end
  end

  describe "for a method taking arguments" do
    it "returns nil when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_method_args.should be_nil
    end

    it "returns nil from a block when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_method_block_args.should be_nil
    end

    it "returns nil from a #define_method when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_define_method_args.should be_nil
    end

    it "returns nil from a block in a #define_method when no superclass method exists" do
      DefinedSpecs::Super.new.no_super_define_method_block_args.should be_nil
    end

    it "returns 'super' when a superclass method exists" do
      DefinedSpecs::Super.new.method_args.should == "super"
    end

    it "returns 'super' from a block when a superclass method exists" do
      DefinedSpecs::Super.new.method_block_args.should == "super"
    end

    it "returns 'super' from a #define_method when a superclass method exists" do
      DefinedSpecs::Super.new.define_method_args.should == "super"
    end

    it "returns 'super' from a block in a #define_method when a superclass method exists" do
      DefinedSpecs::Super.new.define_method_block_args.should == "super"
    end
  end
end
