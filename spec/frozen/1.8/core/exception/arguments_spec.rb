require File.dirname(__FILE__) + '/../../spec_helper'

describe "ArgumentError" do

  it "is a subclass of StandardError" do
    ArgumentError.class.superclass == StandardError
  end
  
end
