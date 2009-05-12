describe "Block parameters" do
  it "does not override a shadowed variable from the outer scope" do
    i = 0
    a = [1,2,3]
    a.each {|i| ;}
    i.should == 0
  end

  it "captures variables from the outer scope" do
    a = [1,2,3]
    sum = 0
    var = nil
    a.each {|var| sum += var}
    sum.should == 6
    var.should == nil
  end
end