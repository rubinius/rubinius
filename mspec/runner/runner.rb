require 'mspec/runner/formatters/dotted'

class SpecRunner
  def initialize(formatter=nil)
    @only = []
    @except = []
    @formatter = formatter
    if @formatter == nil
      if formatter = ENV['SPEC_FORMATTER']
        klass = Object.const_get(formatter) rescue nil
        if klass.nil?
          puts "Unable to find formatter '#{formatter}', falling back."
          @formatter = DottedFormatter.new
        else
          @formatter = klass.new
        end
      else
        @formatter = DottedFormatter.new
      end
    end
  end

  def formatter
    @formatter
  end
  
  def formatter=(formatter)
    @formatter = formatter
  end

  def escape(str)
    str.is_a?(Regexp) ? str : Regexp.new(Regexp.escape(str))
  end
  
  def convert_to_regexps(*args)
    args.inject([]) do |list, item|
      if item.is_a?(String) and File.exist?(item)
        if f = File.open(item, "r")
          f.each do |line|
            line.chomp!
            list << escape(line) unless line.empty?
          end
          f.close
        end
        list
      else
        list << escape(item)
      end
    end
  end
  
  def only(*args)
    @only = convert_to_regexps(*args)
  end
  
  def except(*args)
    @except = convert_to_regexps(*args)
  end
  
end
