require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Numeric#step" do
  before :each do
    ScratchPad.record []
    @prc = lambda { |x| ScratchPad << x }
  end

  it "raises an ArgumentError when step is 0" do
    lambda { 1.step(5, 0) {} }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when step is 0.0" do
    lambda { 1.step(2, 0.0) {} }.should raise_error(ArgumentError)
  end

  it "defaults to step = 1" do
    1.step(5, &@prc)
    ScratchPad.recorded.should == [1, 2, 3, 4, 5]
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator when step is 0" do
      1.step(2, 0).should be_an_instance_of(enumerator_class)
    end

    it "returns an Enumerator when not passed a block and self > stop" do
      1.step(0, 2).should be_an_instance_of(enumerator_class)
    end

    it "returns an Enumerator when not passed a block and self < stop" do
      1.step(2, 3).should be_an_instance_of(enumerator_class)
    end

    it "returns an Enumerator that uses the given step" do
      0.step(5, 2).to_a.should == [0, 2, 4]
    end
  end

  describe "with [stop, step]" do
    before :each do
      @stop = mock("Numeric#step stop value")
      @step = mock("Numeric#step step value")
      @obj = NumericSpecs::Subclass.new
    end

    ruby_version_is ""..."1.8.7" do
      it "does not raise a LocalJumpError when not passed a block and self > stop" do
        @step.should_receive(:>).with(0).and_return(true)
        @obj.should_receive(:>).with(@stop).and_return(true)
        @obj.step(@stop, @step)
      end

      it "raises a LocalJumpError when not passed a block and self < stop" do
        @step.should_receive(:>).with(0).and_return(true)
        @obj.should_receive(:>).with(@stop).and_return(false)

        lambda { @obj.step(@stop, @step) }.should raise_error(LocalJumpError)
      end
    end

    it "increments self using #+ until self > stop when step > 0" do
      @step.should_receive(:>).with(0).and_return(true)
      @obj.should_receive(:>).with(@stop).and_return(false, false, false, true)
      @obj.should_receive(:+).with(@step).and_return(@obj, @obj, @obj)

      @obj.step(@stop, @step, &@prc)

      ScratchPad.recorded.should == [@obj, @obj, @obj]
    end

    it "decrements self using #+ until self < stop when step < 0" do
      @step.should_receive(:>).with(0).and_return(false)
      @obj.should_receive(:<).with(@stop).and_return(false, false, false, true)
      @obj.should_receive(:+).with(@step).and_return(@obj, @obj, @obj)

      @obj.step(@stop, @step, &@prc)

      ScratchPad.recorded.should == [@obj, @obj, @obj]
    end
  end

  describe "Numeric#step with [stop, step] when self, stop and step are Fixnums" do
    it "yields only Fixnums" do
      1.step(5, 1) { |x| x.should be_kind_of(Fixnum) }
    end
  end

  describe "Numeric#step with [stop, +step] when self, stop and step are Fixnums" do
    it "yields while increasing self by step until stop is reached" do
      1.step(5, 1, &@prc)
      ScratchPad.recorded.should == [1, 2, 3, 4, 5]
    end

    it "yields once when self equals stop" do
      1.step(1, 1, &@prc)
      ScratchPad.recorded.should == [1]
    end

    it "does not yield when self is greater than stop" do
      2.step(1, 1, &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [stop, -step] when self, stop and step are Fixnums" do
    it "yields while decreasing self by step until stop is reached" do
      5.step(1, -1, &@prc)
      ScratchPad.recorded.should == [5, 4, 3, 2, 1]
    end

    it "yields once when self equals stop" do
      5.step(5, -1, &@prc)
      ScratchPad.recorded.should == [5]
    end

    it "does not yield when self is less than stop" do
      1.step(5, -1, &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [stop, step]" do
    it "yields only Floats when self is a Float" do
      1.5.step(5, 1) { |x| x.should be_kind_of(Float) }
    end

    it "yields only Floats when stop is a Float" do
      1.step(5.0, 1) { |x| x.should be_kind_of(Float) }
    end

    it "yields only Floats when step is a Float" do
      1.step(5, 1.0) { |x| x.should be_kind_of(Float) }
    end
  end

  describe "Numeric#step with [stop, +step] when self, stop or step is a Float" do
    it "yields while increasing self by step while stop < stop" do
      1.5.step(5, 1, &@prc)
      ScratchPad.recorded.should == [1.5, 2.5, 3.5, 4.5]
    end

    it "yields once when self equals stop" do
      1.5.step(1.5, 1, &@prc)
      ScratchPad.recorded.should == [1.5]
    end

    it "does not yield when self is greater than stop" do
      2.5.step(1.5, 1, &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [stop, -step] when self, stop or step is a Float" do
    it "yields while decreasing self by step while self > stop" do
      5.step(1.5, -1, &@prc)
      ScratchPad.recorded.should == [5.0, 4.0, 3.0, 2.0]
    end

    it "yields once when self equals stop" do
      1.5.step(1.5, -1, &@prc)
      ScratchPad.recorded.should == [1.5]
    end

    it "does not yield when self is less than stop" do
      1.step(5, -1.5, &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [stop, +Infinity]" do
    ruby_bug "#781", "1.8.7" do
      it "yields once if self < stop" do
        42.step(100, infinity_value, &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when stop is Infinity" do
        42.step(infinity_value, infinity_value, &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self equals stop" do
        42.step(42, infinity_value, &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self and stop are Infinity" do
        (infinity_value).step(infinity_value, infinity_value, &@prc)
        ScratchPad.recorded.should == [infinity_value]
      end
    end

    ruby_bug "#3945", "1.9.2.135" do
      it "does not yield when self > stop" do
        100.step(42, infinity_value, &@prc)
        ScratchPad.recorded.should == []
      end

      it "does not yield when stop is -Infinity" do
        42.step(-infinity_value, infinity_value, &@prc)
        ScratchPad.recorded.should == []
      end
    end
  end

  describe "Numeric#step with [stop, -infinity]" do
    ruby_bug "#3945", "1.9.2.135" do
      it "yields once if self > stop" do
        42.step(6, -infinity_value, &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once if stop is -Infinity" do
        42.step(-infinity_value, -infinity_value, &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self equals stop" do
        42.step(42, -infinity_value, &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self and stop are Infinity" do
        (infinity_value).step(infinity_value, -infinity_value, &@prc)
        ScratchPad.recorded.should == [infinity_value]
      end
    end

    ruby_bug "#781", "1.8.7" do
      it "does not yield when self > stop" do
        42.step(100, -infinity_value, &@prc)
        ScratchPad.recorded.should == []
      end

      it "does not yield when stop is Infinity" do
        42.step(infinity_value, -infinity_value, &@prc)
        ScratchPad.recorded.should == []
      end
    end
  end

  it "does not rescue ArgumentError exceptions" do
    lambda { 1.step(2) { raise ArgumentError, "" }}.should raise_error(ArgumentError)
  end

  it "does not rescue TypeError exceptions" do
    lambda { 1.step(2) { raise TypeError, "" } }.should raise_error(TypeError)
  end
end
