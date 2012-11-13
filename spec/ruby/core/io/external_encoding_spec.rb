require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe :io_external_encoding_write, :shared => true do
    it "returns nil if the external encoding is not set" do
      @io = new_io @name, @object
      @io.external_encoding.should be_nil
    end

    it "returns returns nil if the external encoding is not set regardless of Encoding.default_external changes" do
      @io = new_io @name, @object
      Encoding.default_external = Encoding::IBM437
      @io.external_encoding.should be_nil
    end

    it "returns returns nil when Encoding.default_external is set when the instance is created" do
      Encoding.default_external = Encoding::IBM437
      @io = new_io @name, @object
      @io.external_encoding.should be_nil
    end

    it "returns the external encoding specified when the instance was created regardless of Encoding.default_external changes" do
      @io = new_io @name, "#{@object}:ibm866"
      Encoding.default_external = Encoding::IBM437
      @io.external_encoding.should equal(Encoding::IBM866)
    end

    it "returns the encoding set by #set_encoding" do
      @io = new_io @name, "#{@object}:ibm866"
      @io.set_encoding Encoding::EUC_JP, nil
      @io.external_encoding.should equal(Encoding::EUC_JP)
    end
  end

  describe "IO#external_encoding" do
    before :each do
      @external = Encoding.default_external

      @name = tmp("io_external_encoding")
      touch(@name)
    end

    after :each do
      Encoding.default_external = @external

      @io.close if @io and not @io.closed?
      rm_r @name
    end

    describe "with 'r' mode" do
      it "returns Encoding.default_external if the external encoding is not set" do
        @io = new_io @name, "r"
        @io.external_encoding.should equal(Encoding.default_external)
      end

      it "returns Encoding.default_external when that encoding is changed after the instance is created" do
        Encoding.default_external = Encoding::UTF_8
        @io = new_io @name, "r"
        Encoding.default_external = Encoding::IBM437
        @io.external_encoding.should equal(Encoding::IBM437)
      end

      it "returns the external encoding specified when the instance was created regardless of Encoding.default_external changes" do
        @io = new_io @name, "r:utf-8"
        Encoding.default_external = Encoding::IBM437
        @io.external_encoding.should equal(Encoding::UTF_8)
      end

      it "returns the encoding set by #set_encoding" do
        @io = new_io @name, "r:utf-8"
        @io.set_encoding Encoding::EUC_JP, nil
        @io.external_encoding.should equal(Encoding::EUC_JP)
      end
    end

    describe "with 'r+' mode" do
      it_behaves_like :io_external_encoding_write, nil, "r+"
    end

    describe "with 'w' mode" do
      it_behaves_like :io_external_encoding_write, nil, "w"
    end

    describe "with 'w+' mode" do
      it_behaves_like :io_external_encoding_write, nil, "w+"
    end

    describe "with 'a' mode" do
      it_behaves_like :io_external_encoding_write, nil, "a"
    end

    describe "with 'a+' mode" do
      it_behaves_like :io_external_encoding_write, nil, "a+"
    end
  end
end
