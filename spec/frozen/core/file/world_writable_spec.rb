require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/world_writable'

ruby_version_is "1.9" do
  describe "File.world_writable?" do
    
    it_behaves_like(:file_world_writable, :world_writable?, File)

    it "returns nil if the file does not exist" do
      file = rand.to_s + $$.to_s
      File.exists?(file).should be_false
      File.world_writable?(file).should be_nil
    end

  end
end
