require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  describe "Hash::Iterator.new" do
    ruby_version_is ""..."1.9" do
      it "initializes the iterator" do
        h = Hash[:a => 1, :b => 2]
        entries = h.instance_variable_get :@entries
        iter = h.to_iter
        iter.instance_variable_get(:@entries).should == entries
        iter.instance_variable_get(:@capacity).should == entries.size
        iter.instance_variable_get(:@index).should == -1
      end
    end
  end
end
