require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#to_f" do
  it "returns self converted to a Float" do
    Rational(3, 4).to_f.should eql(0.75)
    Rational(3, -4).to_f.should eql(-0.75)
    Rational(-1, 4).to_f.should eql(-0.25)
    Rational(-1, -4).to_f.should eql(0.25)
  end
end
