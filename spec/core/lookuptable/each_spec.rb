require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable#each" do
  before :each do
    @lt = Rubinius::LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "iterates over the table yielding each key and value" do
    keys = []
    values = []

    @lt.each do |k,v|
      keys << k
      values << v
    end

    keys.sort { |a,b| a.to_s <=> b.to_s}.should == [:a, :b, :c]
    values.sort.should == [1, 2, 3]
  end

  it "handles keys that hash to the same bucket" do
    lt = Rubinius::LookupTable.new

    bins = lt.bins

    k1 = 1
    k2 = (1 << 10) | k1

    # check the bin
    (k1 & (bins - 1)).should == (k2 & (bins - 1))

    lt[k1] = :a
    lt[k2] = :b

    keys = []
    lt.each { |k,v| keys << k }

    keys.sort.should == [k1, k2]
  end
end
