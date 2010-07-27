require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)
require File.expand_path('../shared/integer', __FILE__)

describe "String#unpack with format 'n'" do
  it_behaves_like :string_unpack_16bit_be, 'n'
  it_behaves_like :string_unpack_16bit_be_unsigned, 'n'
end
