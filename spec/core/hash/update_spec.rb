require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/iteration_spec'

@hash_update = shared "Hash#update" do |cmd|
  describe "Hash##{cmd}" do
    it "adds the entries from other, overwriting duplicate keys. Returns self" do
      h = { :_1 => 'a', :_2 => '3' }
      h.send(cmd, :_1 => '9', :_9 => 2).equal?(h).should == true
      h.should == {:_1 => "9", :_2 => "3", :_9 => 2}
    end

    it "sets any duplicate key to the value of block if passed a block" do
      h1 = { :a => 2, :b => -1 }
      h2 = { :a => -2, :c => 1 }
      h1.send(cmd, h2) { |k,x,y| 3.14 }.equal?(h1).should == true
      h1.should == {:c => 1, :b => -1, :a => 3.14}

      h1.send(cmd, h1) { nil }
      h1.should == { :a => nil, :b => nil, :c => nil }
    end

    it "calls to_hash on its argument" do
      obj = Object.new
      obj.should_receive(:to_hash, :returning => {1 => 2})
      {3 => 4}.send(cmd, obj).should == {1 => 2, 3 => 4}

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_hash], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_hash], :returning => { 1 => 2})
      {3 => 4}.send(cmd, obj).should == {1 => 2, 3 => 4}
    end

    it "does not call to_hash on hash subclasses" do    
      {3 => 4}.send(cmd, ToHashHash[1 => 2]).should == {1 => 2, 3 => 4}
    end

    it "processes entries with same order as merge()" do
      h = {1 => 2, 3 => 4, 5 => 6, "x" => nil, nil => 5, [] => []}
      merge_bang_pairs = []
      merge_pairs = []
      h.merge(h) { |*arg| merge_pairs << arg }
      h.send(cmd, h) { |*arg| merge_bang_pairs << arg }
      merge_bang_pairs.should == merge_pairs
    end

    it "raises TypeError if called on a frozen instance" do
      hash.send(cmd, empty) # ok, empty
      should_raise(TypeError) { hash.send(cmd, 1 => 2) }
    end
  end
end

describe "Hash#update" do
  it_behaves_like(@hash_update, :update)

  it_behaves_like(@hash_iteration_method, :update)
end
