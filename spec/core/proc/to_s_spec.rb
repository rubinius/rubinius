require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#to_s" do
  it "returns a description of self" do
    Proc.new(def meth; "hello"; end).to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:5>$/
  end
end
