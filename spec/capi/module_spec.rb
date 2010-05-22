require File.dirname(__FILE__) + '/spec_helper'

load_extension('module')

class CApiModuleSpecs
  class A
    X = 1
  end

  class B < A
    Y = 2
  end

  class C
    Z = 3
  end

  module M
  end
end

describe "CApiModule" do

  before :each do
    @m = CApiModuleSpecs.new
  end

  describe "rb_define_global_const" do
    it "defines a constant on Object" do
      @m.rb_define_global_const("CApiModuleSpecsGlobalConst", 7)
      ::CApiModuleSpecsGlobalConst.should == 7
      Object.send :remove_const, :CApiModuleSpecsGlobalConst
    end
  end

  describe "rb_const_set given a symbol name and a value" do
    it "sets a new constant on a module" do
      @m.rb_const_set(CApiModuleSpecs::C, :W, 7)
      CApiModuleSpecs::C::W.should == 7
    end

    it "sets an existing constant's value" do
      @m.rb_const_set(CApiModuleSpecs::C, :Z, 8)
      CApiModuleSpecs::C::Z.should == 8
    end
  end

  describe "rb_define_const given a String name and a value" do
    it "defines a new constant on a module" do
      @m.rb_define_const(CApiModuleSpecs::C, "V", 7)
      CApiModuleSpecs::C::V.should == 7
    end

    it "sets an existing constant's value" do
      @m.rb_define_const(CApiModuleSpecs::C, "Z", 9)
      CApiModuleSpecs::C::Z.should == 9
    end
  end

  describe "rb_const_defined" do
    # The fixture converts C boolean test to Ruby 'true' / 'false'
    it "returns C non-zero if a constant is defined" do
      @m.rb_const_defined(CApiModuleSpecs::A, :X).should be_true
    end

    it "returns C non-zero if a constant is defined in Object" do
      @m.rb_const_defined(CApiModuleSpecs::A, :Module).should be_true
    end
  end

  describe "rb_const_defined_at" do
    # The fixture converts C boolean test to Ruby 'true' / 'false'
    it "returns C non-zero if a constant is defined" do
      @m.rb_const_defined_at(CApiModuleSpecs::A, :X).should be_true
    end

    it "does not search in ancestors for the constant" do
      @m.rb_const_defined_at(CApiModuleSpecs::B, :X).should be_false
    end

    it "does not search in Object" do
      @m.rb_const_defined_at(CApiModuleSpecs::A, :Module).should be_false
    end
  end

  describe "rb_const_get" do
    it "returns a constant defined in the module" do
      @m.rb_const_get(CApiModuleSpecs::A, :X).should == 1
    end

    it "returns a constant defined at toplevel" do
      @m.rb_const_get(CApiModuleSpecs::A, :Fixnum).should == Fixnum
    end

    it "returns a constant defined in a superclass" do
      @m.rb_const_get_from(CApiModuleSpecs::B, :X).should == 1
    end

    it "calls #const_missing if the constant is not defined in the class or ancestors" do
      CApiModuleSpecs::A.should_receive(:const_missing).with(:CApiModuleSpecsUndefined)
      @m.rb_const_get_from(CApiModuleSpecs::A, :CApiModuleSpecsUndefined)
    end
  end

  describe "rb_const_get_from" do
    it "returns a constant defined in the module" do
      @m.rb_const_get_from(CApiModuleSpecs::B, :Y).should == 2
    end

    it "returns a constant defined in a superclass" do
      @m.rb_const_get_from(CApiModuleSpecs::B, :X).should == 1
    end

    it "calls #const_missing if the constant is not defined in the class or ancestors" do
      CApiModuleSpecs::M.should_receive(:const_missing).with(:Fixnum)
      @m.rb_const_get_from(CApiModuleSpecs::M, :Fixnum)
    end
  end

  describe "rb_const_get_at" do
    it "returns a constant defined in the module" do
      @m.rb_const_get_at(CApiModuleSpecs::B, :Y).should == 2
    end

    it "calls #const_missing if the constant is not defined in the module" do
      CApiModuleSpecs::B.should_receive(:const_missing).with(:X)
      @m.rb_const_get_at(CApiModuleSpecs::B, :X)
    end
  end
end
