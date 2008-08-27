# depends on: class.rb

##
# Stores global variables and global variable aliases.

class GlobalVariables
  def initialize
    load_path = %w[.]
    loaded_features = []

    @internal = LookupTable.new(
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
      :$stdin => STDIN,
      :$CONSOLE => STDOUT,
      :$DEBUG => false,
      :$SAFE => 0,
      :$. => 0,    # TODO: Last line number of IO read.
      :$_ => nil,  # HACK: bunk for now.
      :$< => nil,  # HACK: should be ARGF
      :$? => nil,  # Process status. nil until set
      :$= => false # ignore case, whatever that is
    )

    @alias = LookupTable.new
    @hooks = LookupTable.new
  end

  def key?(key)
    @internal.key?(key) || @alias.key?(key)
  end

  def variables
    @internal.keys + @alias.keys
  end

  def [](key)
    if @internal.key? key
      @internal[key]
    elsif @hooks.key? key
      @hooks[key][0].call(key)
    elsif @alias.key? key
      @internal[@alias[key]]
    end
  end

  def []=(key, data)
    if !@internal.key?(key) && alias_key = @alias[key] then
      @internal[alias_key] = data
    elsif @hooks.key? key then
      @hooks[key][1].arity == 1 ? @hooks[key][1].call(data) : @hooks[key][1].call(data, key)
    else
      @internal[key] = data
    end
  end

  def add_alias(from, to)
    if hook = @hooks[from]
      @hooks[to] = hook
    elsif alias_key = @alias[from] then
      @alias[to] = alias_key
    else
      @alias[to] = from
    end
  end

  def illegal_set
    raise RuntimeError, "unable to set global"
  end
  
  def set_hook(var, getter, setter)
    unless getter.respond_to?(:call)
      raise ArgumentError, "getter must respond to call"
    end
    
    if setter.nil?
      setter = method(:illegal_set)
    elsif not setter.respond_to?(:call)
      raise ArgumentError, "setter must respond to call"
    end
    
    @hooks[var] = [getter, setter]
  end
end

Globals = GlobalVariables.new
