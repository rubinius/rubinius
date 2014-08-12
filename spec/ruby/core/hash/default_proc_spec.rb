require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Hash#default_proc" do
  it "returns the block passed to Hash.new" do
    h = new_hash { |i| 'Paris' }
    p = h.default_proc
    p.call(1).should == 'Paris'
  end

  it "returns nil if no block was passed to proc" do
    new_hash.default_proc.should == nil
  end
end
