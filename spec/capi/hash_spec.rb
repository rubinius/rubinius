require File.dirname(__FILE__) + '/spec_helper'

load_extension("hash")

describe "C-API Hash function" do
  before :each do
    @s = CApiHashSpecs.new
  end

  describe "rb_hash_new" do
    it "returns a new hash" do
      @s.rb_hash_new.should == {}
    end
  end

  describe "rb_hash_aref" do
    it "returns the value associated with the key" do
      hsh = {:chunky => 'bacon'}
      @s.rb_hash_aref(hsh, :chunky).should == 'bacon'
    end

    it "returns the default value if it exists" do
      hsh = Hash.new(0)
      @s.rb_hash_aref(hsh, :chunky).should == 0
      @s.rb_hash_aref_nil(hsh, :chunky).should be_false
    end

    it "returns nil if the key does not exist" do
      hsh = { }
      @s.rb_hash_aref(hsh, :chunky).should be_nil
      @s.rb_hash_aref_nil(hsh, :chunky).should be_true
    end
  end

  describe "rb_hash_lookup" do
    it "returns the value associated with the key" do
      hsh = {:chunky => 'bacon'}
      @s.rb_hash_lookup(hsh, :chunky).should == 'bacon'
    end

    it "does not return the default value if it exists" do
      hsh = Hash.new(0)
      @s.rb_hash_lookup(hsh, :chunky).should be_nil
      @s.rb_hash_lookup_nil(hsh, :chunky).should be_true
    end

    it "returns nil if the key does not exist" do
      hsh = { }
      @s.rb_hash_lookup(hsh, :chunky).should be_nil
      @s.rb_hash_lookup_nil(hsh, :chunky).should be_true
    end
  end

  describe "rb_hash_aset" do
    it "adds the key/value pair and returns the value" do
      hsh = {}
      @s.rb_hash_aset(hsh, :chunky, 'bacon').should == 'bacon'
      hsh.should == {:chunky => 'bacon'}
    end
  end

  describe "rb_hash_delete" do
    it "removes the key and returns the value" do
      hsh = {:chunky => 'bacon'}
      @s.rb_hash_delete(hsh, :chunky).should == 'bacon'
      hsh.should == {}
    end
  end

  describe "rb_hash_foreach" do
    it "iterates over the hash" do
      hsh = {:name => "Evan", :sign => :libra}

      out = @s.rb_hash_foreach(hsh)
      out.equal?(hsh).should == false
      out.should == hsh
    end
  end

  # rb_hash_size is a static symbol in MRI
  extended_on :rubinius do
    describe "rb_hash_size" do
      it "returns the size of the hash" do
        hsh = {:fast => 'car', :good => 'music'}
        @s.rb_hash_size(hsh).should == 2
      end

      it "returns zero for an empty hash" do
        @s.rb_hash_size({}).should == 0
      end
    end
  end
end
