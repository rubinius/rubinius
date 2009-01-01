require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/pwd'

describe "Dir.pwd" do
  it_behaves_like :dir_pwd, :pwd
  
  it "correctly displays dirs with unicode characters in them" do
    DirSpecs.clear_dirs

    begin
      old_kcode, $KCODE=$KCODE,'u'
      str = [0xe9].pack 'U' #Unicode é
      Dir.mkdir str
      File.exist?(str).should == true

      old_pwd = Dir.pwd
      Dir.chdir(str) { Dir.pwd.should == File.join(old_pwd, str) }
    ensure
      DirSpecs.clear_dirs
      $KCODE=old_kcode
    end
  end
end
