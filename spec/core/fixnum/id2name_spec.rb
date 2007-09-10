require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#id2name" do
  it "returns the string name of the object whose symbol ID is self" do
    a = :@sym
    b = :@ruby
    c = :@rubinius
    a.to_i.id2name.should == '@sym'
    b.to_i.id2name.should == '@ruby'
    c.to_i.id2name.should == '@rubinius'
  end
end
