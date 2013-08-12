require File.expand_path('../../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Enumerator::Generator#each" do
    before(:each) do
      @generator = enumerator_class::Generator.new do |y, *args|
        y << 3 << 2 << 1
        y << args unless args.empty?
        :block_returned
      end
    end

    it "is an enumerable" do
      @generator.should be_kind_of(Enumerable)
    end

    it "supports enumeration with a block" do
      r = []
      @generator.each { |v| r << v }

      r.should == [3, 2, 1]
    end

    it "raises a LocalJumpError if no block given" do
      lambda { @generator.each }.should raise_error(LocalJumpError)
    end

    it "returns the block returned value" do
      @generator.each {}.should equal(:block_returned)
    end

    ruby_version_is "" ... "2.0" do
      it "does not require arguments" do
        enumerator_class::Generator.instance_method(:each).arity.should == 0
      end
    end

    ruby_version_is "2.0" do
      it "requires multiple arguments" do
        enumerator_class::Generator.instance_method(:each).arity.should < 0
      end

      it "appends given arguments to receiver.each" do
        yields = []
        @generator.each(:each0, :each1) { |yielded| yields << yielded }
        yields.should == [3, 2, 1, [:each0, :each1]]
      end
    end
  end
end
