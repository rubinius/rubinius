require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#==" do
  it "return false if the argument isn't a Tuple" do
    (Rubinius::Tuple[:a] == 1).should be_false
  end

  it "return false if the argument isn't the same size" do
    (Rubinius::Tuple[:a] == Rubinius::Tuple[:a, :b]).should be_false
  end

  it "returns false if the elements aren't equal" do
    (Rubinius::Tuple[:a] == Rubinius::Tuple[1]).should be_false
  end

  it "returns true if all elements are equal" do
    (Rubinius::Tuple[:a, 1] == Rubinius::Tuple[:a, 1]).should be_true
  end
end
