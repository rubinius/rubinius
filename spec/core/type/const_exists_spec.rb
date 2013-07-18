require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../ruby/fixtures/constants', __FILE__)

describe "Rubinius::Type.const_exists?" do
  it "returns true with a String or Symbol name" do
    Rubinius::Type.const_exists?(Object, :CS_CONST1).should be_true
    Rubinius::Type.const_exists?(Object, "CS_CONST1").should be_true
  end

  it "returns false if no constant is defined in the search path" do
    Rubinius::Type.const_exists?(ConstantSpecs, :CS_CONSTX).should be_false
  end

  it "raises a NameError if the name does not start with a capital letter" do
    lambda { Rubinius::Type.const_exists? ConstantSpecs, "name" }.should raise_error(NameError)
  end

  it "raises a NameError if the name starts with a non-alphabetic character" do
    lambda { Rubinius::Type.const_exists? ConstantSpecs, "__CONSTX__" }.should raise_error(NameError)
    lambda { Rubinius::Type.const_exists? ConstantSpecs, "@Name" }.should raise_error(NameError)
    lambda { Rubinius::Type.const_exists? ConstantSpecs, "!Name" }.should raise_error(NameError)
    lambda { Rubinius::Type.const_exists? ConstantSpecs, "::Name" }.should raise_error(NameError)
  end

  it "raises a NameError if the name contains non-alphabetic characters except '_'" do
    Rubinius::Type.const_exists?(Object, "CS_CONST1").should be_true
    lambda { Rubinius::Type.const_exists? ConstantSpecs, "Name=" }.should raise_error(NameError)
    lambda { Rubinius::Type.const_exists? ConstantSpecs, "Name?" }.should raise_error(NameError)
  end

  it "calls #to_str to convert the given name to a String" do
    name = mock("ClassA")
    name.should_receive(:to_str).and_return("ClassA")
    Rubinius::Type.const_exists?(ConstantSpecs, name).should be_true
  end

  it "raises a TypeError if conversion to a String by calling #to_str fails" do
    name = mock('123')
    lambda { Rubinius::Type.const_exists?(ConstantSpecs, name) }.should raise_error(TypeError)

    name.should_receive(:to_str).and_return(123)
    lambda { Rubinius::Type.const_exists?(ConstantSpecs, name) }.should raise_error(TypeError)
  end

  it "doesn't call #const_missing on the receiver if unable to locate the constant" do
    ConstantSpecs::ContainerA.should_not_receive(:const_missing).with(:CS_CONSTX)
    Rubinius::Type.const_exists?(ConstantSpecs::ContainerA, :CS_CONSTX)
  end

  it "does not search the singleton class of a Class or Module" do
    Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST14).should be_false
    Rubinius::Type.const_exists?(ConstantSpecs, :CS_CONST14).should be_false
  end

  it "does not search the containing scope" do
    Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST20).should be_true
    Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST5).should be_false
  end

  it "returns false if the constant is defined in the receiver's supperclass and the inherit flag is false" do
    Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST4, false).should be_false
  end

  it "searches into the receiver superclasses if the inherit flag is true" do
    Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST4, true).should be_true
  end

  it "returns false when the receiver is a Module, the constant is defined at toplevel and the inherit flag is false" do
    Rubinius::Type.const_exists?(ConstantSpecs::ModuleA, :CS_CONST1, false).should be_false
  end

  it "returns false when the receiver is a Class, the constant is defined at toplevel and the inherit flag is false" do
    Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST1, false).should be_false
  end

  describe "with statically assigned constants" do
    it "searches the immediate class or module first" do
      Rubinius::Type.const_exists?(ConstantSpecs::ClassA, :CS_CONST10).should be_true
      Rubinius::Type.const_exists?(ConstantSpecs::ModuleA, :CS_CONST10).should be_true
      Rubinius::Type.const_exists?(ConstantSpecs::ParentA, :CS_CONST10).should be_true
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA, :CS_CONST10).should be_true
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST10).should be_true
    end

    it "searches a module included in the immediate class before the superclass" do
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST15).should be_true
    end

    it "searches the superclass before a module included in the superclass" do
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST11).should be_true
    end

    it "searches a module included in the superclass" do
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST12).should be_true
    end

    it "searches the superclass chain" do
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST13).should be_true
    end

    it "finds a toplevel constant when the receiver is a Class" do
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST1).should be_true
    end

    it "finds a toplevel constant when the receiver is a Module" do
      Rubinius::Type.const_exists?(ConstantSpecs, :CS_CONST1).should be_true
      Rubinius::Type.const_exists?(ConstantSpecs::ModuleA, :CS_CONST1).should be_true
    end
  end

  describe "with dynamically assigned constants" do
    it "searches the immediate class or module first" do
      ConstantSpecs::ClassA::CS_CONST301 = :const301_1
      Rubinius::Type.const_exists?(ConstantSpecs::ClassA, :CS_CONST301).should be_true

      ConstantSpecs::ModuleA::CS_CONST301 = :const301_2
      Rubinius::Type.const_exists?(ConstantSpecs::ModuleA, :CS_CONST301).should be_true

      ConstantSpecs::ParentA::CS_CONST301 = :const301_3
      Rubinius::Type.const_exists?(ConstantSpecs::ParentA, :CS_CONST301).should be_true

      ConstantSpecs::ContainerA::ChildA::CS_CONST301 = :const301_5
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerA::ChildA, :CS_CONST301).should be_true
    end

    it "searches a module included in the immediate class before the superclass" do
      ConstantSpecs::ParentB::CS_CONST302 = :const302_1
      ConstantSpecs::ModuleF::CS_CONST302 = :const302_2
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerB::ChildB, :CS_CONST302).should be_true
    end

    it "searches the superclass before a module included in the superclass" do
      ConstantSpecs::ModuleE::CS_CONST303 = :const303_1
      ConstantSpecs::ParentB::CS_CONST303 = :const303_2
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerB::ChildB, :CS_CONST303).should be_true
    end

    it "searches a module included in the superclass" do
      ConstantSpecs::ModuleA::CS_CONST304 = :const304_1
      ConstantSpecs::ModuleE::CS_CONST304 = :const304_2
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerB::ChildB, :CS_CONST304).should be_true
    end

    it "searches the superclass chain" do
      ConstantSpecs::ModuleA::CS_CONST305 = :const305
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerB::ChildB, :CS_CONST305).should be_true
    end

    it "returns a toplevel constant when the receiver is a Class" do
      Object::CS_CONST306 = :const306
      Rubinius::Type.const_exists?(ConstantSpecs::ContainerB::ChildB, :CS_CONST306).should be_true
    end

    it "returns a toplevel constant when the receiver is a Module" do
      Object::CS_CONST308 = :const308
      Rubinius::Type.const_exists?(ConstantSpecs, :CS_CONST308).should be_true
      Rubinius::Type.const_exists?(ConstantSpecs::ModuleA, :CS_CONST308).should be_true
    end

    it "returns the updated value of a constant" do
      ConstantSpecs::ClassB::CS_CONST309 = :const309_1
      Rubinius::Type.const_exists?(ConstantSpecs::ClassB, :CS_CONST309).should be_true

      ConstantSpecs::ClassB::CS_CONST309 = :const309_2
      Rubinius::Type.const_exists?(ConstantSpecs::ClassB, :CS_CONST309).should be_true
    end
  end
end
