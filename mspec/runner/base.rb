require 'mspec/runner/formatters/dotted'

class Object
  def formatter
    if @formatter.nil?
      @formatter = DottedFormatter.new
    end
    @formatter
  end
    
  def it(msg)
    state = SpecState.new(@describe, msg)
    begin
      yield
    rescue Exception => e
      state.exceptions << [nil, e]
    end
    formatter.after(state)
  end

  def describe(mod, msg)
    @describe = desc ? "#{mod} #{desc}" : mod.to_s
    yield
  end
end
