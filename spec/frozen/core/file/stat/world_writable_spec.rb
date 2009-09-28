require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/world_writable'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is "1.9" do
  describe "File::Stat.world_writable?" do
    
    it_behaves_like(:file_world_writable, :world_writable?, FileStat)

  end
end
