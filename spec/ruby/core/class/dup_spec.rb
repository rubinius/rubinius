require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Class#dup" do
  it "duplicates both the class and the singleton class" do
    klass = Class.new do
      def hello
        "hello"
      end

      def self.message
        "text"
      end
    end

    klass_dup = klass.dup

    klass_dup.new.hello.should == "hello"
    klass_dup.message.should == "text"
  end

  it "retains the correct ancestor chain for the singleton class" do
    super_klass = Class.new do
      def hello
        "hello"
      end

      def self.message
        "text"
      end
    end

    klass = Class.new(super_klass)
    klass_dup = klass.dup

    klass_dup.new.hello.should == "hello"
    klass_dup.message.should == "text"
  end

  it "removes the name from the class if not assigned to a constant" do
    copy = CoreClassSpecs::Record.dup
    copy.name.should == ""
  end

  it "stores the new name if assigned to a constant" do
    CoreClassSpecs::RecordCopy = CoreClassSpecs::Record.dup
    CoreClassSpecs::RecordCopy.name.should == "CoreClassSpecs::RecordCopy"
  end

end
