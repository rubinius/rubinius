require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.coerce_to_symbol" do
  ruby_version_is ''...'1.9' do
    it "raises TypeError if the coercion fails" do
      lambda { Rubinius::Type.coerce_to_symbol nil }.should raise_error(TypeError)
    end

    it "raises ArgumentError if supplied with a Fixnum" do
      lambda { Rubinius::Type.coerce_to_symbol 42 }.should raise_error(ArgumentError)
    end
  end

  ruby_version_is '1.9' do
    it "always raises TypeError if the coercion fails" do
      lambda { Rubinius::Type.coerce_to_symbol nil }.should raise_error(TypeError)
      lambda { Rubinius::Type.coerce_to_symbol 42 }.should raise_error(TypeError)
    end
  end
end
