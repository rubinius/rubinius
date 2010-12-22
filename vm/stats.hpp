namespace rubinius {
  struct Stats {
    size_t jitted_methods;
    size_t jit_time_spent;
    double verification_time;

    Stats()
      : jitted_methods(0)
      , jit_time_spent(0)
      , verification_time(0)
    {}
  };
}
