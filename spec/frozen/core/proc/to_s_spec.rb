require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#to_s" do
  it "returns a description of self" do
    Proc.new { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:5>$/
    lambda { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:6>$/
    proc { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:7>$/
  end
end
