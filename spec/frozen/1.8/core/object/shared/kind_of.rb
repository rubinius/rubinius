module ObjectSpecs
  module SomeOtherModule; end
  module AncestorModule; end
  module MyModule; end
  module MyExtensionModule; end

  class AncestorClass < String 
    include AncestorModule 
  end

  class KindaClass < AncestorClass
    include MyModule
    def initialize
      self.extend MyExtensionModule
    end
  end
end

shared :object_kind_of do |cmd|
  describe "Object##{cmd}" do
    before(:each) do
      @o = ObjectSpecs::KindaClass.new
    end

    it "returns true if given class is the object's class" do
      @o.send(cmd, ObjectSpecs::KindaClass).should == true
    end

    it "returns true if given class is an ancestor of the object's class" do
      @o.send(cmd, ObjectSpecs::AncestorClass).should == true
      @o.send(cmd, String).should == true
      @o.send(cmd, Object).should == true
    end

    it "returns false if the given class is not object's class nor an ancestor" do
      @o.send(cmd, Array).should == false
    end

    it "returns true if given a Module that is included in object's class" do
      @o.send(cmd, ObjectSpecs::MyModule).should == true
    end

    it "returns true if given a Module that is included one of object's ancestors only" do
      @o.send(cmd, ObjectSpecs::AncestorModule).should == true
    end
    
    it "returns true if given a Module that object has been extended with" do
      @o.send(cmd, ObjectSpecs::MyExtensionModule).should == true
    end

    it "returns false if given a Module not included in object's class nor ancestors" do
      @o.send(cmd, ObjectSpecs::SomeOtherModule).should == false
    end

    it "raises a TypeError if given an object that is not a Class nor a Module" do
      lambda { @o.send(cmd, 1) }.should raise_error(TypeError)
      lambda { @o.send(cmd, 'KindaClass') }.should raise_error(TypeError)
      lambda { @o.send(cmd, :KindaClass) }.should raise_error(TypeError)
      lambda { @o.send(cmd, Object.new) }.should raise_error(TypeError)
    end
  end
end
