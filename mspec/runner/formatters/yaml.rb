require 'mspec/expectations'
require 'mspec/runner/formatters/dotted'

class YamlFormatter < DottedFormatter
  def initialize(out=nil)
    @states = []
    @out = $stdout

    if out.nil?
      @finish = $stdout
    else
      @finish = File.open out, "w"
    end
  end

  def switch
    @out = @finish
  end

  def finish
    switch

    result = { "exceptions" => [] }
    @states.each do |state|
      state.exceptions.each do |msg, exc|
        outcome = failure?(state) ? "FAILED" : "ERROR"
        str =  "#{state.description} #{outcome}\n"
        str << "Exception occurred during: #{msg}\n" if msg
        str << backtrace(exc)
        result["exceptions"] << str
      end
    end

    result["time"]         = @timer.elapsed
    result["examples"]     = @tally.examples
    result["expectations"] = @tally.expectations
    result["failures"]     = @tally.failures
    result["errors"]       = @tally.errors

    require 'yaml'
    print result.to_yaml
  end
end
