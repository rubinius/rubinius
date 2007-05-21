require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/hash/subtend_hash'

context "SubtendHash" do
  setup do
    @s = SubtendHash.new
  end
    
  specify "hash_new should return a new hash" do
    @s.new_hash.should == {}
  end
  
  specify "access should return the value" do
    hsh = {:chunky => 'bacon'}
    @s.access(hsh, :chunky).should == 'bacon'
  end
  
  specify "insert should add the key/set pair and return the value" do
    hsh = {}
    @s.insert(hsh, :chunky, 'bacon').should == 'bacon'
    hsh.should == {:chunky => 'bacon'}
  end
  
  specify "remove should remove the key and return the value" do
    hsh = {:chunky => 'bacon'}
    @s.remove(hsh, :chunky).should == 'bacon'
    hsh.should == {}
  end
end