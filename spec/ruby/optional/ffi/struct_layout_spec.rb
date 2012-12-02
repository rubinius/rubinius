require File.expand_path(File.join(File.dirname(__FILE__), "spec_helper"))

describe "FFI::Struct.layout" do
  context 'when class name is not set' do
    it 'should not raise a error' do
      lambda {
        klass = Class.new(FFI::Struct)
        klass.layout :any, :int
      }.should_not raise_error
    end
  end
end
