
# Example Usage:
#
# ruby -Itools -rtracer blah.rb > spec/profiles/blah.yaml

require 'yaml'

class Tracer
  @@stdout = STDOUT

  EVENT_SYMBOL = {
    :call       => ">",
    :class      => "C",
    :"c-call"   => ">",
  }

  def initialize
    # @counts[klass][method] += 1
    @counts = Hash.new { |h,k| h[k] = Hash.new 0 }
  end

  def on
    set_trace_func method(:trace_func).to_proc

    at_exit {
      off
      print_calls
    }
  end

  def off
    set_trace_func nil
  end

  def print_calls
    # @counts["klass[.|#]"] = [methods]
    counts = Hash.new { |h,k| h[k] = [] }

    @counts.each do |klass, methods|
      pim = klass.public_instance_methods(false).map { |s| s.to_sym }
      methods.each do |method, _|
        name = if pim.include? method then
                 "#{klass}#"
               else
                 "#{klass}."
               end
        counts[name] << method
      end
    end

    y counts
  end

  def trace_func(event, file, line, id, binding, klass, *)
    return if file == __FILE__
    return unless EVENT_SYMBOL[event.to_sym]
    return if klass.nil? or id.nil?

    saved_crit = Thread.critical
    Thread.critical = true

    @counts[klass][id] += 1

    Thread.critical = saved_crit
  end

  Single = new
  def Tracer.on
    Single.on
  end

  def Tracer.off
    Single.off
  end
end

if $0 == __FILE__
  # direct call

  $0 = ARGV[0]
  ARGV.shift
  Tracer.on
  require $0
elsif caller(0).size == 1
  Tracer.on
end
