require File.expand_path('../spec_helper', __FILE__)

load_extension("util")

describe "CApiUtilSpecs" do
  before :each do
    @s = CApiUtilSpecs.new
  end

  describe "ruby_setenv" do
    it "sets an environment variable" do
      @s.ruby_setenv('FOO', 'BAR')
      ENV['FOO'].should == 'BAR'
    end
  end

  describe "ruby_strdup" do
    it "duplicates a string" do
      @s.ruby_strdup("FOO").should == 'FOO'
    end
  end
end
