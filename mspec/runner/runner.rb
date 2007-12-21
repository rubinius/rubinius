require 'mspec/runner/formatters/dotted'

class DescribeState
  def before_all
    @before_all ||= []
  end
  
  def before_each
    @before_each ||= []
  end

  def after_each
    @after_each ||= []
  end
  
  def after_all
    @after_all ||= []
  end
  
  def it
    @it ||= []
  end
end

class SpecRunner
  def initialize(formatter=nil)
    clear_filters
    @stack = []
    @formatter = formatter
    if @formatter.nil?
      @formatter = DottedFormatter.new
    end
    @formatter.start_timer
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
    @only.concat convert_to_regexps(*args)
  end
  
  def except(*args)
    @except.concat convert_to_regexps(*args)
  end
  
  def clear_filters
    @only = []
    @except = []
  end
  
  def skip?(example)
    @except.each { |re| return true if re.match(example) }
    return false if @only.empty?
    matched = @only.any? { |re| re.match(example) }
    return true unless matched
    return false
  end

  def before(at=:each, &block)
    case at
    when :each
      @stack.last.before_each << block
    when :all
      @stack.last.before_all << block
    else
    end
  end
  
  def after(at=:each, &block)
    case at
    when :each
      @stack.last.after_each << block
    when :all
      @stack.last.after_all << block
    end
  end
  
  def it(msg, &block)
    @stack.last.it << [msg, block]
  end
  
  def describe(*args, &block)
    @stack.push DescribeState.new
    dmsg = args.join " "
    formatter.before_describe(dmsg)

    begin
      block.call

      @stack.last.before_all.each { |ba| ba.call }
      @stack.last.it.each do |msg, b|
        unless skip?("#{dmsg} #{msg}")
          formatter.before_it(msg)
          begin
            begin
              @stack.last.before_each.each { |be| be.call }
              b.call
              Mock.verify_count
            rescue Exception => e
              formatter.exception(e)
            ensure
              @stack.last.after_each.each { |ae| ae.call }
              Mock.cleanup
            end
          rescue Exception => e
            formatter.exception(e)
          end
          formatter.after_it(msg)
        end
      end
    ensure
      @stack.last.after_all.each { |aa| aa.call }
      Mock.cleanup
    end

    formatter.after_describe(dmsg)
    @stack.pop
  end
end
