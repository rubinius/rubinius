require File.dirname(__FILE__) + '/../../spec_helper'

describe "Symbol#to_i" do
  not_compliant_on :rubinius do
    it "returns an integer that is unique for each symbol for each program execution" do
      :ruby.to_i.is_a?(Integer).should == true
      :ruby.to_i.should == :ruby.to_i
      :ruby.to_i.should_not == :rubinius.to_i
    end
  end
end
