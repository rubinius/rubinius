# -*- encoding: utf-8 -*-
describe :encode_string, :shared => true do
  it "transcodes to the default internal encoding with no argument" do
    begin
      old_default_internal = Encoding.default_internal
      Encoding.default_internal = Encoding::EUC_JP
      str = "問か".force_encoding('utf-8')
      str.encoding.should_not == Encoding.default_internal
      str.encode.encoding.should == Encoding.default_internal
    ensure
      Encoding.default_internal = old_default_internal
    end
  end

  it "accepts a target encoding name as a String for an argument" do
    str = "Füll"
    lambda do
      str.send(@method, 'binary')
    end.should_not raise_error(ArgumentError)
  end

  it "accepts a target encoding as an Encoding for an argument" do
    str = "Füll"
    lambda do
      str.send(@method, Encoding::BINARY)
    end.should_not raise_error(ArgumentError)
  end

  it "accepts a target and source encoding given as Encoding objects" do
    str = "Füll"
    lambda do
      str.send(@method, Encoding::ASCII, Encoding::BINARY)
    end.should_not raise_error(ArgumentError)
  end

  it "accepts a target and source encoding given as Strings" do
    str = "Füll"
    lambda do
      str.send(@method, 'ascii', 'binary')
    end.should_not raise_error(ArgumentError)
  end

  it "accepts a target encoding given as an Encoding, and a source encoding given as a String" do
    str = "Füll"
    lambda do
      str.send(@method, Encoding::ASCII, 'binary')
    end.should_not raise_error(ArgumentError)
  end

  it "accepts a target encoding given as a String, and a source encoding given as an Encoding" do
    str = "Füll"
    lambda do
      str.send(@method, 'ascii', Encoding::BINARY)
    end.should_not raise_error(ArgumentError)
  end

  it "accepts an options hash as the final argument" do
    lambda do
      "yes".send(@method, 'ascii', Encoding::BINARY, {:xml => :text})
    end.should_not raise_error(ArgumentError)

    lambda do
      "yes".send(@method, Encoding::BINARY, {:xml => :text})
    end.should_not raise_error(ArgumentError)
  end

  it "transcodes self to the given encoding" do
    str = "\u3042".force_encoding('UTF-8')
    str.send(@method, Encoding::EUC_JP).should == "\xA4\xA2".force_encoding('EUC-JP')
  end

  it "can convert between encodings where a multi-stage conversion path is needed" do
    str = "big?".force_encoding(Encoding::US_ASCII)
    str.send(@method, Encoding::Big5, Encoding::US_ASCII).encoding.should == Encoding::Big5
  end

  it "raises an Encoding::InvalidByteSequenceError for invalid byte sequences" do
    lambda do
      "\xa4".force_encoding(Encoding::EUC_JP).send(@method, 'iso-8859-1')
    end.should raise_error(Encoding::InvalidByteSequenceError)
  end

  it "raises UndefinedConversionError if the String contains characters invalid for the target     encoding" do
    str = "\u{6543}"
    lambda do
      str.send(@method, Encoding.find('macCyrillic'))
    end.should raise_error(Encoding::UndefinedConversionError)
  end

  it "raises Encoding::ConverterNotFoundError for invalid target encodings" do
    lambda do
      "\u{9878}".send(@method, 'xyz')
    end.should raise_error(Encoding::ConverterNotFoundError)
  end

  it "replaces invalid characters" do
    str = "\xa4"
    str.send(@method, "ISO-8859-1", "UTF-8", :invalid => :replace).should == "?"
  end

  it "replaces undefined characters" do
    "abc\u{fffd}def".send(@method, "EUC-JP", "UTF-8", {:undef => :replace, :replace => ""}).should == "abcdef"
  end

  it "replaces xml characters" do
    '<xml>bed & breakfast</xml>'.send(@method, 'UTF-8', {:xml => :text}).should == "&lt;xml&gt;bed &amp; breakfast&lt;/xml&gt;"
  end

  it "replaces xml characters and quotes the result" do
    '<xml>bed & breakfast</xml>'.send(@method, 'UTF-8', {:xml => :attr}).should == "\"&lt;xml&gt;bed &amp; breakfast&lt;/xml&gt;\""
  end
end
