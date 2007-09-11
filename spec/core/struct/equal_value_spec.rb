require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#==" do
  it "compares all attributes" do
    Struct::Ruby.new('2.0', 'i686').should == Struct::Ruby.new('2.0', 'i686')
  end  
end
