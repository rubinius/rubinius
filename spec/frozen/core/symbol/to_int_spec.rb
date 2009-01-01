require File.dirname(__FILE__) + '/../../spec_helper'

describe "Symbol#to_int" do
  not_compliant_on :rubinius do
    it "returns Symbol#to_i" do
      :ruby.to_int.is_a?(Integer).should == true
      :ruby.to_int.should == :ruby.to_i
      :ruby.to_int.should_not == :rubinius.to_i
    end
  end
end
