# -*- encoding: binary -*-
require File.expand_path('../spec_helper', __FILE__)

load_extension("integer")

describe "CApiIntegerSpecs" do
  before :each do
    @s = CApiIntegerSpecs.new
  end

  describe "rb_integer_pack" do
    before :each do
      @value = 0x0123_4567_89ab_cdef
      @words = "\000" * 8
    end

    describe "with big endian output" do
      it "converts a positive 64-bit number" do
        result = @s.rb_integer_pack(@value, @words, 1, 8, 0,
            CApiIntegerSpecs::BIG_ENDIAN|CApiIntegerSpecs::PACK_2COMP)
        result.should == 1
        @words.should == "\x01\x23\x45\x67\x89\xAB\xCD\xEF"
      end

      it "converts a negative 64-bit number" do
        result = @s.rb_integer_pack(-@value, @words, 1, 8, 0,
            CApiIntegerSpecs::BIG_ENDIAN|CApiIntegerSpecs::PACK_2COMP)
        result.should == -1
        @words.should == "\xFE\xDC\xBA\x98\x76\x54\x32\x11"
      end
    end

    describe "with little endian output" do
      it "converts a positive 64-bit number" do
        result = @s.rb_integer_pack(@value, @words, 1, 8, 0,
            CApiIntegerSpecs::LITTLE_ENDIAN|CApiIntegerSpecs::PACK_2COMP)
        result.should == 1
        @words.should == "\xEF\xCD\xAB\x89\x67\x45\x23\x01"
      end

      it "converts a negative 64-bit number" do
        result = @s.rb_integer_pack(-@value, @words, 1, 8, 0,
            CApiIntegerSpecs::LITTLE_ENDIAN|CApiIntegerSpecs::PACK_2COMP)
        result.should == -1
        @words.should == "\x11\x32\x54\x76\x98\xBA\xDC\xFE"
      end
    end

    describe "with native endian output" do
      big_endian do
        it "converts a positive 64-bit number" do
          result = @s.rb_integer_pack(@value, @words, 1, 8, 0,
              CApiIntegerSpecs::NATIVE|CApiIntegerSpecs::PACK_2COMP)
          result.should == 1
          @words.should == "\x01\x23\x45\x67\x89\xAB\xCD\xEF"
        end

        it "converts a negative 64-bit number" do
          result = @s.rb_integer_pack(-@value, @words, 1, 8, 0,
              CApiIntegerSpecs::NATIVE|CApiIntegerSpecs::PACK_2COMP)
          result.should == -1
          @words.should == "\xFE\xDC\xBA\x98\x76\x54\x32\x11"
        end
      end

      little_endian do
        it "converts a positive 64-bit number" do
          result = @s.rb_integer_pack(@value, @words, 1, 8, 0,
              CApiIntegerSpecs::NATIVE|CApiIntegerSpecs::PACK_2COMP)
          result.should == 1
          @words.should == "\xEF\xCD\xAB\x89\x67\x45\x23\x01"
        end

        it "converts a negative 64-bit number" do
          result = @s.rb_integer_pack(-@value, @words, 1, 8, 0,
              CApiIntegerSpecs::NATIVE|CApiIntegerSpecs::PACK_2COMP)
          result.should == -1
          @words.should == "\x11\x32\x54\x76\x98\xBA\xDC\xFE"
        end
      end
    end
  end
end
