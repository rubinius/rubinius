require File.expand_path('../../../spec_helper', __FILE__)

describe "Module#thunk_method" do
  before :each do
    @class = Class.new
    @value = Object.new
  end

  it "defines a reader method that always returns same value" do
    @class.thunk_method :a_reader, @value
    @class.new.a_reader.should equal(@value)
    @class.new.a_reader.should equal(@value)
  end

  it "accepts a String name as Symbol" do
    @class.thunk_method "a_reader", @value
    @class.new.a_reader.should equal(@value)
  end

  it "returns the coerced name" do
    @class.thunk_method("a_reader", @value).should equal(:a_reader)
  end

  it "creates an executable that can be used to read the value" do
    @class.thunk_method(:thunk, @value)
    @class.instance_method(:thunk).executable.value.should equal(@value)
  end

  it "creates an executable that can be used to change the value" do
    @class.thunk_method(:thunk, @value)
    new_value = Object.new
    @class.instance_method(:thunk).executable.value = new_value
    @class.instance_method(:thunk).executable.value.should equal(new_value)
    @class.new.thunk.should equal(new_value)
  end
end
