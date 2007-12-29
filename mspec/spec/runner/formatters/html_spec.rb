require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/html'

describe HtmlFormatter do
  before :each do
    @out = CaptureOutput.new
    @formatter = HtmlFormatter.new(@out)
    @exception = Exception.new("something bad")
    @execution = SpecExecution.new
    @execution.describe = "describe"
    @execution.it = "it"
    @execution.exception = @exception
  end
  
  it "responds to print_head with no arguments" do
    @formatter.print_head
    @out.should == "<html>\n<head>\n<title>Spec Output For #{RUBY_NAME} (#{RUBY_VERSION})</title>\n" \
      "<script type=\"text/css\">\nul {\n  list-style: none;\n}\n" \
      ".fail {\n  color: red;\n}\n.pass {\n  color: green;\n}\n</script>\n</head>\n<body>\n"
  end
  
  it "responds to print_describe with no arguments" do
    @formatter.print_describe
    @out.should == "<div><p></p>\n<ul>"
  end

  it "responds to before_describe with one argument" do
    @formatter.before_describe "describe"
    @out.should == "<div><p>describe</p>\n<ul>"
  end
  
  it "responds to after_describe with one argument" do
    @formatter.after_describe "after"
    @out.should == "</ul>\n</div>"
  end
  
  it "responds to after_it with one argument and outputs a pass message" do
    @formatter.before_it "before"
    @formatter.after_it "after"
    @out.should == "<li class=\"pass\">- before</li>"
  end
  
  it "responds to after_it with one argument and outputs a fail message" do
    @formatter.before_it "before"
    @formatter.exception @exception
    @formatter.after_it "after"
    @out.should == "<li class=\"fail\">- before (ERROR - 1)</li>"
  end
  
  it "provides print_failure with an index and SpecExecution instance" do
    @formatter.print_failure(1, @execution)
    @out.should == "<li>describe it FAILED</li>\n"
  end
  
  it "provides print_exception with an Exception instance" do
    @formatter.print_backtrace(@exception)
    @out.should == "<p>something bad</p><pre>\n</pre>\n"
  end
  
  it "provides print_summary when all specs pass" do
    @formatter.print_summary
    @out.should == %[<p class="pass">0 examples, 0 failures</p>\n</body>\n</html>\n]
  end
  
  it "provides print_summary when any spec fails" do
    @formatter.exception @exception
    @formatter.print_summary
    @out.should == %[<p class="fail">0 examples, 1 failures</p>\n</body>\n</html>\n]
  end
end
