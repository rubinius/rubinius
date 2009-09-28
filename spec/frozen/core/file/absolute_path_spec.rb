require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "File.absolute_path" do
    before(:each) do
      @abs = File.expand_path(__FILE__)
    end

    it "returns the argument if it's an absolute pathname" do
      File.absolute_path(@abs).should == @abs
    end

    it "resolves paths relative to the current working directory" do
      Dir.chdir(File.dirname(@abs)) do |dir|
        File.absolute_path(File.expand_path('./' + File.basename(__FILE__))).should == @abs
      end
    end

    it "doesn't expand '~'" do
      File.absolute_path('~').should_not == File.expand_path('~')
    end 

    it "accepts a second argument of a directory from which to resolve the path" do
      File.absolute_path(__FILE__, File.dirname(__FILE__)).should == @abs
    end

    it "calls #to_path on its argument" do
      File.absolute_path(mock_to_path(@abs)).should == @abs
    end
  end
end
