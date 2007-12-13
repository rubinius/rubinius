# Class that allows global variables to have aliases
class GlobalVariables
  def initialize
    load_path = ["lib", "stdlib", "."]
    loaded_features = []

    @internal = {
      :$; => nil,
      :$/ => "\n",                # Input record separator
      :$\ => nil,                 # Output record separator
      :$> => STDOUT,
      :$: => load_path,
      :$LOAD_PATH => load_path,
      :$" => loaded_features,
      :$LOADED_FEATURES => loaded_features,
      :$, => '',                  # Output field separator
      :$stderr => STDERR,
      :$stdout => STDOUT,
      :$CONSOLE => STDOUT,
      :$DEBUG => false
    }
    @alias = {}
  end

  def key?(key)
    @internal.key?(key) || @alias.key?(key)
  end

  def [](key)
    if val = @internal[key]
      return val
    else
      return @internal[@alias[key]]
    end
  end

  def []=(key, data)
    if !@internal[key] && val = @alias[key]
      return @internal[val] = data
    else
      return @internal[key] = data
    end
  end

  def add_alias(a, b)
    if @internal[a]
      @alias[b] = a
    elsif val = @alias[a]
      @alias[b] = val
    else
      raise NameError, "Error finding global variable #{a} while " +
                       "attempting to alias it"
    end
  end
end

Globals = GlobalVariables.new
