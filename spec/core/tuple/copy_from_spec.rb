require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#copy_from" do
  it "copies elements from one Tuple to another" do
    t = Rubinius::Tuple[:a, :c, :e, :g]
    t2 = Rubinius::Tuple.new(4)
    t2.copy_from t, 0, 4, 0
    t2.should == t
  end

  it "copies a subset of elements" do
    t = Rubinius::Tuple[:a, :c, :e, :g]
    t2 = Rubinius::Tuple.new(4)
    t2.copy_from t, 0, 2, 0
    t2.should == Rubinius::Tuple[:a, :c, nil, nil]
  end

  it "copies from and to a specific location" do
    t = Rubinius::Tuple[:a, :c, :e, :g]
    t2 = Rubinius::Tuple.new(4)
    t2.copy_from t, 1, 2, 2
    t2.should == Rubinius::Tuple[nil, nil, :c, :e]
  end

  it "can copy within the same tuple to the right" do
    t = Rubinius::Tuple[:a, :c, :e, :g, nil]
    t.copy_from t, 2, 2, 3
    t.should == Rubinius::Tuple[:a, :c, :e, :e, :g]
  end

  it "can copy within the same tuple to the left" do
    t = Rubinius::Tuple[:a, :c, :e, :g, nil]
    t.copy_from t, 2, 2, 1
    t.should == Rubinius::Tuple[:a, :e, :g, :g, nil]
  end
end
