require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

@dir_pwd = shared "Dir.pwd" do |cmd|
  describe "Dir.pwd" do
    it "returns the current working directory" do
      Dir.send(cmd).should == `pwd`.chomp
    end
  end
end

describe "Dir.pwd" do
  it_behaves_like @dir_pwd, :pwd
end
