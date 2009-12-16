require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/world_readable'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is "1.9" do
  describe "File::Stat.world_readable?" do
    it_behaves_like(:file_world_readable, :world_readable?, FileStat)
  end
end
