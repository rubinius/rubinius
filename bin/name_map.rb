class NameMap
  MAP = {
    '`'   => 'backtick',
    '+'   => 'plus',
    '-'   => 'minus',
    '+@'  => 'uplus',
    '-@'  => 'uminus',
    '*'   => 'multiply',
    '/'   => 'divide',
    '%'   => 'modulo',
    '<<'  => {'Bignum' => 'left_shift', 
              'Fixnum' => 'left_shift',
              'IO'     => 'output',
              :default => 'append' },
    '>>'  => 'right_shift',
    '<'   => 'lt',
    '<='  => 'lte',
    '>'   => 'gt',
    '>='  => 'gte',
    '='   => 'assignment',
    '=='  => 'equal_value',
    '===' => 'case_compare',
    '<=>' => 'comparison',
    '[]'  => 'element_reference',
    '[]=' => 'element_set',
    '**'  => 'exponent',
    '!'   => 'not',
    '~'   => {'Bignum' => 'complement',
              'Fixnum' => 'complement',
              'Regexp' => 'match',
              'String' => 'match' },
    '!='  => 'not_equal',
    '!~'  => 'not_match',
    '=~'  => 'match',
    '&'   => {'Bignum'     => 'bit_and',
              'Fixnum'     => 'bit_and',
              'Array'      => 'intersection',
              'TrueClass'  => 'and',
              'FalseClass' => 'and', 
              'NilClass'   => 'and',
              'Set'        => 'intersection' },
    '|'   => {'Bignum'     => 'bit_or',
              'Fixnum'     => 'bit_or',
              'Array'      => 'union',
              'TrueClass'  => 'or',
              'FalseClass' => 'or',
              'NilClass'   => 'or',
              'Set'        => 'union' },
    '^'   => {'Bignum'     => 'bit_xor',
              'Fixnum'     => 'bit_xor',
              'TrueClass'  => 'xor',
              'FalseClass' => 'xor',
              'NilClass'   => 'xor',
              'Set'        => 'exclusion'},
    'initialize' => 'new'
  }

  def self.const_lookup(c)
    c.split('::').inject(Object) { |k,n| k.const_get n }
  end
    
  def self.get_class_or_module(c)
    const = const_lookup(c)
    return const if (const.is_a?(Module) or const.is_a?(Class)) and 
      not ['DTracer', 'NameMap', 'OptionParser', 'SystemExit'].include?(const.name)
  end

  def self.get_dir_name(c, base='spec/ruby/1.8/core/')
    base = 'spec/ruby/1.8/core/' unless base
    name = c.name.split('::').last
    if (name.include?('Error') or name == 'Errno') and c.ancestors.include?(Exception)
      base + 'exception'
    elsif c == Class
      base + name.downcase
    else
      base + name.gsub(/Class/, '').downcase
    end
  end

  def self.get_spec_name(m, c)
    if MAP.key?(m)
      name = MAP[m].is_a?(Hash) ? MAP[m][c] || MAP[m][:default] : MAP[m]
    else
      name = m.gsub(/[?!=]/, '')
    end
    "#{name}_spec.rb"
  end
end
