require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Class.inherited" do

  before :each do
    ClassSpecs::Record.called nil
  end
  
  it "is invoked with the child Class when self is subclassed" do
    begin
      top = Class.new do
        def self.inherited(cls)
          $child_class = cls
        end
      end
      
      child = Class.new(top)
      $child_class.should == child

      other_child = Class.new(top)
      $child_class.should == other_child
    ensure
      $child_class = nil
    end
  end

  it "is invoked only once per subclass" do
    expected = [
      [ClassSpecs::Inherited::A, ClassSpecs::Inherited::B],
      [ClassSpecs::Inherited::B, ClassSpecs::Inherited::C],
    ]

    ClassSpecs::Inherited::A::SUBCLASSES.should == expected
  end

  it "is called when marked as a private class method" do
    ClassSpecs::A.private_class_method :inherited
    ClassSpecs::Record.called?.should == nil
    module ClassSpecs; class B < A; end; end
    ClassSpecs::Record.called?.should == ClassSpecs::B
  end
  
  it "is called when marked as a protected class method" do
    class << ClassSpecs::A
      protected :inherited
    end
    ClassSpecs::Record.called?.should == nil
    module ClassSpecs; class C < A; end; end
    ClassSpecs::Record.called?.should == ClassSpecs::C
  end
  
  it "is called when marked as a public class method" do
    ClassSpecs::A.public_class_method :inherited
    ClassSpecs::Record.called?.should == nil
    module ClassSpecs; class D < A; end; end
    ClassSpecs::Record.called?.should == ClassSpecs::D
  end
  
  it "is called by super from a method provided by an included module" do
    ClassSpecs::Record.called?.should == nil
    module ClassSpecs; class E < F; end; end
    ClassSpecs::Record.called?.should == ClassSpecs::E
  end
  
  it "is called by super even when marked as a private class method" do
    ClassSpecs::Record.called?.should == nil
    ClassSpecs::H.private_class_method :inherited
    module ClassSpecs; class I < H; end; end
    ClassSpecs::Record.called?.should == ClassSpecs::I
  end
  
  it "will be invoked by child class regardless of visibility" do
    top = Class.new do
      class << self
        def inherited(cls); end
      end
    end

    class << top; private :inherited; end
    lambda { Class.new(top) }.should_not raise_error

    class << top; protected :inherited; end
    lambda { Class.new(top) }.should_not raise_error
  end

end

