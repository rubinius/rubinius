require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.try_convert" do
  it "returns the object without calling the method if it is already of the specified type" do
    obj = TypeSpecs::A.new
    obj.should_not_receive(:coerce)

    Rubinius::Type.try_convert(obj, TypeSpecs::A, :coerce).should equal(obj)
  end

  it "returns the result of calling the method on the object if it is not the specified type" do
    obj = mock("Type.coerce_to")
    val = TypeSpecs::A.new
    obj.should_receive(:coerce).and_return(val)

    Rubinius::Type.try_convert(obj, TypeSpecs::A, :coerce).should equal(val)
  end

  it "raises a TypeError if the method does not return the specified type" do
    obj = mock("Type.coerce_to")
    obj.should_receive(:coerce).and_return(TypeSpecs::C.new)

    lambda do
      Rubinius::Type.try_convert(obj, TypeSpecs::A, :coerce)
    end.should raise_error(Exception)
  end
end
