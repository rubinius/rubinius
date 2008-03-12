require 'mspec/expectations'
require 'mspec/runner/formatters/dotted'

class HtmlFormatter < DottedFormatter
  def register
    super
    MSpec.register :start, self
    MSpec.register :enter, self
    MSpec.register :leave, self
  end
  
  def start
    print <<-EOH
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
  end
  
  def enter(describe)
    print "<div><p>#{describe}</p>\n<ul>\n"
  end
  
  def leave
    print "</ul>\n</div>\n"
  end

  def after(state)
    desc = "- #{state.it}"
    if state.exception?
      @states << state
      count = @tally.failures + @tally.errors - state.exceptions.size
      state.exceptions.each do |msg, exc|
        outcome = state.failure?(exc) ? "FAILED" : "ERROR"
        print %[<li class="fail">#{desc} (#{outcome} - #{count += 1})</li>\n]
      end
    else
      print %[<li class="pass">#{desc}</li>\n]
    end
  end

  def finish
    success = @states.empty?
    print "<ol>" unless success
    @states.each do |state|
      state.exceptions.each do |msg, exc|
        outcome = failure?(state) ? "FAILED" : "ERROR"
        print "\n<li><p>#{state.description} #{outcome}</p>\n<p>"
        print (exc.message.empty? ? "<No message>" : exc.message)
        print "</p>\n<pre>\n"
        print backtrace(exc)
        print "</pre>\n</li>\n"
      end
    end
    print "</ol>\n" unless success
    print %[<p>#{@timer.format}</p>\n]
    print %[<p class="#{success ? "pass" : "fail"}">#{@tally.format}</p>\n]
    print "</body>\n</html>\n"
  end
end
