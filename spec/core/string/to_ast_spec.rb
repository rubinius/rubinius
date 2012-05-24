require File.expand_path('../../../spec_helper', __FILE__)

describe "String#to_ast" do
  it "converts to AST nodes from Ruby code" do
    "true".to_ast.should be_kind_of(Rubinius::AST::TrueLiteral)
  end

  it "raises SyntaxError with invalid Ruby code " do
    lambda { "{".to_ast }.should raise_error(SyntaxError)
  end

  ruby_version_is "1.9" do
    it "converts to AST nodes from Ruby 1.9 code" do
      "->{}".to_ast.should be_kind_of(Rubinius::AST::Send)
    end
  end
end
