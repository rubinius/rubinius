require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe :io_internal_encoding, :shared => true do
    describe "when Encoding.default_internal is not set" do
      before :each do
        Encoding.default_internal = nil
      end

      it "returns nil if the internal encoding is not set" do
        @io = new_io @name, @object
        @io.internal_encoding.should be_nil
      end

      it "returns nil if Encoding.default_internal is changed after the instance is created" do
        @io = new_io @name, @object
        Encoding.default_internal = Encoding::IBM437
        @io.internal_encoding.should be_nil
      end

      it "returns the value set when the instance was created" do
        @io = new_io @name, "#{@object}:utf-8:euc-jp"
        Encoding.default_internal = Encoding::IBM437
        @io.internal_encoding.should equal(Encoding::EUC_JP)
      end

      it "returns the value set by #set_encoding" do
        @io = new_io @name, @object
        @io.set_encoding(Encoding::US_ASCII, Encoding::IBM437)
        @io.internal_encoding.should equal(Encoding::IBM437)
      end
    end

    describe "when Encoding.default_internal is set" do
      before :each do
        Encoding.default_internal = Encoding::IBM866
      end

      it "returns the value of Encoding.default_internal when the instance was created if the internal encoding is not set" do
        @io = new_io @name, @object
        @io.internal_encoding.should equal(Encoding::IBM866)
      end

      it "does not change when Encoding.default_internal is changed" do
        @io = new_io @name, @object
        Encoding.default_internal = Encoding::IBM437
        @io.internal_encoding.should equal(Encoding::IBM866)
      end

      it "returns the internal encoding set when the instance was created" do
        @io = new_io @name, "#{@object}:utf-8:euc-jp"
        @io.internal_encoding.should equal(Encoding::EUC_JP)
      end

      it "does not change when set and Encoding.default_internal is changed" do
        @io = new_io @name, "#{@object}:utf-8:euc-jp"
        Encoding.default_internal = Encoding::IBM437
        @io.internal_encoding.should equal(Encoding::EUC_JP)
      end

      it "returns the value set by #set_encoding" do
        @io = new_io @name, @object
        @io.set_encoding(Encoding::US_ASCII, Encoding::IBM437)
        @io.internal_encoding.should equal(Encoding::IBM437)
      end
    end
  end

  describe "IO#internal_encoding" do
    before :each do
      @internal = Encoding.default_internal

      @name = tmp("io_internal_encoding")
      touch(@name)
    end

    after :each do
      Encoding.default_internal = @internal

      @io.close if @io and not @io.closed?
      rm_r @name
    end

    describe "with 'r' mode" do
      it_behaves_like :io_internal_encoding, nil, "r"
    end

    describe "with 'r+' mode" do
      it_behaves_like :io_internal_encoding, nil, "r+"
    end

    describe "with 'w' mode" do
      it_behaves_like :io_internal_encoding, nil, "w"
    end

    describe "with 'w+' mode" do
      it_behaves_like :io_internal_encoding, nil, "w+"
    end

    describe "with 'a' mode" do
      it_behaves_like :io_internal_encoding, nil, "a"
    end

    describe "with 'a+' mode" do
      it_behaves_like :io_internal_encoding, nil, "a+"
    end
  end
end
