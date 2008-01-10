require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_hash')
require File.dirname(__FILE__) + '/ext/subtend_hash'

describe "SubtendHash" do
  setup do
    @s = SubtendHash.new
  end
  
  it "rb_hash_new should return a new hash" do
    @s.new_hash.should == {}
  end

  it "rb_hash_aref should return the value at the key" do
    hsh = {:chunky => 'bacon'}
    @s.access(hsh, :chunky).should == 'bacon'
  end

  it "rb_hash_aset should add the key/set pair and return the value" do
    hsh = {}
    @s.insert(hsh, :chunky, 'bacon').should == 'bacon'
    hsh.should == {:chunky => 'bacon'}
  end

  it "rb_hash_delete should remove the key and return the value" do
    hsh = {:chunky => 'bacon'}
    @s.remove(hsh, :chunky).should == 'bacon'
    hsh.should == {}
  end
end
