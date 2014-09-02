require File.expand_path('../../../spec_helper', __FILE__)

describe 'Rubinius::Type.coerce_basic_object_guard' do
  it 'raises a TypeError for a BasicObject' do
    obj   = BasicObject.new
    block = lambda { Rubinius::Type.coerce_basic_object_guard(obj, String) }

    block.should raise_error(TypeError)
  end

  it 'does not raise a TypeError for a String' do
    obj   = 'hello'
    block = lambda { Rubinius::Type.coerce_basic_object_guard(obj, String) }

    block.should_not raise_error
  end
end
