require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Mirror.reflect" do
  it "returns a Mirror::String instance for a String" do
    Rubinius::Mirror.reflect("a").should be_an_instance_of(Rubinius::Mirror::String)
  end

  it "returns a Mirror::String instance for an anonymous String subclass" do
    klass = Class.new String
    obj = klass.new
    Rubinius::Mirror.reflect(obj).should be_an_instance_of(Rubinius::Mirror::String)
  end

  it "returns a Mirror::String instance for a String with a singleton class" do
    str = "a"
    def str.some_method; end

    Rubinius::Mirror.reflect(str).should be_an_instance_of(Rubinius::Mirror::String)
  end
end
