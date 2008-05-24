require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/pwd'

describe "Dir.pwd" do
  it_behaves_like :dir_pwd, :pwd
  
  it "correctly displays dirs with unicode characters in them" do
    DirSpecs.clear_dirs

    begin
      Dir.mkdir '???'
      File.exist?('???').should == true

      old_pwd = Dir.pwd
      Dir.chdir('???') { Dir.pwd.should == File.join(old_pwd, '???') }
    ensure
      DirSpecs.clear_dirs
    end
  end
end
