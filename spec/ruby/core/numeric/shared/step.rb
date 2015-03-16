require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../helpers/step', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

describe :numeric_step, :shared => true do
  before :each do
    ScratchPad.record []
    @prc = lambda { |x| ScratchPad << x }
  end

  it "defaults to step = 1" do
    1.send(@method, *get_args(@args_type, 5), &@prc)
    ScratchPad.recorded.should == [1, 2, 3, 4, 5]
  end

  it "returns an Enumerator when step is 0" do
    1.send(@method, *get_args(@args_type, 2, 0)).should be_an_instance_of(enumerator_class)
  end

  it "returns an Enumerator when not passed a block and self > stop" do
    1.send(@method, *get_args(@args_type, 0, 2)).should be_an_instance_of(enumerator_class)
  end

  it "returns an Enumerator when not passed a block and self < stop" do
    1.send(@method, *get_args(@args_type, 2, 3)).should be_an_instance_of(enumerator_class)
  end

  it "returns an Enumerator that uses the given step" do
    0.send(@method, *get_args(@args_type, 5, 2)).to_a.should == [0, 2, 4]
  end

  describe "Numeric#step with [stop, step] when self, stop and step are Fixnums" do
    it "yields only Fixnums" do
      1.send(@method, *get_args(@args_type, 5, 1)) { |x| x.should be_an_instance_of(Fixnum) }
    end
  end

  describe "Numeric#step with [stop, step] when self and stop are Fixnums but step is a String" do
    it "returns an Enumerator if not given a block" do
      1.send(@method, *get_args(@args_type, 5, "1")).should be_an_instance_of(enumerator_class)
      1.send(@method, *get_args(@args_type, 5, "0.1")).should be_an_instance_of(enumerator_class)
      1.send(@method, *get_args(@args_type, 5, "1/3")).should be_an_instance_of(enumerator_class)
      1.send(@method, *get_args(@args_type, 5, "foo")).should be_an_instance_of(enumerator_class)
    end

    it "raises an ArgumentError if given a block" do
      lambda { 1.send(@method, *get_args(@args_type, 5, "1")) {} }.should raise_error(ArgumentError)
      lambda { 1.send(@method, *get_args(@args_type, 5, "0.1")) {} }.should raise_error(ArgumentError)
      lambda { 1.send(@method, *get_args(@args_type, 5, "1/3")) {} }.should raise_error(ArgumentError)
      lambda { 1.send(@method, *get_args(@args_type, 5, "foo")) {} }.should raise_error(ArgumentError)
    end
  end

  describe "Numeric#step with [stop, step] when self and stop are Floats but step is a String" do
    it "returns an Enumerator if not given a block" do
      1.1.send(@method, *get_args(@args_type, 5.1, "1")).should be_an_instance_of(enumerator_class)
      1.1.send(@method, *get_args(@args_type, 5.1, "0.1")).should be_an_instance_of(enumerator_class)
      1.1.send(@method, *get_args(@args_type, 5.1, "1/3")).should be_an_instance_of(enumerator_class)
      1.1.send(@method, *get_args(@args_type, 5.1, "foo")).should be_an_instance_of(enumerator_class)
    end

    it "raises a ArgumentError if given a block" do
      lambda { 1.1.send(@method, *get_args(@args_type, 5.1, "1")) {} }.should raise_error(ArgumentError)
      lambda { 1.1.send(@method, *get_args(@args_type, 5.1, "0.1")) {} }.should raise_error(ArgumentError)
      lambda { 1.1.send(@method, *get_args(@args_type, 5.1, "1/3")) {} }.should raise_error(ArgumentError)
      lambda { 1.1.send(@method, *get_args(@args_type, 5.1, "foo")) {} }.should raise_error(ArgumentError)
    end
  end

  describe "Numeric#step with [stop, +step] when self, stop and step are Fixnums" do
    it "yields while increasing self by step until stop is reached" do
      1.send(@method, *get_args(@args_type, 5, 1), &@prc)
      ScratchPad.recorded.should == [1, 2, 3, 4, 5]
    end

    it "yields once when self equals stop" do
      1.send(@method, *get_args(@args_type, 1, 1), &@prc)
      ScratchPad.recorded.should == [1]
    end

    it "does not yield when self is greater than stop" do
      2.send(@method, *get_args(@args_type, 1, 1), &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [stop, -step] when self, stop and step are Fixnums" do
    it "yields while decreasing self by step until stop is reached" do
      5.send(@method, *get_args(@args_type, 1, -1), &@prc)
      ScratchPad.recorded.should == [5, 4, 3, 2, 1]
    end

    it "yields once when self equals stop" do
      5.send(@method, *get_args(@args_type, 5, -1), &@prc)
      ScratchPad.recorded.should == [5]
    end

    it "does not yield when self is less than stop" do
      1.send(@method, *get_args(@args_type, 5, -1), &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [stop, step]" do
    it "yields only Floats when self is a Float" do
      1.5.send(@method, *get_args(@args_type, 5, 1)) { |x| x.should be_an_instance_of(Float) }
    end

    it "yields only Floats when stop is a Float" do
      1.send(@method, *get_args(@args_type, 5.0, 1)) { |x| x.should be_an_instance_of(Float) }
    end

    it "yields only Floats when step is a Float" do
      1.send(@method, *get_args(@args_type, 5, 1.0)) { |x| x.should be_an_instance_of(Float) }
    end
  end

  describe "Numeric#step with [stop, +step] when self, stop or step is a Float" do
    it "yields while increasing self by step while < stop" do
      1.5.send(@method, *get_args(@args_type, 5, 1), &@prc)
      ScratchPad.recorded.should == [1.5, 2.5, 3.5, 4.5]
    end

    it "yields once when self equals stop" do
      1.5.send(@method, *get_args(@args_type, 1.5, 1), &@prc)
      ScratchPad.recorded.should == [1.5]
    end

    it "does not yield when self is greater than stop" do
      2.5.send(@method, *get_args(@args_type, 1.5, 1), &@prc)
      ScratchPad.recorded.should == []
    end

    ruby_bug "redmine #4576", "1.9.3" do
      it "is careful about not yielding a value greater than limit" do
        # As 9*1.3+1.0 == 12.700000000000001 > 12.7, we test:
        1.0.send(@method, *get_args(@args_type, 12.7, 1.3), &@prc)
        ScratchPad.recorded.should eql [1.0, 2.3, 3.6, 4.9, 6.2, 7.5, 8.8, 10.1, 11.4, 12.7]
      end
    end
  end

  describe "Numeric#step with [stop, -step] when self, stop or step is a Float" do
    it "yields while decreasing self by step while self > stop" do
      5.send(@method, *get_args(@args_type, 1.5, -1), &@prc)
      ScratchPad.recorded.should == [5.0, 4.0, 3.0, 2.0]
    end

    it "yields once when self equals stop" do
      1.5.send(@method, *get_args(@args_type, 1.5, -1), &@prc)
      ScratchPad.recorded.should == [1.5]
    end

    it "does not yield when self is less than stop" do
      1.send(@method, *get_args(@args_type, 5, -1.5), &@prc)
      ScratchPad.recorded.should == []
    end

    ruby_bug "redmine #4576", "1.9.3" do
      it "is careful about not yielding a value smaller than limit" do
        # As -9*1.3-1.0 == -12.700000000000001 < -12.7, we test:
        -1.0.send(@method, *get_args(@args_type, -12.7, -1.3), &@prc)
        ScratchPad.recorded.should eql [-1.0, -2.3, -3.6, -4.9, -6.2, -7.5, -8.8, -10.1, -11.4, -12.7]
      end
    end
  end

  describe "Numeric#step with [stop, +Infinity]" do
    ruby_bug "#781", "1.8.7" do
      it "yields once if self < stop" do
        42.send(@method, *get_args(@args_type, 100, infinity_value), &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when stop is Infinity" do
        42.send(@method, *get_args(@args_type, infinity_value, infinity_value), &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self equals stop" do
        42.send(@method, *get_args(@args_type, 42, infinity_value), &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self and stop are Infinity" do
        (infinity_value).send(@method, *get_args(@args_type, infinity_value, infinity_value), &@prc)
        ScratchPad.recorded.should == [infinity_value]
      end
    end

    ruby_bug "#3945", "1.9.2.135" do
      it "does not yield when self > stop" do
        100.send(@method, *get_args(@args_type, 42, infinity_value), &@prc)
        ScratchPad.recorded.should == []
      end

      it "does not yield when stop is -Infinity" do
        42.send(@method, *get_args(@args_type, -infinity_value, infinity_value), &@prc)
        ScratchPad.recorded.should == []
      end
    end
  end

  describe "Numeric#step with [stop, -infinity]" do
    ruby_bug "#3945", "1.9.2.135" do
      it "yields once if self > stop" do
        42.send(@method, *get_args(@args_type, 6, -infinity_value), &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once if stop is -Infinity" do
        42.send(@method, *get_args(@args_type, -infinity_value, -infinity_value), &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self equals stop" do
        42.send(@method, *get_args(@args_type, 42, -infinity_value), &@prc)
        ScratchPad.recorded.should == [42]
      end

      it "yields once when self and stop are Infinity" do
        (infinity_value).send(@method, *get_args(@args_type, infinity_value, -infinity_value), &@prc)
        ScratchPad.recorded.should == [infinity_value]
      end
    end

    ruby_bug "#781", "1.8.7" do
      it "does not yield when self > stop" do
        42.send(@method, *get_args(@args_type, 100, -infinity_value), &@prc)
        ScratchPad.recorded.should == []
      end

      it "does not yield when stop is Infinity" do
        42.send(@method, *get_args(@args_type, infinity_value, -infinity_value), &@prc)
        ScratchPad.recorded.should == []
      end
    end
  end

  describe "Numeric#step with [infinity, -step]" do
    it "does not yield when self is -infinity" do
      (-infinity_value).send(@method, *get_args(@args_type, infinity_value, -1), &@prc)
      ScratchPad.recorded.should == []
    end

    it "does not yield when self is +infinity" do
      infinity_value.send(@method, *get_args(@args_type, infinity_value, -1), &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [infinity, step]" do
    it "does not yield when self is infinity" do
      (infinity_value).send(@method, *get_args(@args_type, infinity_value, 1), &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [-infinity, step]" do
    it "does not yield when self is -infinity" do
      (-infinity_value).send(@method, *get_args(@args_type, -infinity_value, 1), &@prc)
      ScratchPad.recorded.should == []
    end
  end

  describe "Numeric#step with [-infinity, -step]" do
    it "does not yield when self is -infinity" do
      (-infinity_value).send(@method, *get_args(@args_type, -infinity_value, -1), &@prc)
      ScratchPad.recorded.should == []
    end
  end

  it "does not rescue ArgumentError exceptions" do
    lambda { 1.send(@method, *get_args(@args_type, 2)) { raise ArgumentError, "" }}.should raise_error(ArgumentError)
  end

  it "does not rescue TypeError exceptions" do
    lambda { 1.send(@method, *get_args(@args_type, 2)) { raise TypeError, "" } }.should raise_error(TypeError)
  end

  describe "when no block is given" do
    describe "returned Enumerator" do
      describe "size" do
        describe "when self, stop and step are Fixnums and step is positive" do
          it "returns the difference between self and stop divided by the number of steps" do
            5.send(@method, *get_args(@args_type, 10, 11)).size.should == 1
            5.send(@method, *get_args(@args_type, 10, 6)).size.should == 1
            5.send(@method, *get_args(@args_type, 10, 5)).size.should == 2
            5.send(@method, *get_args(@args_type, 10, 4)).size.should == 2
            5.send(@method, *get_args(@args_type, 10, 2)).size.should == 3
            5.send(@method, *get_args(@args_type, 10, 1)).size.should == 6
            5.send(@method, *get_args(@args_type, 10)).size.should == 6
            10.send(@method, *get_args(@args_type, 10, 1)).size.should == 1
          end

          it "returns 0 if value > limit" do
            11.send(@method, *get_args(@args_type, 10, 1)).size.should == 0
          end
        end

        describe "when self, stop and step are Fixnums and step is negative" do
          it "returns the difference between self and stop divided by the number of steps" do
            10.send(@method, *get_args(@args_type, 5, -11)).size.should == 1
            10.send(@method, *get_args(@args_type, 5, -6)).size.should == 1
            10.send(@method, *get_args(@args_type, 5, -5)).size.should == 2
            10.send(@method, *get_args(@args_type, 5, -4)).size.should == 2
            10.send(@method, *get_args(@args_type, 5, -2)).size.should == 3
            10.send(@method, *get_args(@args_type, 5, -1)).size.should == 6
            10.send(@method, *get_args(@args_type, 10, -1)).size.should == 1
          end

          it "returns 0 if value < limit" do
            10.send(@method, *get_args(@args_type, 11, -1)).size.should == 0
          end
        end

        describe "when self, stop or step is a Float" do
          describe "and step is positive" do
            it "returns the difference between self and stop divided by the number of steps" do
              5.send(@method, *get_args(@args_type, 10, 11.0)).size.should == 1
              5.send(@method, *get_args(@args_type, 10, 6.0)).size.should == 1
              5.send(@method, *get_args(@args_type, 10, 5.0)).size.should == 2
              5.send(@method, *get_args(@args_type, 10, 4.0)).size.should == 2
              5.send(@method, *get_args(@args_type, 10, 2.0)).size.should == 3
              5.send(@method, *get_args(@args_type, 10, 0.5)).size.should == 11
              5.send(@method, *get_args(@args_type, 10, 1.0)).size.should == 6
              5.send(@method, *get_args(@args_type, 10.5)).size.should == 6
              10.send(@method, *get_args(@args_type, 10, 1.0)).size.should == 1
            end

            it "returns 0 if value > limit" do
              10.send(@method, *get_args(@args_type, 5.5)).size.should == 0
              11.send(@method, *get_args(@args_type, 10, 1.0)).size.should == 0
              11.send(@method, *get_args(@args_type, 10, 1.5)).size.should == 0
              10.send(@method, *get_args(@args_type, 5, Float::INFINITY)).size.should == 0
            end

            it "returns 1 if step is Float::INFINITY" do
              5.send(@method, *get_args(@args_type, 10, Float::INFINITY)).size.should == 1
            end
          end

          describe "and step is negative" do
            it "returns the difference between self and stop divided by the number of steps" do
              10.send(@method, *get_args(@args_type, 5, -11.0)).size.should == 1
              10.send(@method, *get_args(@args_type, 5, -6.0)).size.should == 1
              10.send(@method, *get_args(@args_type, 5, -5.0)).size.should == 2
              10.send(@method, *get_args(@args_type, 5, -4.0)).size.should == 2
              10.send(@method, *get_args(@args_type, 5, -2.0)).size.should == 3
              10.send(@method, *get_args(@args_type, 5, -0.5)).size.should == 11
              10.send(@method, *get_args(@args_type, 5, -1.0)).size.should == 6
              10.send(@method, *get_args(@args_type, 10, -1.0)).size.should == 1
            end

            it "returns 0 if value < limit" do
              10.send(@method, *get_args(@args_type, 11, -1.0)).size.should == 0
              10.send(@method, *get_args(@args_type, 11, -1.5)).size.should == 0
              5.send(@method, *get_args(@args_type, 10, -Float::INFINITY)).size.should == 0
            end

            it "returns 1 if step is Float::INFINITY" do
              10.send(@method, *get_args(@args_type, 5, -Float::INFINITY)).size.should == 1
            end
          end
        end
      end
    end
  end
end