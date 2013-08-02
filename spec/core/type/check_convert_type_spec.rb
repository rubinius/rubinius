require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.check_convert_type" do
  it "returns the object without calling the method if it is already of the specified type" do
    obj = TypeSpecs::A.new
    obj.should_not_receive(:coerce)

    Rubinius::Type.check_convert_type(obj, TypeSpecs::A, :coerce).should equal(obj)
  end

  it "returns the result of calling the method on the object if it is not the specified type" do
    obj = mock("Type.coerce_to")
    val = TypeSpecs::A.new
    obj.should_receive(:coerce).and_return(val)

    Rubinius::Type.check_convert_type(obj, TypeSpecs::A, :coerce).should equal(val)
  end

  it "returns the result of calling the private method on the object if it is not the specified type" do
    obj = TypeSpecs::PrivateToAry.new

    Rubinius::Type.check_convert_type(obj, Array, :to_ary).should == [1, 2, 3]
  end

  it "returns nil if the method raises an exception" do
    obj = mock("Type.coerce_to")
    obj.should_receive(:coerce).and_raise(Exception)

    Rubinius::Type.check_convert_type(obj, TypeSpecs::A, :coerce).should be_nil
  end

  it "raises a TypeError if the method does not return the specified type" do
    obj = mock("Type.coerce_to")
    obj.should_receive(:coerce).and_return(TypeSpecs::C.new)

    lambda do
      Rubinius::Type.check_convert_type(obj, TypeSpecs::A, :coerce)
    end.should raise_error(Exception)
  end
end
