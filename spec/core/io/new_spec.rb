require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO.new" do
  it "raises TypeError if given invalid arguments" do
    should_raise(TypeError) do
      IO.new(nil)
    end
  end
end
