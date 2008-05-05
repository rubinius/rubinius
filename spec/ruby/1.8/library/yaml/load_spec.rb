require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "YAML.load" do
  after :each do
    File.delete $test_file if File.exist? $test_file
  end
  
  it "returns a document from current io stream when io provided" do
    File.open($test_file, 'w') do |io|
      YAML.dump( ['badger', 'elephant', 'tiger'], io )
    end
    File.open($test_file) { |yf| YAML.load( yf ) }.should == ['badger', 'elephant', 'tiger']
  end
  
  it "accepts a string argument" do
    YAML.load( "--- :locked" ).should == :locked
  end  

  it "cleans strings when loading" do
    YAML.load("--- \nstring").should == "string"
    YAML.load("--- \"string\"").should == "string"
  end
end
