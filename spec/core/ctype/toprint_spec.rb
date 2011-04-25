require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#toprint" do
  it "returns escaped control characters" do
    a = []
    [?\n, ?\t, ?\a, ?\v, ?\f, ?\r, ?\e, ?\b].each do |c|
      a << Rubinius::CType.toprint(c)
    end

    a.should == ["\\n", "\\t", "\\a", "\\v", "\\f", "\\r", "\\e", "\\b"]
  end

  it "returns '\\\"' for '?\"'" do
    Rubinius::CType.toprint(?").should == "\\\""
  end

  it "returns '\\\\' for '?\\'" do
    Rubinius::CType.toprint(?\\).should == "\\\\"
  end

  it "returns a table of transforms for '?#'" do
    table = Rubinius::Tuple['#$', '\#$', '#@', '\#@', '#{', '\#{', '#', '#']
    Rubinius::CType.toprint(?#).to_a.should == table.to_a
  end

  it "returns an octal value for values 0..31 except for control characters" do
    a = []
    (0..31).each { |c| a << Rubinius::CType.toprint(c) }
    a.should == [
      "\\000", "\\001", "\\002", "\\003", "\\004", "\\005", "\\006",
      "\\a", "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "\\016", "\\017",
      "\\020", "\\021", "\\022", "\\023", "\\024", "\\025", "\\026",
      "\\027", "\\030", "\\031", "\\032", "\\e", "\\034", "\\035", "\\036", "\\037"
    ]
  end

  it "returns the octal value for values 127..255" do
    (127..255).each do |c|
      Rubinius::CType.toprint(c).should == ("\\%03o" % c.to_s(8).oct)
    end
  end

  it "returns a regular character values 32..126 except for \\, \#, \"" do
    (32..126).each do |c|
      Rubinius::CType.toprint(c).should == c.chr unless "\\\#\"".include? c.chr
    end
  end
end
