require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/world_readable'

ruby_version_is "1.9" do
  describe "File.world_readable?" do
    it_behaves_like(:file_world_readable, :world_readable?, File)

    it "returns nil if the file does not exist" do
      file = rand.to_s + $$.to_s
      File.exists?(file).should be_false
      File.world_readable?(file).should be_nil
    end
  end
end
