require File.expand_path('../spec_helper', __FILE__)

load_extension("st", :language=>:cpp)

describe "C-API ST-Hash function" do
  before :each do
    @s = CApiStSpecs.new
  end

  describe "st_insert" do
    it "returns 1/0 if the key does/does not exist" do
      @s.st_insert(1, "a").should == 0
      @s.st_insert(1, "b").should == 1
    end
  end

  describe "st_lookup" do
    it "returns 0 if the key does not exist" do
      @s.st_lookup(1).should be_nil
    end

    it "returns value if the key exists" do
      @s.st_insert(1, "a")
      @s.st_lookup(1).should == "a"
    end
  end

  describe "st_delete" do
    it "returns 0 if the key does not exist" do
      @s.st_delete(1).should be_nil
    end

    it "returns value if the key exists" do
      @s.st_insert(1, "a")
      @s.st_delete(1).should == "a"
    end
  end

  describe "st_foreach" do
    before :each do
      @s.st_insert(123, "x")
      @s.st_insert(234, "y")
    end

    it "iterates over the table" do
      out = @s.st_foreach
      out.should == {123=>"x", 234=>"y"}
    end

    it "stops via the callback" do
      out = @s.st_foreach_stop
      out.size.should == 1
    end

    it "deletes via the callback" do
      out = @s.st_foreach_delete
      out.should == {123=>"x", 234=>"y"}
      hsh = @s.st_foreach
      hsh.should == {}
    end
  end
end
