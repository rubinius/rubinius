require 'mspec/expectations/expectations'
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
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
    "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<title>Spec Output For #{RUBY_NAME} (#{RUBY_VERSION})</title>
<style type="text/css">
ul {
  list-style: none;
}
.fail {
  color: red;
}
.pass {
  color: green;
}
#details :target {
  background-color: #ffffe0;
}
</style>
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
      count = @counter.failures + @counter.errors - state.exceptions.size
      state.exceptions.each do |msg, exc|
        outcome = state.failure?(exc) ? "FAILED" : "ERROR"
        count += 1
        print %[<li class="fail">#{desc} (<a href="#details-#{count}">#{outcome} - #{count}</a>)</li>\n]
      end
    else
      print %[<li class="pass">#{desc}</li>\n]
    end
  end

  def finish
    success = @states.empty?
    unless success
      print "<hr>\n"
      print %[<ol id="details">]
      count = 0
      @states.each do |state|
        state.exceptions.each do |msg, exc|
          outcome = failure?(state) ? "FAILED" : "ERROR"
          print %[\n<li id="details-#{count += 1}"><p>#{escape(state.description)} #{outcome}</p>\n<p>]
          print escape(message(exc))
          print "</p>\n<pre>\n"
          print escape(backtrace(exc))
          print "</pre>\n</li>\n"
        end
      end
      print "</ol>\n"
    end
    print %[<p>#{@timer.format}</p>\n]
    print %[<p class="#{success ? "pass" : "fail"}">#{@tally.format}</p>\n]
    print "</body>\n</html>\n"
  end

  def escape(string)
    string.gsub("&", "&nbsp;").gsub("<", "&lt;").gsub(">", "&gt;")
  end
end
