namespace rubinius {
  struct FinalizeObject {
  public:
    enum FinalizationStatus {
      eLive,
      eQueued,
      eFinalized
    };

  public:
    FinalizeObject()
      : queue_count(0)
    {}

    Object* object;
    FinalizationStatus status;
    int queue_count;

    void queued() {
      status = eQueued;
    }
  };
}
