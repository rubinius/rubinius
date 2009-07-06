require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# Modifying a collection while the contents are being iterated
# gives undefined behavior. See
# http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-core/23633

describe "Array#each_index" do
  before :each do
    ScratchPad.record []
  end

  it "passes the index of each element to the block" do
    a = ['a', 'b', 'c', 'd']
    a.each_index { |i| ScratchPad << i }
    ScratchPad.recorded.should == [0, 1, 2, 3]
  end

  it "returns self" do
    a = [:a, :b, :c]
    a.each_index { |i| }.should equal(a)
  end

  ruby_version_is '' ... '1.8.7' do
    it 'raises a LocalJumpError if no block given' do
      lambda{ [1,2].each_index }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is '1.8.7' do
    it 'returns an Enumerator if no block given' do
      [1,2].each_index.should be_kind_of(enumerator_class)
    end
  end
end
