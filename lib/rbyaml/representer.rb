require 'set'
require 'date'

require 'rbyaml/error'
require 'rbyaml/nodes'

module RbYAML
  class RepresenterError < YAMLError
  end

  class BaseRepresenter
    def initialize(serializer, opts={})
      @serializer = serializer
      @default_style = opts[:UseDouble] ? '"' : (opts[:UseSingle] ? "'" : nil)
      @represented_objects = {}
    end

    def represent(data)
      node = represent_data(data)
      @serializer.serialize(node)
      represented_objects = {}
    end
 
    def represent_data(data)
      if ignore_aliases(data)
        alias_key = nil
      else
        alias_key = data.object_id
      end
      
      if !alias_key.nil?
        if @represented_objects.include?(alias_key)
          node = @represented_objects[alias_key]
          raise RepresenterError.new("recursive objects are not allowed: #{data}") if node.nil?
          return node
        end
        @represented_objects[alias_key] = nil
      end
      node = data.to_yaml_node(self)
      @represented_objects[alias_key] = node if !alias_key.nil?
      node
    end
    
    def scalar(tag, value, style=nil)
      represent_scalar(tag,value,style)
    end

    def represent_scalar(tag, value, style=nil)
      style ||= @default_style
      ScalarNode.new(tag,value,style)
    end

    def seq(tag, sequence, flow_style=nil)
      represent_sequence(tag,sequence,flow_style)
    end

    def represent_sequence(tag, sequence, flow_style=nil)
      best_style = false
      value = sequence.map {|item|
        node_item = represent_data(item)
        best_style = false if !node_item.__is_scalar && !node_item.flow_style
        node_item
      }
      flow_style ||= (@flow_default_style || best_style)
      SequenceNode.new(tag, value, flow_style)
    end

    def map(tag, *args)
      mapping, flow_style = case args.length
                            when 1 then
                              mapping = {}
                              def mapping.add(key, value) self[key] = value end
                              yield mapping
                              [mapping, args.first]
                            when 2 then
                              args
                            else
                              raise ArgumentError,
                                    "wrong number of arguments (#{args.length})"
                            end

      represent_mapping(tag,mapping,flow_style)
    end

    def represent_mapping(tag, mapping, flow_style=nil)
      best_style = false
      if mapping.respond_to?(:keys)
        value = {}
        for item_key,item_value in mapping
          node_key = represent_data(item_key)
          node_value = represent_data(item_value)
          best_style = false if !node_key.__is_scalar && !node_key.flow_style
          best_style = false if !node_value.__is_scalar && !node_value.flow_style
          value[node_key] = node_value
        end
      else
        value = []
        for item_key, item_value in mapping
          node_key = represent_data(item_key)
          node_value = represent_data(item_value)
          best_style = false if !node_key.__is_scalar && !node_key.flow_style
          best_style = false if !node_value.__is_scalar && !node_value.flow_style
          value << [node_key, node_value]
        end
      end
      flow_style ||= (@flow_default_style || best_style)
      MappingNode.new(tag, value, flow_style)
    end

    def ignore_aliases(data)
      false
    end
  end

  class SafeRepresenter < BaseRepresenter
    def ignore_aliases(data)
      data.nil? || data.__is_str || TrueClass === data || FalseClass === data || data.__is_int || Float === data
    end
  end

  class Representer < SafeRepresenter
  end
end
