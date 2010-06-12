require File.expand_path('../../../spec_helper', __FILE__)

# These specs do not replace the RubySpec Hash#delete specs. These
# are specifically describing the chained bucket implementation.

describe "Hash#delete" do
  before :each do
    # construct hash values that will collide
    @hash = Hash.new
    MASK = Hash::MIN_SIZE - 1

    @top = mock("top entry")
    @mid = mock("middle entry")
    @bot = mock("bottom entry")

    def @top.hash() 0b10010000 | MASK & 7 end
    def @mid.hash() 0b10100000 | MASK & 7 end
    def @bot.hash() 0b11000000 | MASK & 7 end

    @hash[@top] = :top
    @hash[@mid] = :mid
    @hash[@bot] = :bot
  end

  it "returns and deletes the entry at the head of a bucket chain" do
    @hash.delete(@top).should == :top
  end

  it "returns and deletes an entry in the middle of a bucket chain" do
    @hash.delete(@mid).should == :mid
  end

  it "returns and deletes an entry at the end of a bucket chain" do
    @hash.delete(@bot).should == :bot
  end
end
