require File.dirname(__FILE__) + '/../spec_helper'

# ==, [], []=, clear, default, default=, default_proc, delete,
# delete_if, each, each_key, each_pair, each_value, empty?, fetch,
# has_key?, has_value?, include?, index, indexes, indices,
# initialize_copy, inspect, invert, key?, keys, length, member?,
# merge, merge!, rehash, reject, reject!, replace, select, shift,
# size, sort, store, to_a, to_hash, to_s, update, value?, values,
# values_at

context "Hash instance method" do
  specify "key? should return true if argument is a key" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      p [@h.key?(:a), @h.key?(:b), @h.key?('b'), @h.key?(2)]
    end.should == '[true, true, false, false]'
  end
  
  specify "has_key? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      p [@h.has_key?(:a), @h.has_key?(:b), @h.has_key?('b'), @h.has_key?(2)]
    end.should == '[true, true, false, false]'
  end
  
  specify "member? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      p [@h.member?(:a), @h.member?(:b), @h.member?('b'), @h.member?(2)]
    end.should == '[true, true, false, false]'
  end
  
  specify "include? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      p [@h.include?(:a), @h.include?(:b), @h.include?('b'), @h.include?(2)]
    end.should == '[true, true, false, false]'
  end

  specify "'key?' should return false if the key was not found" do
    example do
      @h = {5 => 7}
      p @h.key?(7)
    end.should == "false"
  end

  specify "'key?' should return true if the key's matching value was nil" do
    example do
      @h = {:xyz => nil}
      p @h.key?(:xyz)
    end.should == "true"
  end

  specify "'key?' should return true if the key's matching value was false" do
    example do
      @h = {:xyz => false}
      p @h.key?(:xyz)
    end.should == "true"
  end

  specify "'key?' should return true if the key was found" do
    example do
      @h = {:xyz => 9}
      p @h.key?(:xyz)
    end.should == "true"
  end

  specify "'[]' should return the default (immediate) value" do
    example do
      @h = Hash.new(7)
      puts @h[1234]
    end.should == '7'
  end

  specify "'[]' should return the default (dynamic) value" do
    example do
      @h = Hash.new {|hsh, key| hsh[key] = (key.to_s + 'awesome')}
      puts @h[1234]
    end.should == '1234awesome'
  end
end
