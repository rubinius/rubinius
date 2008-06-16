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
  
  it "loads strings" do
    strings = ["str",
               " str", 
               "'str'",
               "str",
               " str",
               "'str'",
               "\"str\"",
                "\n str",
                "---  str",
                "---\nstr",
                "--- \nstr",
                "--- \n str",
                "--- 'str'"
              ]
    strings.each do |str|
      YAML.load(str).should == "str"
    end
  end  

  it "does not return a hash on empty strings" do
    YAML.load("").should_not be_kind_of(Hash)
    YAML.load("# nothing\n# still nothing").should_not be_kind_of(Hash)
  end

  it "accepts symbols" do
    YAML.load( "--- :locked" ).should == :locked
  end

  it "accepts collections" do
    expected = ["a", "b", "c"]
    YAML.load("--- \n- a\n- b\n- c\n").should == expected
    YAML.load("--- [a, b, c]").should == expected
    YAML.load("[a, b, c]").should == expected
  end

  it "parses start markers" do
    YAML.load("---\n").should == nil
    YAML.load("--- ---\n").should == "---"
    YAML.load("--- abc").should == "abc"
  end

  it "does not escape symbols" do
    YAML.load("foobar: >= 123").should == { "foobar" => ">= 123"}
    YAML.load("foobar: |= 567").should == { "foobar" => "|= 567"}
    YAML.load("--- \n*.rb").should == "*.rb"
    YAML.load("--- \n&.rb").should == "&.rb"
  end

  
  it "loads a symbol key that contains spaces" do
    string = ":user name: This is the user name."
    expected = { :"user name" => "This is the user name."}
       YAML.load(string).should == expected
  end
end
