require File.expand_path('../../../spec_helper', __FILE__)

describe "Enumerator#dup" do
  before do
    @enum = [0, 1].to_enum
  end

  it "creates new iteration state" do
    another = @enum.dup
    @enum.next.should == another.next
  end

  context "when the iteration was already started" do
    before do
      @enum.next
    end

    it "raises a TypeError" do
      lambda { @enum.dup }.should raise_error(TypeError)
    end

    context 'and rewound' do
      before do
        @enum.rewind
      end

      it "creates new iteration state" do
        another = @enum.dup
        @enum.next.should == another.next
      end
    end
  end
end
