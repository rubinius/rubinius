require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/codepoints'

# See redmine #1667
describe "IO#each_codepoint" do
  ruby_version_is "1.9.2" do
    it_behaves_like(:io_codepoints, :codepoints)
  
    it "calls the given block" do
      File.open(IOSpecs.gets_fixtures) do |io|
        r = []
        io.each_codepoint{|c| r << c }
        r[24].should == 232
        r.last.should == 10
      end
    end

    it "returns self" do
      File.open(IOSpecs.gets_fixtures) do |io|
        io.each_codepoint {|l| l }.should equal(io)
      end
    end
  end
end

