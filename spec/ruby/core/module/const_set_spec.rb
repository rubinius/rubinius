require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../fixtures/constants', __FILE__)

describe "Module#const_set" do
  it "sets the constant specified by a String or Symbol to the given value" do
    ConstantSpecs.const_set :CS_CONST401, :const401
    ConstantSpecs::CS_CONST401.should == :const401

    ConstantSpecs.const_set "CS_CONST402", :const402
    ConstantSpecs.const_get(:CS_CONST402).should == :const402
  end

  it "returns the value set" do
    ConstantSpecs.const_set(:CS_CONST403, :const403).should == :const403
  end

  describe "on a named Module" do
    before(:each) do
      @named_module = ConstantSpecs
      @first_defined = :TemporaryConstantForModuleConstSet1
      @second_defined = :TemporaryConstantForModuleConstSet2
      @anonymous_module = Module.new
      @named_module.const_set @first_defined, @anonymous_module
      @first_constant_path = "#{@named_module}::#{@first_defined}"
    end

    after(:each) do
      @named_module.send :remove_const, @second_defined
    end

    it "sets the name of an anonymous module only once" do
      @anonymous_module.name.should == @first_constant_path
      @named_module.send :remove_const, @first_defined
      @named_module.const_set @second_defined, @anonymous_module
      @anonymous_module.name.should == @first_constant_path
    end
  end

  describe "on Object" do
    it "sets the name of an anonymous module only once" do
      foo, bar, baz = Module.new, Module.new, Module.new
      foo::Bar = bar
      foo::Bar::Baz = baz

      foo.name.to_s.should == ""
      bar.name.to_s.should == ""
      baz.name.to_s.should == ""

      Object.const_set :TemporaryConstantForModuleConstSet1, foo

      begin
        foo.name.should == "TemporaryConstantForModuleConstSet1"
        bar.name.should == "TemporaryConstantForModuleConstSet1::Bar"
        baz.name.should == "TemporaryConstantForModuleConstSet1::Bar::Baz"
      ensure
        Object.send :remove_const, :TemporaryConstantForModuleConstSet1
      end

      Object.const_set :TemporaryConstantForModuleConstSet2, foo

      begin
        foo.name.should == "TemporaryConstantForModuleConstSet1"
        bar.name.should == "TemporaryConstantForModuleConstSet1::Bar"
        baz.name.should == "TemporaryConstantForModuleConstSet1::Bar::Baz"
      ensure
        Object.send :remove_const, :TemporaryConstantForModuleConstSet2
      end
    end
  end

  it "raises a NameError if the name does not start with a capital letter" do
    lambda { ConstantSpecs.const_set "name", 1 }.should raise_error(NameError)
  end

  it "raises a NameError if the name starts with a non-alphabetic character" do
    lambda { ConstantSpecs.const_set "__CONSTX__", 1 }.should raise_error(NameError)
    lambda { ConstantSpecs.const_set "@Name", 1 }.should raise_error(NameError)
    lambda { ConstantSpecs.const_set "!Name", 1 }.should raise_error(NameError)
    lambda { ConstantSpecs.const_set "::Name", 1 }.should raise_error(NameError)
  end

  it "raises a NameError if the name contains non-alphabetic characters except '_'" do
    ConstantSpecs.const_set("CS_CONST404", :const404).should == :const404
    lambda { ConstantSpecs.const_set "Name=", 1 }.should raise_error(NameError)
    lambda { ConstantSpecs.const_set "Name?", 1 }.should raise_error(NameError)
  end

  it "calls #to_str to convert the given name to a String" do
    name = mock("CS_CONST405")
    name.should_receive(:to_str).and_return("CS_CONST405")
    ConstantSpecs.const_set(name, :const405).should == :const405
    ConstantSpecs::CS_CONST405.should == :const405
  end

  it "raises a TypeError if conversion to a String by calling #to_str fails" do
    name = mock('123')
    lambda { ConstantSpecs.const_set name, 1 }.should raise_error(TypeError)

    name.should_receive(:to_str).and_return(123)
    lambda { ConstantSpecs.const_set name, 1 }.should raise_error(TypeError)
  end

  describe "on a frozen module" do
    before(:each) do
      @frozen = Module.new.freeze
      @name = :Foo
    end

    ruby_version_is "" ... "1.9" do
      it "raises a TypeError before setting the name" do
        lambda { @frozen.const_set @name, nil }.should raise_error(TypeError)
        @frozen.should_not have_constant(@name)
      end
    end

    ruby_version_is "1.9" do
      it "raises a RuntimeError before setting the name" do
        lambda { @frozen.const_set @name, nil }.should raise_error(RuntimeError)
        @frozen.should_not have_constant(@name)
      end
    end
  end
end
