require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#to_s" do
  it "returns a description of self" do
    a = lambda { "hello" }
    a.to_s =~ /^\\#<Proc:(.*?)@to_s_spec\\.rb:5$/
  end
end