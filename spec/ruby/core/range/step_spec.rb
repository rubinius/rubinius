require File.expand_path('../../../spec_helper', __FILE__)

describe "Range#step" do
  before :each do
    ScratchPad.record []
  end

  ruby_version_is "1.8.7" do
    it "returns an enumerator when no block is given" do
      enum = (1..10).step(4)
      enum.should be_an_instance_of(enumerator_class)
      enum.to_a.should eql([1, 5, 9])
    end
  end

  it "returns self" do
    r = 1..2
    r.step { }.should equal(r)
  end

  it "raises TypeError if step" do
    obj = mock("mock")
    lambda { (1..10).step(obj) { } }.should raise_error(TypeError)
  end

  it "calls #to_int to coerce step to an Integer" do
    obj = mock("Range#step")
    obj.should_receive(:to_int).and_return(1)

    (1..2).step(obj) { |x| ScratchPad << x }
    ScratchPad.recorded.should eql([1, 2])
  end

  it "raises a TypeError if step does not respond to #to_int" do
    obj = mock("Range#step non-integer")

    lambda { (1..2).step(obj) { } }.should raise_error(TypeError)
  end

  it "raises a TypeError if #to_int does not return an Integer" do
    obj = mock("Range#step non-integer")
    obj.should_receive(:to_int).and_return("1")

    lambda { (1..2).step(obj) { } }.should raise_error(TypeError)
  end

  it "coerces the argument to integer by invoking to_int" do
    (obj = mock("2")).should_receive(:to_int).and_return(2)
    res = []
    (1..10).step(obj) {|x| res << x}
    res.should == [1, 3, 5, 7, 9]
  end

  it "raises a TypeError if the first element does not respond to #succ" do
    obj = mock("Range#step non-comparable")
    obj.should_receive(:<=>).with(obj).and_return(1)

    lambda { (obj..obj).step { |x| x } }.should raise_error(TypeError)
  end

  it "raises an ArgumentError if step is 0" do
    lambda { (-1..1).step(0) { |x| x } }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if step is 0.0" do
    lambda { (-1..1).step(0.0) { |x| x } }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if step is negative" do
    lambda { (-1..1).step(-2) { |x| x } }.should raise_error(ArgumentError)
  end

  ruby_version_is ""..."1.8.7" do
    it "raises an ArgumentError if start + step < start.succ" do
      lambda { (-1..1).step(0.5) { |x| x } }.should raise_error(ArgumentError)
    end
  end

  describe "with inclusive end" do
    describe "and Integer values" do
      it "yields Integer values incremented by 1 and less than or equal to end when not passed a step" do
        (-2..2).step { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-2, -1, 0, 1, 2])
      end

      it "yields Integer values incremented by an Integer step" do
        (-5..5).step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-5, -3, -1, 1, 3, 5])
      end

      ruby_version_is ""..."1.8.7" do
        it "yields truncated Integer values incremented by a Float step" do
          (-2..2).step(1.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2, -1, 0, 1, 2])
        end
      end

      ruby_version_is "1.8.7" do
        it "yields Float values incremented by a Float step" do
          (-2..2).step(1.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2.0, -0.5, 1.0])
        end
      end
    end

    describe "and Float values" do
      it "yields Float values incremented by 1 and less than or equal to end when not passed a step" do
        (-2.0..2.0).step { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-2.0, -1.0, 0.0, 1.0, 2.0])
      end

      it "yields Float values incremented by an Integer step" do
        (-5.0..5.0).step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-5.0, -3.0, -1.0, 1.0, 3.0, 5.0])
      end

      it "yields Float values incremented by a Float step" do
        (-1.0..1.0).step(0.5) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-1.0, -0.5, 0.0, 0.5, 1.0])
      end

      ruby_bug "redmine #4576", "1.9.3" do
        it "returns Float values of 'step * n + begin <= end'" do
          (1.0..6.4).step(1.8) { |x| ScratchPad << x }
          (1.0..12.7).step(1.3) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([1.0, 2.8, 4.6, 6.4, 1.0, 2.3, 3.6,
                                         4.9, 6.2, 7.5, 8.8, 10.1, 11.4, 12.7])
        end
      end
    end

    describe "and Integer, Float values" do
      ruby_version_is ""..."1.8.7" do
        it "yields Integer values incremented by 1 and less than or equal to end when not passed a step" do
          (-2..2.0).step { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2, -1, 0, 1, 2])
        end

        it "yields Integer values incremented by an Integer step" do
          (-5..5.0).step(2) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-5, -3, -1, 1, 3, 5])
        end

        it "yields an Integer and then Float values incremented by a Float step" do
          (-1..1.0).step(0.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-1, -0.5, 0.0, 0.5, 1.0])
        end
      end

      ruby_version_is "1.8.7" do
        it "yields Float values incremented by 1 and less than or equal to end when not passed a step" do
          (-2..2.0).step { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2.0, -1.0, 0.0, 1.0, 2.0])
        end

        it "yields Float values incremented by an Integer step" do
          (-5..5.0).step(2) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-5.0, -3.0, -1.0, 1.0, 3.0, 5.0])
        end

        it "yields Float values incremented by a Float step" do
          (-1..1.0).step(0.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-1.0, -0.5, 0.0, 0.5, 1.0])
        end
      end

    end

    describe "and Float, Integer values" do
      it "yields Float values incremented by 1 and less than or equal to end when not passed a step" do
        (-2.0..2).step { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-2.0, -1.0, 0.0, 1.0, 2.0])
      end

      it "yields Float values incremented by an Integer step" do
        (-5.0..5).step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-5.0, -3.0, -1.0, 1.0, 3.0, 5.0])
      end

      it "yields Float values incremented by a Float step" do
        (-1.0..1).step(0.5) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-1.0, -0.5, 0.0, 0.5, 1.0])
      end
    end

    describe "and String values" do
      it "yields String values incremented by #succ and less than or equal to end when not passed a step" do
        ("A".."E").step { |x| ScratchPad << x }
        ScratchPad.recorded.should == ["A", "B", "C", "D", "E"]
      end

      it "yields String values incremented by #succ called Integer step times" do
        ("A".."G").step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should == ["A", "C", "E", "G"]
      end

      ruby_version_is ""..."1.8.7" do
        it "yields String values incremented by #succ called Float step times" do
          ("A".."G").step(2.0) { |x| ScratchPad << x }
          ScratchPad.recorded.should == ["A", "C", "E", "G"]
        end
      end

      ruby_version_is "1.8.7" do
        it "raises a TypeError when passed a Float step" do
          lambda { ("A".."G").step(2.0) { } }.should raise_error(TypeError)
        end
      end

      it "calls #succ on begin and each element returned by #succ" do
        obj = mock("Range#step String start")
        obj.should_receive(:<=>).exactly(3).times.and_return(-1, -1, -1, 0)
        obj.should_receive(:succ).exactly(2).times.and_return(obj)

        (obj..obj).step { |x| ScratchPad << x }
        ScratchPad.recorded.should == [obj, obj, obj]
      end
    end
  end

  describe "with exclusive end" do
    describe "and Integer values" do
      it "yields Integer values incremented by 1 and less than end when not passed a step" do
        (-2...2).step { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-2, -1, 0, 1])
      end

      it "yields Integer values incremented by an Integer step" do
        (-5...5).step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-5, -3, -1, 1, 3])
      end

      ruby_version_is ""..."1.8.7" do
        it "yields truncated Integer values incremented by a Float step" do
          (-2...2).step(1.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2, -1, 0, 1])
        end
      end

      ruby_version_is "1.8.7" do
        it "yields Float values incremented by a Float step" do
          (-2...2).step(1.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2.0, -0.5, 1.0])
        end
      end
    end

    describe "and Float values" do
      it "yields Float values incremented by 1 and less than end when not passed a step" do
        (-2.0...2.0).step { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-2.0, -1.0, 0.0, 1.0])
      end

      it "yields Float values incremented by an Integer step" do
        (-5.0...5.0).step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-5.0, -3.0, -1.0, 1.0, 3.0])
      end

      it "yields Float values incremented by a Float step" do
        (-1.0...1.0).step(0.5) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-1.0, -0.5, 0.0, 0.5])
      end

      ruby_bug "redmine #4576", "1.9.3" do
        it "returns Float values of 'step * n + begin < end'" do
          (1.0...6.4).step(1.8) { |x| ScratchPad << x }
          (1.0...55.6).step(18.2) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([1.0, 2.8, 4.6, 1.0, 19.2, 37.4])
        end
      end
    end

    describe "and Integer, Float values" do
      ruby_version_is ""..."1.8.6" do
        it "yields Integer values incremented by 1 and less than end when not passed a step" do
          (-2...2.0).step { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2, -1, 0, 1])
        end

        it "yields Integer values incremented by an Integer step" do
          (-5...5.0).step(2) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-5, -3, -1, 1, 3])
        end

        it "yields an Integer and then Float values incremented by a Float step" do
          (-1...1.0).step(0.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-1, -0.5, 0.0, 0.5])
        end
      end

      ruby_version_is "1.8.7" do
        it "yields Float values incremented by 1 and less than end when not passed a step" do
          (-2...2.0).step { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-2.0, -1.0, 0.0, 1.0])
        end

        it "yields Float values incremented by an Integer step" do
          (-5...5.0).step(2) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-5.0, -3.0, -1.0, 1.0, 3.0])
        end

        it "yields an Float and then Float values incremented by a Float step" do
          (-1...1.0).step(0.5) { |x| ScratchPad << x }
          ScratchPad.recorded.should eql([-1.0, -0.5, 0.0, 0.5])
        end
      end
    end

    describe "and Float, Integer values" do
      it "yields Float values incremented by 1 and less than end when not passed a step" do
        (-2.0...2).step { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-2.0, -1.0, 0.0, 1.0])
      end

      it "yields Float values incremented by an Integer step" do
        (-5.0...5).step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-5.0, -3.0, -1.0, 1.0, 3.0])
      end

      it "yields Float values incremented by a Float step" do
        (-1.0...1).step(0.5) { |x| ScratchPad << x }
        ScratchPad.recorded.should eql([-1.0, -0.5, 0.0, 0.5])
      end
    end

    describe "and String values" do
      it "yields String values incremented by #succ and less than or equal to end when not passed a step" do
        ("A"..."E").step { |x| ScratchPad << x }
        ScratchPad.recorded.should == ["A", "B", "C", "D"]
      end

      it "yields String values incremented by #succ called Integer step times" do
        ("A"..."G").step(2) { |x| ScratchPad << x }
        ScratchPad.recorded.should == ["A", "C", "E"]
      end

      ruby_version_is ""..."1.8.7" do
        it "yields String values incremented by #succ called Float step times" do
          ("A"..."G").step(2.0) { |x| ScratchPad << x }
          ScratchPad.recorded.should == ["A", "C", "E"]
        end
      end

      ruby_version_is "1.8.7" do
        it "raises a TypeError when passed a Float step" do
          lambda { ("A"..."G").step(2.0) { } }.should raise_error(TypeError)
        end
      end
    end
  end
end
