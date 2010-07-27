require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)
require File.expand_path('../shared/integer', __FILE__)

describe "String#unpack with format 'v'" do
  it_behaves_like :string_unpack_16bit_le, 'v'
  it_behaves_like :string_unpack_16bit_le_unsigned, 'v'
end
