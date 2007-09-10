require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#hash" do
  it "is provided" do
    Regexp.new('').respond_to?(:hash).should == true
  end

  it "is based on the text and options of Regexp" do
    (/cat/ix.hash == /cat/ixn.hash).should == true
    (/dog/m.hash  == /dog/m.hash).should == true
    (/cat/.hash   == /cat/ix.hash).should == false
    (/cat/.hash   == /dog/.hash).should == false
  end
end
