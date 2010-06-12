require File.expand_path('../../../spec_helper', __FILE__)

describe "Class" do
  it "permits setting an attr_accessor :name" do
    klass = Class.new
    class << klass
      attr_accessor :name
    end

    klass.name = "Kittens"
    klass.name.should == "Kittens"
  end
end
