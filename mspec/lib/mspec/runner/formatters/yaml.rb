require 'mspec/expectations/expectations'
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

    print "---\n"
    print "exceptions:\n"
    @states.each do |state|
      state.exceptions.each do |msg, exc|
        outcome = failure?(state) ? "FAILED" : "ERROR"
        str =  "#{state.description} #{outcome}\n"
        str << "#{exc.class.name} occurred during: #{msg}\n" if msg
        str << message(exc)
        str << backtrace(exc)
        print "- ", str.inspect, "\n"
      end
    end

    print "time: ",         @timer.elapsed,              "\n"
    print "files: ",        @tally.counter.files,        "\n"
    print "examples: ",     @tally.counter.examples,     "\n"
    print "expectations: ", @tally.counter.expectations, "\n"
    print "failures: ",     @tally.counter.failures,     "\n"
    print "errors: ",       @tally.counter.errors,       "\n"
  end
end
