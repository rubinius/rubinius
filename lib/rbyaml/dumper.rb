require 'rbyaml/constants'
require 'rbyaml/emitter'
require 'rbyaml/serializer'
require 'rbyaml/representer'
require 'rbyaml/resolver'

module RbYAML
  class CommonDumper
    attr_accessor :emitter, :serializer, :representer, :resolver
    def initialize(stream,opts={})
      super()
      opts = DEFAULTS.merge(opts)
      @emitter = opts[:Emitter].new(stream,opts)
      @resolver = opts[:Resolver].new
      @serializer = opts[:Serializer].new(@emitter,@resolver,opts)
      @representer = opts[:Representer].new(@serializer,opts)
    end
  end
  
  class BaseDumper < CommonDumper
    attr_accessor 
    def initialize(stream,opts={})
      super(stream,DEFAULTS.merge({:Representer=>RbYAML::BaseRepresenter, :Resolver=>RbYAML::BaseResolver}).merge(opts))
    end
  end
  
  class SafeDumper < CommonDumper
    def initialize(stream,opts={})
      super(stream,DEFAULTS.merge({:Representer=>RbYAML::SafeRepresenter}).merge(opts))
      super
    end
  end
  
  class Dumper < CommonDumper
    def initialize(stream,opts={})
      super
    end
  end
end
