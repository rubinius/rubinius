require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)
require File.expand_path('../shared/basic', __FILE__)
require File.expand_path('../shared/numeric_basic', __FILE__)
require File.expand_path('../shared/integer', __FILE__)

describe "Array#pack with format 'I'" do
  it_behaves_like :array_pack_basic, 'I'
  it_behaves_like :array_pack_basic_non_float, 'I'
  it_behaves_like :array_pack_arguments, 'I'
  it_behaves_like :array_pack_numeric_basic, 'I'
  it_behaves_like :array_pack_integer, 'I'
end

describe "Array#pack with format 'i'" do
  it_behaves_like :array_pack_basic, 'i'
  it_behaves_like :array_pack_basic_non_float, 'i'
  it_behaves_like :array_pack_arguments, 'i'
  it_behaves_like :array_pack_numeric_basic, 'i'
  it_behaves_like :array_pack_integer, 'i'
end

little_endian do
  describe "Array#pack with format 'I'" do
    it_behaves_like :array_pack_32bit_le, 'I'
  end

  describe "Array#pack with format 'I' with modifier '_'" do
    it_behaves_like :array_pack_32bit_le_platform, 'I_'
  end

  describe "Array#pack with format 'I' with modifier '!'" do
    it_behaves_like :array_pack_32bit_le_platform, 'I!'
  end

  describe "Array#pack with format 'i'" do
    it_behaves_like :array_pack_32bit_le, 'i'
  end

  describe "Array#pack with format 'i' with modifier '_'" do
    it_behaves_like :array_pack_32bit_le_platform, 'i_'
  end

  describe "Array#pack with format 'i' with modifier '!'" do
    it_behaves_like :array_pack_32bit_le_platform, 'i!'
  end
end

big_endian do
  describe "Array#pack with format 'I'" do
    it_behaves_like :array_pack_32bit_be, 'I'
  end

  describe "Array#pack with format 'I' with modifier '_'" do
    it_behaves_like :array_pack_32bit_be_platform, 'I_'
  end

  describe "Array#pack with format 'I' with modifier '!'" do
    it_behaves_like :array_pack_32bit_be_platform, 'I!'
  end

  describe "Array#pack with format 'i'" do
    it_behaves_like :array_pack_32bit_be, 'i'
  end

  describe "Array#pack with format 'i' with modifier '_'" do
    it_behaves_like :array_pack_32bit_be_platform, 'i_'
  end

  describe "Array#pack with format 'i' with modifier '!'" do
    it_behaves_like :array_pack_32bit_be_platform, 'i!'
  end
end
