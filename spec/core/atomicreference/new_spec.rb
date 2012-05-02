require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::AtomicReference.new" do
  it "creates an AtomicReference to a specified value" do
    object = Object.new
    Rubinius::AtomicReference.new(object).value.should equal(object)
  end

  it "creates an AtomicReference to nil" do
    Rubinius::AtomicReference.new(nil).value.should equal(nil)
  end
end
