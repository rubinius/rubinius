require 'mspec/expectations'
require 'mspec/runner/guards'
require 'mspec/runner/formatters/base'

class HtmlFormatter < BaseFormatter
  def print_head
    unless @head_printed
      @out.print <<-EOH
<html>
<head>
<title>Spec Output For #{RUBY_NAME} (#{RUBY_VERSION})</title>
<script type="text/css">
ul {
  list-style: none;
}
.fail {
  color: red;
}
.pass {
  color: green;
}
</script>
</head>
<body>
EOH
      @head_printed = true
    end
  end
  
  def before_describe(msg)
    super
    print_describe
  end
  
  def print_describe
    unless @describe_printed
      @out.print "<div><p>#{@describe}</p>\n<ul>"
      @decribe_printed = true
    end
  end
  
  def after_describe(msg)
    @out.print "</ul>\n</div>"
  end
  
  def after_it(msg)
    if @current.exception
      count = @tally.failures + @tally.errors
      error = if failure?(@current.exception)
        " (FAILED - #{count})"
      else
        " (ERROR - #{count})"
      end
      @out.print %[<li class="fail">- #{@current.it}#{error}</li>]
    else
      @out.print %[<li class="pass">- #{@current.it}</li>]
    end
  end
  
  def print_failure(i,r)
    result = failure?(r.exception) ? "FAILED" : "ERROR"
    @out.print "<li>#{r.describe} #{r.it} #{result}</li>\n"
  end

  def print_backtrace(e)
    if e.message != ""
      @out.print "<p>#{e.message}</p>"

      @out.print "<pre>\n"
      begin
        @out.print e.awesome_backtrace.show
      rescue Exception
        @out.print e.backtrace
      end
      @out.print "</pre>\n"
    end
  end

  def print_summary
    css_class = @exceptions.empty? ? "pass" : "fail"
    @out.print %[<p class="#{css_class}">#{@tally.examples} example]
    @out.print "s" if @tally.examples != 1
    @out.print %[, #{@tally.expectations} expectation]
    @out.print "s" if @tally.expectations != 1
    @out.print %[, #{@tally.failures} failure]
    @out.print "s" if @tally.failures != 1
    @out.print %[, #{@tally.errors} error]
    @out.print "s" if @tally.errors != 1
    @out.print %[</p>\n</body>\n</html>\n]
  end
end
