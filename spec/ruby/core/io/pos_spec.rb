require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/pos', __FILE__)

describe "IO#pos" do
  it_behaves_like :io_pos, :pos
end

describe "IO#pos" do  
  before :each do
    @fname = tmp('pos-text.txt')
    File.open @fname, 'w' do |f| f.write "123" end
  end

  after :each do
    rm_r @fname
  end

  it "allows a negative value when calling #ungetc at beginning of stream" do
    File.open @fname do |f|
      f.pos.should == 0
      f.ungetbyte(97)
      f.pos.should == -1
      f.getbyte
      f.pos.should == 0
    end
  end
end

describe "IO#pos=" do
  it_behaves_like :io_set_pos, :pos=
end

