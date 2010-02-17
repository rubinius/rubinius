
describe :enum_for, :shared => true do
  it "is defined in Kernel" do
    Kernel.method_defined?(@method).should be_true
  end

  it "returns a new enumerator" do
    "abc".send(@method).should be_an_instance_of(enumerator_class)
  end

  it "defaults the first argument to :each" do
    enum = [1,2].send(@method)
    enum.map { |v| v }.should == [1,2].each { |v| v }
  end
end
