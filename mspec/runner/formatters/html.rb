require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../guards'
require File.dirname(__FILE__) + '/base'

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
    if @report.exception
      error = if @report.exception.is_a?(ExpectationNotMetError)
        " (FAILED - #{@failures})"
      else
        " (ERROR - #{@failures})"
      end
      @out.print %[<li class="fail">- #{@report.it}#{error}</li>]
    else
      @out.print %[<li class="pass">- #{@report.it}</li>]
    end
  end
  
  def print_failure(i,r)
    @out.print "<li>#{r.describe} #{r.it} FAILED</li>\n"
  end

  def print_backtrace(e)
    if e.message != ""
      @out.print "<p>#{e.message}</p>"

      @out.print "<pre>\n"
      begin
        @out.print e.backtrace.show
      rescue Exception
        @out.print e.backtrace
      end
      @out.print "</pre>\n"
    end
  end

  def print_summary
    css_class = @exceptions.empty? ? "pass" : "fail"
    @out.print %[<p class="#{css_class}">#{@examples} examples, #{@failures} failures</p>\n</body>\n</html>\n]
  end
end
