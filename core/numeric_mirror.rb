module Rubinius
  class Mirror
    class Numeric < Mirror
      self.subject = ::Numeric

      def step_float_size(value, limit, step, asc)
        if (asc && value > limit) || (!asc && value < limit)
          return 0
        end

        if step.infinite?
          1
        else
          err = (value.abs + limit.abs + (limit - value).abs) / step.abs * Float::EPSILON
          if err.finite?
            err = 0.5 if err > 0.5
            ((limit - value) / step + err).floor + 1
          else
            0
          end
        end
      end

      def step_size(limit, step, to, by)
        values = step_fetch_args(limit, step, to, by)
        value = values[0]
        limit = values[1]
        step = values[2]
        asc = values[3]
        is_float = values[4]

        if limit == Float::INFINITY
          if step == Float::INFINITY
            return 1
          else
            return Float::INFINITY
          end
        end

        return Float::INFINITY if step == 0

        if is_float
          # Ported from MRI

          step_float_size(value, limit, step, asc)

        else
          if (asc && value > limit) || (!asc && value < limit)
            0
          else
            ((value - limit).abs + 1).fdiv(step.abs).ceil
          end
        end
      end

      def step_fetch_args(limit, step, to, by)
        raise ArgumentError, "limit is given twice" if limit && to
        raise ArgumentError, "step is given twice" if step && by
        raise ArgumentError, "step cannot be 0" if step == 0

        step ||= by || 1
        asc = step > 0
        limit ||= to || (asc ? Float::INFINITY : -Float::INFINITY)
        value = @object
        if value.kind_of? Float or limit.kind_of? Float or step.kind_of? Float
          return FloatValue(value), FloatValue(limit), FloatValue(step), asc, true
        else
          return value, limit, step, asc, false
        end
      end

    end
  end
end
