#
# YAML::Syck module
# .. glues syck and yaml.rb together ..
#
require 'syck/ext/syck'
require 'yaml/basenode'

module YAML
    module Syck

        #
        # Mixin BaseNode functionality
        #
        class Node
            include YAML::BaseNode
        end

        #--
        # For Rubinius, replaces the rb_iterate call to syck_set_ivars.
        #++
        def self.set_ivars(hsh, obj)
          hsh.each do |key, value|
            obj.instance_variable_set :"@#{key}", value
          end
        end

        #--
        # For Rubinius, replaces the rb_iterate call to syck_merge_i.
        #++
        def self.merge_i(ary, hsh)
          ary.each do |entry|
            begin
              entry = Rubinius::Type.coerce_to entry, Hash, :to_hash
              hsh.update entry
            rescue
              # ignore coercion errors
            end
          end

          nil
        end

    end
end
