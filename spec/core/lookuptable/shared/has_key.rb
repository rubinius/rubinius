describe :lookuptable_has_key, :shared => true do
  before :each do
    @lt = Rubinius::LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "returns true if the LookupTable contains the key" do
    @lt.send(@method, :a).should == true
    @lt.send(@method, :b).should == true
  end

  it "returns false if the LookupTable does not contain the key" do
    @lt.send(@method, :d).should == false
  end

  it "converts a String key argument to a Symbol" do
    @lt.send(@method, "c").should == true
  end
end
